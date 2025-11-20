#
# Copyright (c) 2025-2026 Texas Instruments Incorporated - https://www.ti.com
#
# SPDX-License-Identifier: BSD-3-Clause
#

INCLUDES += -Idrivers/ti/common/include \
	    -Idrivers/ti/common/pm/include \
	    -Idrivers/ti/clk/include

BL31_SOURCES += drivers/ti/clk/ti_clk_mux.c \
		drivers/ti/clk/ti_clk_div.c \
		drivers/ti/clk/ti_clk_fixed.c
