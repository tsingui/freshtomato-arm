#
# Makefile for the Broadcom CTF
#
# Copyright (C) 2014, Broadcom Corporation. All Rights Reserved.
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# $Id: Makefile,v 1.1 2009-10-27 01:27:44 $
#

obj-${CONFIG_BCM_CTF} := ctf.o

CTFSRC := ../../../../../ctf
CTFOBJ := $(CTFSRC)/hndctf.o $(CTFSRC)/hndctf_linux.o
SYMOBJ := $(CTFSRC)/hndctf.o

ifeq ($(CONFIG_BUZZZ_FUNC),y)
subdir-ccflags-y += -O0 -fno-inline -finstrument-functions
endif # CONFIG_BUZZZ_FUNC

# Search for sources under src/router/ctf
ifneq (,$(wildcard $(src)/$(CTFSRC)/*.c))
  EXTRA_CFLAGS += -I$(src)/$(CTFSRC)/../../ctf -DHNDCTF
  ctf-objs := $(CTFOBJ) ctf_ksyms.o
else
ifneq ($(CONFIG_SMP),y)
  ctf-objs := $(CTFSRC)/up/linux/ctf.o
else
  ctf-objs := $(CTFSRC)/linux/ctf.o
endif
endif

ifeq (,${KERNELRELEASE})
  # Normal makefile
  KERNELDIR := $(BASEDIR)/src-rt-7.x.main/src/linux/linux-2.6.36

  .PHONY: all clean
  all clean :
	  +$(MAKE) -C $(KERNELDIR) M=`pwd` $@
endif

PHONY += $(src)/ctf_ksyms.c

quiet_cmd_ksyms = CALL    $<
      cmd_ksyms = $(CONFIG_SHELL) -e $< $(src)/$(SYMOBJ) > $@

$(src)/ctf_ksyms.c: $(src)/ctf_ksyms.sh $(src)/$(SYMOBJ) FORCE
	$(call cmd,ksyms)

clean-files += $(CTFSRC)/*.o $(CTFSRC)/.*.o.cmd
