target_cpu = arm

MODULE := zcommon

obj-$(CONFIG_ZFS) := $(MODULE).o

ccflags-y := $(ZFS_MODULE_CFLAGS) $(ZFS_MODULE_CPPFLAGS)

# Suppress unused-value warnings in sparc64 architecture headers
ifeq ($(target_cpu),sparc64)
ccflags-y += -Wno-unused-value
endif

$(MODULE)-objs += zfeature_common.o
$(MODULE)-objs += zfs_comutil.o
$(MODULE)-objs += zfs_deleg.o
$(MODULE)-objs += zfs_fletcher.o
$(MODULE)-objs += zfs_fletcher_superscalar.o
$(MODULE)-objs += zfs_fletcher_superscalar4.o
$(MODULE)-objs += zfs_namecheck.o
$(MODULE)-objs += zfs_prop.o
$(MODULE)-objs += zfs_uio.o
$(MODULE)-objs += zpool_prop.o
$(MODULE)-objs += zprop_common.o

$(MODULE)-$(CONFIG_X86) += zfs_fletcher_intel.o
$(MODULE)-$(CONFIG_X86) += zfs_fletcher_sse.o
$(MODULE)-$(CONFIG_X86) += zfs_fletcher_avx512.o
$(MODULE)-$(CONFIG_ARM64) += zfs_fletcher_aarch64_neon.o
