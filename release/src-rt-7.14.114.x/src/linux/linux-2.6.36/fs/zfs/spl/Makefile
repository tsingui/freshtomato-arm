
MODULE := spl

obj-$(CONFIG_ZFS) := $(MODULE).o

ccflags-y := $(ZFS_MODULE_CFLAGS) $(ZFS_MODULE_CPPFLAGS)

$(MODULE)-objs += spl-atomic.o
$(MODULE)-objs += spl-condvar.o
$(MODULE)-objs += spl-cred.o
$(MODULE)-objs += spl-err.o
$(MODULE)-objs += spl-generic.o
$(MODULE)-objs += spl-kmem.o
$(MODULE)-objs += spl-kmem-cache.o
$(MODULE)-objs += spl-kobj.o
$(MODULE)-objs += spl-kstat.o
$(MODULE)-objs += spl-proc.o
$(MODULE)-objs += spl-procfs-list.o
$(MODULE)-objs += spl-taskq.o
$(MODULE)-objs += spl-thread.o
$(MODULE)-objs += spl-tsd.o
$(MODULE)-objs += spl-vmem.o
$(MODULE)-objs += spl-vnode.o
$(MODULE)-objs += spl-xdr.o
$(MODULE)-objs += spl-zlib.o
