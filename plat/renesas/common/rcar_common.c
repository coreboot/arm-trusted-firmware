/*
 * Copyright (c) 2019-2026, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <arch_helpers.h>
#include <drivers/console.h>
#include <lib/xlat_tables/xlat_mmu_helpers.h>
#include <plat/common/platform.h>
#include <rcar_private.h>

#include <lib/mmio.h>
#include <cpg_registers.h>

#define MSTP318			(1 << 18)
#define MSTP319			(1 << 19)
#define PMSR			0x5c
#define PMSR_L1FAEG		(1U << 31)
#define PMSR_PMEL1RX		(1 << 23)
#define PMCTLR			0x60
#define PMSR_L1IATN		(1U << 31)

static int rcar_pcie_fixup(unsigned int controller)
{
	uint32_t rcar_pcie_base[] = { 0xfe011000, 0xee811000 };
	uint32_t addr = rcar_pcie_base[controller];
	uint32_t cpg, pmsr;
	int ret = 0;

	/* Test if PCIECx is enabled */
	cpg = mmio_read_32(CPG_MSTPSR3);
	if (cpg & (MSTP318 << !controller))
		return ret;

	pmsr = mmio_read_32(addr + PMSR);

	if ((pmsr & PMSR_PMEL1RX) && ((pmsr & 0x70000) != 0x30000)) {
		/* Fix applicable */
		mmio_write_32(addr + PMCTLR, PMSR_L1IATN);
		while (!(mmio_read_32(addr + PMSR) & PMSR_L1FAEG))
			;
		mmio_write_32(addr + PMSR, PMSR_L1FAEG | PMSR_PMEL1RX);
		ret = 1;
	}

	return ret;
}

/* RAS functions common to AArch64 ARM platforms */
void plat_ea_handler(unsigned int ea_reason, uint64_t syndrome, void *cookie,
		void *handle, uint64_t flags)
{
	unsigned int fixed = 0;

	fixed |= rcar_pcie_fixup(0);
	fixed |= rcar_pcie_fixup(1);

	if (fixed)
		return;

	plat_default_ea_handler(ea_reason, syndrome, cookie, handle, flags);
}

#include <drivers/renesas/rcar/console/console.h>

void rcar_console_boot_init(void)
{
	console_renesas_register(0, 0, 0, CONSOLE_FLAG_BOOT);
}

void rcar_console_runtime_init(void)
{
	console_renesas_register(1, 0, 0, CONSOLE_FLAG_RUNTIME);
}

static uint32_t rcar_product_id(void)
{
	static uint32_t rcar_prod_id_num;
	uint32_t rcar_prod_indent;
	uint32_t product;

	if (rcar_prod_id_num > 0)
		return rcar_prod_id_num;

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;

	switch (product) {
	case PRR_PRODUCT_H3:
		rcar_prod_id_num = PRODUCT_ID_H3;
		break;
	case PRR_PRODUCT_M3:
		rcar_prod_id_num = PRODUCT_ID_M3;
		break;
	case PRR_PRODUCT_D3:
		rcar_prod_id_num = PRODUCT_ID_D3;
		break;
	case PRR_PRODUCT_E3:
		rcar_prod_id_num = PRODUCT_ID_E3;
		break;
	case PRR_PRODUCT_M3N:
		rcar_prod_indent = mmio_read_32(RCAR_M3NM3L_IDENT);

		if (rcar_prod_indent == RCAR_M3N_IDENT_VAL) {
			rcar_prod_id_num = PRODUCT_ID_M3N;
		} else {
			rcar_prod_id_num = PRODUCT_ID_M3L;
		}
		break;
	default:
		break;
	}

	return rcar_prod_id_num;
}

bool is_rcar_product(uint32_t product_id)
{
	return rcar_product_id() == product_id;
}
