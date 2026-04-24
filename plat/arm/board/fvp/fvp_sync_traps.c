/*
 * Copyright (c) 2022-2026, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This file just contains demonstration code, to "handle" RNG traps.
 */

#include <stdbool.h>

#include <arch.h>
#include <arch_helpers.h>
#include <bl31/sync_handle.h>
#include <context.h>

/*
 * This emulation code here is not very meaningful: enabling the RNG
 * trap typically happens for a reason, so just calling the actual
 * hardware instructions might not be useful or even possible.
 */
#if ENABLE_FEAT_RNG_TRAP
int plat_handle_rng_trap(uint8_t rt, bool rndrrs, cpu_context_t *ctx)
{
	if (rndrrs) {
		ctx->gpregs_ctx.ctx_regs[rt] = read_rndrrs();
	} else {
		ctx->gpregs_ctx.ctx_regs[rt] = read_rndr();
	}

	return TRAP_RET_CONTINUE;
}
#endif
