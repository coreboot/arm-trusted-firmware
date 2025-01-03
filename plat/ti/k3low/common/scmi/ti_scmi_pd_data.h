/*
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SCMI_PD_DATA_H
#define SCMI_PD_DATA_H

#include <stddef.h>
#include <stdint.h>

#include <ti_devices.h>

struct ti_power_domain {
	char *name;
	uint32_t id;
};

extern struct ti_power_domain scmi_power_domains[];
extern const size_t scmi_power_domains_count;

#endif /* SCMI_PD_DATA_H */
