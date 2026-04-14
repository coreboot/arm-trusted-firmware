/*
 * Copyright (c) 2025-2026, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IDTE3_H
#define IDTE3_H

#ifdef IMAGE_BL31
#include <arch.h>
#include <bl31/sync_handle.h>
#include <context.h>
#include <lib/el3_runtime/cpu_data.h>

#if ENABLE_FEAT_IDTE3
void idte3_enable(cpu_context_t *ctx);
int handle_idreg_trap(uint8_t rt, uint64_t idreg, cpu_context_t *ctx,
		      u_register_t flags);
void idte3_init_cached_idregs_per_world(size_t security_state);
void idte3_init_percpu_once_regs(size_t security_state);
#else
static inline void idte3_enable(cpu_context_t *ctx)
{
}
static inline int handle_idreg_trap(uint8_t rt, uint64_t idreg,
				    cpu_context_t *ctx, u_register_t flags)
{
	return TRAP_RET_UNHANDLED;
}
static inline void idte3_init_percpu_once_regs(size_t security_state)
{
}
static inline void idte3_init_cached_idregs_per_world(size_t security_state)
{
}
#endif /* ENABLE_FEAT_IDTE3 */
#endif /* IMAGE_BL31 */
#endif /* IDTE3_H */
