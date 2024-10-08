/*
 * Copyright (c) 2017-2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
#include <cortex_a12.h>
#include <cpu_macros.S>

	.macro assert_cache_enabled
#if ENABLE_ASSERTIONS
		ldcopr	r0, SCTLR
		tst	r0, #SCTLR_C_BIT
		ASM_ASSERT(eq)
#endif
	.endm

func cortex_a12_disable_smp
	ldcopr	r0, ACTLR
	bic	r0, #CORTEX_A12_ACTLR_SMP_BIT
	stcopr	r0, ACTLR
	isb
	dsb	sy
	bx	lr
endfunc cortex_a12_disable_smp

func cortex_a12_enable_smp
	ldcopr	r0, ACTLR
	orr	r0, #CORTEX_A12_ACTLR_SMP_BIT
	stcopr	r0, ACTLR
	isb
	bx	lr
endfunc cortex_a12_enable_smp

func cortex_a12_reset_func
	b	cortex_a12_enable_smp
endfunc cortex_a12_reset_func

func cortex_a12_core_pwr_dwn
	push	{r12, lr}

	assert_cache_enabled

	/* Flush L1 cache */
	mov	r0, #DC_OP_CISW
	bl	dcsw_op_level1

	/* Exit cluster coherency */
	pop	{r12, lr}
	b	cortex_a12_disable_smp
endfunc cortex_a12_core_pwr_dwn

func cortex_a12_cluster_pwr_dwn
	push	{r12, lr}

	assert_cache_enabled

	/* Flush L1 caches */
	mov	r0, #DC_OP_CISW
	bl	dcsw_op_level1

	bl	plat_disable_acp

	/* Flush L2 caches */
	mov	r0, #DC_OP_CISW
	bl	dcsw_op_level2

	/* Exit cluster coherency */
	pop	{r12, lr}
	b	cortex_a12_disable_smp
endfunc cortex_a12_cluster_pwr_dwn

declare_cpu_ops cortex_a12, CORTEX_A12_MIDR, \
	cortex_a12_reset_func, \
	cortex_a12_core_pwr_dwn, \
	cortex_a12_cluster_pwr_dwn
