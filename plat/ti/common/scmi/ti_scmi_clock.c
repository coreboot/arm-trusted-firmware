/*
 * Copyright (C) 2026 Texas Instruments Incorporated - https://www.ti.com/
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <drivers/clk.h>
#include <drivers/scmi-msg.h>
#include <drivers/scmi.h>
#include <lib/utils_def.h>
#include <ti_device.h>
#include <ti_device_clk.h>
#include <ti_sci.h>

#include <scmi_clock.h>
#include <ti_clocks.h>
#include <ti_devices.h>
#include <ti_plat_scmi_def.h>
#include <ti_scmi_clk_data.h>

struct ti_scmi_clock *ti_scmi_get_clock(uint32_t agent_id __unused,
				   uint32_t clock_id)
{
	struct ti_scmi_clock *table = NULL;

	if (clock_id < clock_table_size) {
		table = &clock_table[clock_id];
	}

	return table;
}

size_t plat_scmi_clock_count(unsigned int agent_id __unused)
{
	return clock_table_size;
}

int32_t plat_scmi_clock_set_rate(unsigned int agent_id __unused,
				 unsigned int scmi_id,
				 unsigned long rate)
{
	unsigned long orate;
	int32_t ret;

	VERBOSE("scmi_clock_set_rate scmi_id = %u rate = %lu\n", scmi_id, rate);

	if (scmi_id >= clock_table_size) {
		return SCMI_NOT_FOUND;
	}

	ret = clk_set_rate(scmi_id, rate, &orate);
	if (ret != 0) {
		WARN("%s: Failed to set freq with scmi_id = %u rate = %lu\n",
		     __func__, scmi_id, rate);
		return SCMI_DENIED;
	}

	return SCMI_SUCCESS;
}

const char *plat_scmi_clock_get_name(unsigned int agent_id,
				     unsigned int scmi_id)
{
	struct ti_scmi_clock *clock;

	clock = ti_scmi_get_clock(agent_id, scmi_id);
	if (clock != NULL) {
		return clock->name;
	}

	return NULL;
}

int32_t plat_scmi_clock_rates_by_step(unsigned int agent_id,
				      unsigned int scmi_id,
				      unsigned long *steps)
{
	struct ti_scmi_clock *clock;

	clock = ti_scmi_get_clock(agent_id, scmi_id);
	if (clock == NULL) {
		return SCMI_NOT_SUPPORTED;
	}

	assert(clock->rates != NULL);
	steps[0] = clock->rates[0];
	steps[1] = clock->rates[1];
	steps[2] = clock->rates[2];

	return SCMI_SUCCESS;
}

unsigned long plat_scmi_clock_get_rate(unsigned int agent_id __unused,
				       unsigned int scmi_id)
{
	if (scmi_id >= clock_table_size) {
		return 0;
	}

	return clk_get_rate(scmi_id);
}

int32_t plat_scmi_clock_get_state(unsigned int agent_id __unused,
				  unsigned int scmi_id)
{
	if (scmi_id >= clock_table_size) {
		return SCMI_NOT_SUPPORTED;
	}

	return clk_is_enabled(scmi_id) ? 1 : 0;
}

int32_t plat_scmi_clock_set_state(unsigned int agent_id __unused,
				  unsigned int scmi_id,
				  bool enable)
{
	int32_t ret;

	if (scmi_id >= clock_table_size) {
		return SCMI_NOT_SUPPORTED;
	}

	VERBOSE("%s: agent_id = %u, scmi_id = %u, enable: %d\n",
		__func__, agent_id, scmi_id, (int)enable);

	if (enable) {
		ret = clk_enable(scmi_id);
		if (ret != 0) {
			WARN("%s: Failed to enable clock scmi_id = %u\n",
			     __func__, scmi_id);
			return SCMI_DENIED;
		}
	} else {
		clk_disable(scmi_id);
	}

	return SCMI_SUCCESS;
}

int32_t plat_scmi_clock_get_possible_parents(unsigned int agent_id __unused,
					     unsigned int scmi_id,
					     unsigned int *plat_possible_parents,
					     size_t *nb_elts,
					     uint32_t skip_parents __unused)
{
	int encoded_parents;
	uint32_t mux_dev_id;
	size_t total_parents, reserved_parents, non_reserved_parents;
	size_t parent_idx = 0;
	size_t reserved_found = 0;

	VERBOSE("scmi_clock_get_possible_parents agent_id = %u, scmi_id = %u\n",
		agent_id, scmi_id);

	if (scmi_id >= clock_table_size) {
		return SCMI_NOT_FOUND;
	}

	encoded_parents = clk_get_possible_parents_num(scmi_id);
	total_parents = (size_t)(encoded_parents & 0xFF);
	reserved_parents = (size_t)((encoded_parents >> 8) & 0xFF);
	non_reserved_parents = total_parents - reserved_parents;

	*nb_elts = (total_parents > 1U) ? total_parents : 0U;

	if (plat_possible_parents != NULL) {
		if (*nb_elts > 1U) {
			mux_dev_id = clock_table[scmi_id].dev_id;

			for (size_t i = 0U; i < non_reserved_parents; i++) {
				plat_possible_parents[parent_idx] =
					scmi_id - non_reserved_parents + i;
				VERBOSE("%s: non-reserved parent[%zu] = %u\n",
					__func__, parent_idx,
					plat_possible_parents[parent_idx]);
				parent_idx++;
			}

			if (reserved_parents > 0U) {
				for (long i = (long)clock_table_size - 1L;
				     i >= (long)scmi_id &&
				     reserved_found < reserved_parents; i--) {
					if ((clock_table[i].dev_id == mux_dev_id) &&
					    ((size_t)i != (size_t)scmi_id)) {
						plat_possible_parents[parent_idx] = (unsigned int)i;
						VERBOSE("%s: reserved parent[%zu] = %ld (dev=%u)\n",
							__func__, parent_idx, i,
							clock_table[i].dev_id);
						parent_idx++;
						reserved_found++;
					}
				}
			}
		} else {
			*nb_elts = 0U;
			VERBOSE("%s: 0 possible parents\n", __func__);
		}
	} else {
		*nb_elts = (*nb_elts > 1U) ? *nb_elts : 0U;
		VERBOSE("%s: plat_possible_parents is NULL, *nb_elts = %zu\n",
			__func__, *nb_elts);
	}

	VERBOSE("num_parents %zu (non-reserved=%zu, reserved=%zu)\n",
		*nb_elts, non_reserved_parents, reserved_parents);

	return SCMI_SUCCESS;
}

int32_t plat_scmi_clock_get_parent(unsigned int agent_id __unused,
				   unsigned int scmi_id,
				   unsigned int *parent_id)
{
	int tmp_parent;

	VERBOSE("scmi_clock_get_parent agent_id = %u, scmi_id = %u\n",
		agent_id, scmi_id);

	if (scmi_id >= clock_table_size) {
		return SCMI_NOT_FOUND;
	}

	tmp_parent = clk_get_parent(scmi_id);
	if (tmp_parent < 0) {
		ERROR("Failed to get clock parent for scmi_id %u\n", scmi_id);
		return SCMI_GENERIC_ERROR;
	}

	*parent_id = (unsigned int)tmp_parent;
	VERBOSE("scmi_clock_get_parent parent_id = %u\n", *parent_id);

	return SCMI_SUCCESS;
}

int32_t plat_scmi_clock_set_parent(unsigned int agent_id __unused,
				   unsigned int scmi_id,
				   unsigned int parent_id)
{
	int32_t status;

	VERBOSE("%s: agent_id = %u, scmi_id = %u parent_id = %u\n", __func__,
		agent_id, scmi_id, parent_id);

	if ((scmi_id >= clock_table_size) || (parent_id >= clock_table_size)) {
		return SCMI_NOT_FOUND;
	}

	status = clk_set_parent(scmi_id, parent_id);
	if (status != 0) {
		return SCMI_GENERIC_ERROR;
	}

	return SCMI_SUCCESS;
}
