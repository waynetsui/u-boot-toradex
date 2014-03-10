#
# (C) Copyright 2013 Freescale Semiconductor, Inc.
# (C) Copyright 2014 Toradex AG
#
# SPDX-License-Identifier:	GPL-2.0+
#

include $(TOPDIR)/config.mk

LIB    = $(obj)lib$(BOARD).o

COBJS  := apalis_imx6.o pf0100.o do_fuse.o

SRCS   := $(COBJS:.o=.c)
OBJS   := $(addprefix $(obj),$(COBJS))

$(LIB):        $(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
