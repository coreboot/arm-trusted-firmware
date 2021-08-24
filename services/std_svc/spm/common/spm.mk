#
# Copyright (c) 2021, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifneq (${ARCH},aarch64)
        $(error "Error: SPM is only supported on aarch64.")
endif

INCLUDES	+=	-Iservices/std_svc/spm/common/include

SPM_SOURCES	:=	$(addprefix services/std_svc/spm/common/,\
			${ARCH}/spm_helpers.S			\
			${ARCH}/spm_shim_exceptions.S		\
			spm_common.c				\
			spm_setup.c				\
			spm_xlat.c)
			# logical_mm_sp.c)


# Let the top-level Makefile know that we intend to include a BL32 image
NEED_BL32		:=	yes
