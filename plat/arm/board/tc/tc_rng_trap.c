/*
 * Copyright (c) 2025-2026, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <bl31/sync_handle.h>
#include <context.h>
#include <plat/common/plat_trng.h>

int plat_handle_rng_trap(uint8_t rt, bool rndrrs, cpu_context_t *ctx)
{
	uint64_t entropy;

	if (!plat_get_entropy(&entropy)) {
		ERROR("Failed to get entropy\n");
		panic();
	}

	/* Emulate RNDR and RNDRRS */
	gp_regs_t *gpregs = get_gpregs_ctx(ctx);

	gpregs->ctx_regs[rt] = entropy;

	return TRAP_RET_CONTINUE;
}
