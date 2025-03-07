
#include <linux/types.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/clkdev.h>

#include <mach/clkdev.h>
#include <mach/hardware.h>
#include <mach/memory.h>
#include <mach/io_map.h>

#include <plat/bsp.h>
#include <plat/mpcore.h>
#include <plat/plat-bcm5301x.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/romfs_fs.h>
#include <linux/cramfs_fs.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#include <linux/squashfs_fs.h>
#else
/* #include <magic.h> */
#endif
#endif

#include <typedefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <bcmnvram.h>
#include <bcmendian.h>
#include <hndsoc.h>
#include <siutils.h>
#include <hndcpu.h>
#include <hndpci.h>
#include <pcicfg.h>
#include <bcmdevs.h>
#include <trxhdr.h>
#ifdef HNDCTF
#include <ctf/hndctf.h>
#endif /* HNDCTF */
#include <hndsflash.h>
#ifdef CONFIG_MTD_NFLASH
#include <hndnand.h>
#endif

extern char __initdata saved_root_name[];

/* Global SB handle */
si_t *bcm947xx_sih = NULL;
spinlock_t bcm947xx_sih_lock = SPIN_LOCK_UNLOCKED;
EXPORT_SYMBOL(bcm947xx_sih);
EXPORT_SYMBOL(bcm947xx_sih_lock);

/* Convenience */
#define sih bcm947xx_sih
#define sih_lock bcm947xx_sih_lock

#define WATCHDOG_MIN    3000    /* milliseconds */
extern int panic_timeout;
extern int panic_on_oops;
static int watchdog = 0;

#ifdef HNDCTF
ctf_t *kcih = NULL;
EXPORT_SYMBOL(kcih);
ctf_attach_t ctf_attach_fn = NULL;
EXPORT_SYMBOL(ctf_attach_fn);
#endif /* HNDCTF */


struct dummy_super_block {
	u32	s_magic ;
};

/* This is the main reference clock 25MHz from external crystal */
static struct clk clk_ref = {
	.name = "Refclk",
	.rate = 25 * 1000000,	/* run-time override */
	.fixed = 1,
	.type	= CLK_XTAL,
};


static struct clk_lookup board_clk_lookups[] = {
	{
	.con_id		= "refclk",
	.clk		= &clk_ref,
	}
};

extern int _memsize;

void __init board_map_io(void)
{
early_printk("board_map_io\n");
	/* Install clock sources into the lookup table */
	clkdev_add_table(board_clk_lookups, 
			ARRAY_SIZE(board_clk_lookups));

	/* Map SoC specific I/O */
	soc_map_io( &clk_ref );
}


void __init board_init_irq(void)
{
early_printk("board_init_irq\n");
	soc_init_irq();
	
	/* serial_setup(sih); */
}

void board_init_timer(void)
{
early_printk("board_init_timer\n");
	soc_init_timer();
}

static int __init rootfs_mtdblock(void)
{
	int bootdev;
	int knldev;
	int block = 0;
#ifdef CONFIG_FAILSAFE_UPGRADE
	char *img_boot = nvram_get(BOOTPARTITION);
#endif

	bootdev = soc_boot_dev((void *)sih);
	knldev = soc_knl_dev((void *)sih);

	/* NANDBOOT */
	if (bootdev == SOC_BOOTDEV_NANDFLASH &&
	    knldev == SOC_KNLDEV_NANDFLASH) {
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (img_boot && simple_strtol(img_boot, NULL, 10))
			return 5;
		else
			return 3;
#else
		return 3;
#endif
	}

	/* SFLASH/PFLASH only */
	if (bootdev != SOC_BOOTDEV_NANDFLASH &&
	    knldev != SOC_KNLDEV_NANDFLASH) {
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (img_boot && simple_strtol(img_boot, NULL, 10))
			return 4;
		else
			return 2;
#else
		return 2;
#endif
	}

#ifdef BCMCONFMTD
	block++;
#endif
#ifdef CONFIG_FAILSAFE_UPGRADE
	if (img_boot && simple_strtol(img_boot, NULL, 10))
		block += 2;
#endif
	/* Boot from norflash and kernel in nandflash */
	return block+3;
}

static void __init brcm_setup(void)
{
	/* Get global SB handle */
	sih = si_kattach(SI_OSH);

	if (strncmp(boot_command_line, "root=/dev/mtdblock", strlen("root=/dev/mtdblock")) == 0)
		sprintf(saved_root_name, "/dev/mtdblock%d", rootfs_mtdblock());

	/* Set watchdog interval in ms */
        watchdog = simple_strtoul(nvram_safe_get("watchdog"), NULL, 0);

	/* Ensure at least WATCHDOG_MIN */
	if ((watchdog > 0) && (watchdog < WATCHDOG_MIN))
		watchdog = WATCHDOG_MIN;

	/* Set panic timeout in seconds */
	panic_timeout = watchdog / 1000;
	panic_on_oops = watchdog / 1000;
}

void soc_watchdog(void)
{
	if (watchdog > 0)
		si_watchdog_ms(sih, watchdog);
}

#define CFE_UPDATE 1            /* added by Chen-I for mac/regulation update */

#ifdef CFE_UPDATE
void bcm947xx_watchdog_disable(void)
{
	watchdog=0;
	si_watchdog_ms(sih, 0);
}
#endif

void __init board_init(void)
{
early_printk("board_init\n");
	brcm_setup();
	/*
	 * Add common platform devices that do not have board dependent HW
	 * configurations
	 */
	soc_add_devices();

	return;
}

static void __init board_fixup(
	struct machine_desc *desc,
	struct tag *t,
	char **cmdline,
	struct meminfo *mi
	)
{
	u32 mem_size, lo_size ;
        early_printk("board_fixup\n" );

	/* Fuxup reference clock rate */
	if (desc->nr == MACH_TYPE_BRCM_NS_QT )
		clk_ref.rate = 17594;	/* Emulator ref clock rate */


	mem_size = _memsize;

	early_printk("board_fixup: mem=%uMiB\n", mem_size >> 20);

	lo_size = min(mem_size, DRAM_MEMORY_REGION_SIZE);

	mi->bank[0].start = PHYS_OFFSET;
	mi->bank[0].size = lo_size;
	mi->nr_banks++;

	if (lo_size == mem_size)
		return;

#ifdef CONFIG_DRAM_512M
	mi->bank[1].start = DRAM_LARGE_REGION_BASE + lo_size;
#else
	mi->bank[1].start = PHYS_OFFSET2;
#endif  /* CONFIG_DRAM_512M */
	mi->bank[1].size = mem_size - lo_size;
	mi->nr_banks++;
}

#ifdef CONFIG_ZONE_DMA
/*
 * Adjust the zones if there are restrictions for DMA access.
 */
void __init bcm47xx_adjust_zones(unsigned long *size, unsigned long *hole)
{
	unsigned long dma_size = SZ_128M >> PAGE_SHIFT;

	if (size[0] <= dma_size)
		return;

	size[ZONE_NORMAL] = size[0] - dma_size;
	size[ZONE_DMA] = dma_size;
	hole[ZONE_NORMAL] = hole[0];
	hole[ZONE_DMA] = 0;
}
#endif /* CONFIG_ZONE_DMA */

static struct sys_timer board_timer = {
   .init = board_init_timer,
};

#if (( (IO_BASE_VA >>18) & 0xfffc) != 0x3c40)
#error IO_BASE_VA 
#endif

MACHINE_START(BRCM_NS, "Northstar Prototype")
   .phys_io = 					/* UART I/O mapping */
	IO_BASE_PA,
   .io_pg_offst = 				/* for early debug */
	(IO_BASE_VA >>18) & 0xfffc,
   .fixup = board_fixup,			/* Opt. early setup_arch() */
   .map_io = board_map_io,			/* Opt. from setup_arch() */
   .init_irq = board_init_irq,			/* main.c after setup_arch() */
   .timer  = &board_timer,			/* main.c after IRQs */
   .init_machine = board_init,			/* Late archinitcall */
   .boot_params = CONFIG_BOARD_PARAMS_PHYS,
MACHINE_END

#ifdef	CONFIG_MACH_BRCM_NS_QT
MACHINE_START(BRCM_NS_QT, "Northstar Emulation Model")
   .phys_io = 					/* UART I/O mapping */
	IO_BASE_PA,
   .io_pg_offst = 				/* for early debug */
	(IO_BASE_VA >>18) & 0xfffc,
   .fixup = board_fixup,			/* Opt. early setup_arch() */
   .map_io = board_map_io,			/* Opt. from setup_arch() */
   .init_irq = board_init_irq,			/* main.c after setup_arch() */
   .timer  = &board_timer,			/* main.c after IRQs */
   .init_machine = board_init,			/* Late archinitcall */
   .boot_params = CONFIG_BOARD_PARAMS_PHYS,
MACHINE_END
#endif

void arch_reset(char mode, const char *cmd)
{
#ifdef CONFIG_OUTER_CACHE_SYNC
	outer_cache.sync = NULL;
#endif
	hnd_cpu_reset(sih);
}

#ifdef CONFIG_MTD_PARTITIONS

static spinlock_t *mtd_lock = NULL;

spinlock_t *partitions_lock_init(void)
{
	if (!mtd_lock) {
		mtd_lock = (spinlock_t *)kzalloc(sizeof(spinlock_t), GFP_KERNEL);
		if (!mtd_lock)
			return NULL;

		spin_lock_init( mtd_lock );
	}
	return mtd_lock;
}
EXPORT_SYMBOL(partitions_lock_init);

static struct nand_hw_control *nand_hwcontrol = NULL;
struct nand_hw_control *nand_hwcontrol_lock_init(void)
{
	if (!nand_hwcontrol) {
		nand_hwcontrol = (struct nand_hw_control *)kzalloc(sizeof(struct nand_hw_control), GFP_KERNEL);
		if (!nand_hwcontrol)
			return NULL;

		spin_lock_init(&nand_hwcontrol->lock);
		init_waitqueue_head(&nand_hwcontrol->wq);
	}
	return nand_hwcontrol;
}
EXPORT_SYMBOL(nand_hwcontrol_lock_init);

/* Find out prom size */
static uint32 boot_partition_size(uint32 flash_phys) {
	uint32 bootsz, *bisz;

	/* Default is 256K boot partition */
	bootsz = 256 * 1024;

	/* Do we have a self-describing binary image? */
	bisz = (uint32 *)(flash_phys + BISZ_OFFSET);
	if (bisz[BISZ_MAGIC_IDX] == BISZ_MAGIC) {
		int isz = bisz[BISZ_DATAEND_IDX] - bisz[BISZ_TXTST_IDX];

		if (isz > (1024 * 1024))
			bootsz = 2048 * 1024;
		else if (isz > (512 * 1024))
			bootsz = 1024 * 1024;
		else if (isz > (256 * 1024))
			bootsz = 512 * 1024;
		else if (isz <= (128 * 1024))
			bootsz = 128 * 1024;
	}
	return bootsz;
}

#if defined(BCMCONFMTD)
#define MTD_PARTS 1
#else
#define MTD_PARTS 0
#endif
#if defined(PLC)
#define PLC_PARTS 1
#else
#define PLC_PARTS 0
#endif
#if defined(CONFIG_FAILSAFE_UPGRADE)
#define FAILSAFE_PARTS 2
#else
#define FAILSAFE_PARTS 0
#endif
#if defined(CONFIG_CRASHLOG)
#define CRASHLOG_PARTS 1
#else
#define CRASHLOG_PARTS 0
#endif
/* boot;nvram;kernel;rootfs;empty */
#define FLASH_PARTS_NUM	(5+MTD_PARTS+PLC_PARTS+FAILSAFE_PARTS+CRASHLOG_PARTS)

static struct mtd_partition bcm947xx_flash_parts[FLASH_PARTS_NUM] = {{0}};

static uint lookup_flash_rootfs_offset(struct mtd_info *mtd, int *trx_off, size_t size, 
                                       uint32 *trx_size)
{
	struct romfs_super_block *romfsb;
	struct cramfs_super *cramfsb;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	struct squashfs_super_block *squashfsb;
#else
	struct dummy_super_block *squashfsb;
#endif
	struct trx_header *trx;
	unsigned char buf[512];
	int off;
	size_t len;

	romfsb = (struct romfs_super_block *) buf;
	cramfsb = (struct cramfs_super *) buf;
	squashfsb = (void *) buf;
	trx = (struct trx_header *) buf;

	/* Look at every 64 KB boundary */
	for (off = *trx_off; off < size; off += (64 * 1024)) {
		memset(buf, 0xe5, sizeof(buf));

		/*
		 * Read block 0 to test for romfs and cramfs superblock
		 */
		if (mtd->read(mtd, off, sizeof(buf), &len, buf) ||
		    len != sizeof(buf))
			continue;

		/* Try looking at TRX header for rootfs offset */
		if (le32_to_cpu(trx->magic) == TRX_MAGIC) {
			*trx_off = off;
			*trx_size = le32_to_cpu(trx->len);
			if (trx->offsets[1] == 0)
				continue;
			/*
			 * Read to test for romfs and cramfs superblock
			 */
			off += le32_to_cpu(trx->offsets[1]);
			memset(buf, 0xe5, sizeof(buf));
			if (mtd->read(mtd, off, sizeof(buf), &len, buf) || len != sizeof(buf))
				continue;
		}

		/* romfs is at block zero too */
		if (romfsb->word0 == ROMSB_WORD0 &&
		    romfsb->word1 == ROMSB_WORD1) {
			printk(KERN_NOTICE
			       "%s: romfs filesystem found at block %d\n",
			       mtd->name, off / mtd->erasesize);
			break;
		}

		/* so is cramfs */
		if (cramfsb->magic == CRAMFS_MAGIC) {
			printk(KERN_NOTICE
			       "%s: cramfs filesystem found at block %d\n",
			       mtd->name, off / mtd->erasesize);
			break;
		}

		if (squashfsb->s_magic == SQUASHFS_MAGIC) {
			printk(KERN_NOTICE
			       "%s: squash filesystem found at block %d\n",
			       mtd->name, off / mtd->erasesize);
			break;
		}
	}

	return off;
}

struct mtd_partition *
init_mtd_partitions(hndsflash_t *sfl_info, struct mtd_info *mtd, size_t size)
{
	int bootdev;
	int knldev;
	int nparts = 0;
	uint32 offset = 0;
	uint32 maxsize = 0;
	uint rfs_off = 0;
	uint vmlz_off, knl_size;
	uint32 top = 0;
	uint32 bootsz = 0;
	uint32 trx_size;
#ifdef CONFIG_CRASHLOG
	char create_crash_partition = 0;
#endif
#ifdef CONFIG_FAILSAFE_UPGRADE
	char *img_boot = nvram_get(BOOTPARTITION);
	char *imag_1st_offset = nvram_get(IMAGE_FIRST_OFFSET);
	char *imag_2nd_offset = nvram_get(IMAGE_SECOND_OFFSET);
	unsigned int image_first_offset = 0;
	unsigned int image_second_offset = 0;
	char dual_image_on = 0;

	/* The image_1st_size and image_2nd_size are necessary if the Flash does not have any
	 * image
	 */
	dual_image_on = (img_boot != NULL && imag_1st_offset != NULL && imag_2nd_offset != NULL);

	if (dual_image_on) {
		image_first_offset = simple_strtol(imag_1st_offset, NULL, 10);
		image_second_offset = simple_strtol(imag_2nd_offset, NULL, 10);
		printk("The first offset=%x, 2nd offset=%x\n", image_first_offset,
			image_second_offset);

	}
#endif	/* CONFIG_FAILSAFE_UPGRADE */

    	/*  BOOT and NVRAM size NETGEAR*/
	/* R6900, R7000 and R6700v1 */
	if (nvram_match("boardnum", "32") &&
		 nvram_match("boardtype", "0x0665") &&
		 nvram_match("boardrev", "0x1301")) {
	        maxsize = 0x200000; /* 2 MB */
	        size = maxsize;
	}
	/* R6400, R6400v2, R6700v3 and XR300 */
	else if (nvram_match("boardnum", "32") &&
		 nvram_match("boardtype", "0x0646") &&
		 nvram_match("boardrev", "0x1601")) {
	        maxsize = 0x200000; /* 2 MB */
	        size = maxsize;
	}
#ifdef CONFIG_SMP
	/* AC1450, R6300V2 / R6250 */
	else if (nvram_match("boardnum", "679") &&
	     nvram_match("boardtype", "0x0646") &&
	     nvram_match("boardrev", "0x1110")) {
	        maxsize = 0x200000; /* 2 MB */
	        size = maxsize;
	}
#else /* single core */
	/* R6200v2 */
	else if (nvram_match("boardnum", "679") &&
	         nvram_match("boardtype", "0x0646") &&
	         nvram_match("boardrev", "0x1110")) {
			maxsize = 0x200000; /* 2 MB */
			size = maxsize;
	}
#endif /* CONFIG_SMP */
	/* Buffalo WZR-1750DHP */
	else if (nvram_match("boardnum", "00") &&
	     nvram_match("boardtype","0xF646") &&
	     nvram_match("boardrev", "0x1100")) {
		size = 0x100000;	/* flash0 ST Compatible Serial flash size 1024KB */
		bootsz = 0x40000;	/* flash0.boot ST Compatible Serial flash offset 00000000 size 256KB */
					/* flash0.nvram ST Compatible Serial flash offset 000F0000 size 64KB */
	}
	
	bootdev = soc_boot_dev((void *)sih);
	knldev = soc_knl_dev((void *)sih);

	if (bootdev == SOC_BOOTDEV_NANDFLASH) {
		/* Do not init MTD partitions on NOR flash when NAND boot */
		return NULL;	
	}

	if (knldev != SOC_KNLDEV_NANDFLASH) {
		vmlz_off = 0;
		rfs_off = lookup_flash_rootfs_offset(mtd, &vmlz_off, size, &trx_size);

		/* Size pmon */
		bcm947xx_flash_parts[nparts].name = "boot";
		bcm947xx_flash_parts[nparts].size = vmlz_off;
		bcm947xx_flash_parts[nparts].offset = top;
		bcm947xx_flash_parts[nparts].mask_flags = MTD_WRITEABLE; /* forces on read only */
		nparts++;

		/* Setup kernel MTD partition */
		bcm947xx_flash_parts[nparts].name = "linux";
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (trx_size > (image_second_offset-image_first_offset)) {
			printk("sflash size is too small to afford two images.\n");
			dual_image_on = 0;
			image_first_offset = 0;
			image_second_offset = 0;
		}
		if (dual_image_on) {
			bcm947xx_flash_parts[nparts].size = image_second_offset-image_first_offset;
		} else {
			bcm947xx_flash_parts[nparts].size = mtd->size - vmlz_off;

			/* Reserve for NVRAM */
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(nvram_space, mtd->erasesize);
#ifdef PLC
			/* Reserve for PLC */
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x1000, mtd->erasesize);
#endif
			/* Netgear EX7000 - Reserve space for board_data */
			if (nvram_match("boardnum", "679") &&
			    nvram_match("boardtype", "0x0646") &&
			    nvram_match("boardrev", "0x1100")) {
				bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x10000, mtd->erasesize); /* 64K */
			}
#ifdef BCMCONFMTD
			bcm947xx_flash_parts[nparts].size -= (mtd->erasesize *4);
#endif
		}
#else

		bcm947xx_flash_parts[nparts].size = mtd->size - vmlz_off;

#ifdef PLC
		/* Reserve for PLC */
		bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x1000, mtd->erasesize);
#endif
		/* Reserve for NVRAM */
		bcm947xx_flash_parts[nparts].size -= ROUNDUP(nvram_space, mtd->erasesize);
		
		/* Netgear EX7000 - Reserve space for board_data */
		if (nvram_match("boardnum", "679") &&
		    nvram_match("boardtype", "0x0646") &&
		    nvram_match("boardrev", "0x1100")) {
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x10000, mtd->erasesize); /* 64K */
		}

#ifdef BCMCONFMTD
		bcm947xx_flash_parts[nparts].size -= (mtd->erasesize *4);
#endif
#endif	/* CONFIG_FAILSAFE_UPGRADE */

		/* R1D - Reserve for board_data */
		if (nvram_match("boardnum", "32") && nvram_match("boardtype", "0x0665") &&
		    nvram_match("boardrev", "0x1301") && nvram_match("model", "R1D")) {
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x10000, mtd->erasesize);
		}

#ifdef CONFIG_CRASHLOG
		if ((bcm947xx_flash_parts[nparts].size - trx_size) >=
		    ROUNDUP(0x4000, mtd->erasesize)) {
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x4000, mtd->erasesize);
			create_crash_partition = 1;
		} else {
			create_crash_partition = 0;
		}
#endif

		bcm947xx_flash_parts[nparts].offset = vmlz_off;
		knl_size = bcm947xx_flash_parts[nparts].size;
		offset = bcm947xx_flash_parts[nparts].offset + knl_size;
		nparts++; 
		
		/* Setup rootfs MTD partition */
		bcm947xx_flash_parts[nparts].name = "rootfs";
		bcm947xx_flash_parts[nparts].size = knl_size - (rfs_off - vmlz_off);
		bcm947xx_flash_parts[nparts].offset = rfs_off;
		bcm947xx_flash_parts[nparts].mask_flags = MTD_WRITEABLE; /* forces on read only */
		offset = bcm947xx_flash_parts[nparts].offset + bcm947xx_flash_parts[nparts].size;
		nparts++;

#if defined(CONFIG_CRASHLOG)
		if (create_crash_partition) {
			/* Setup crash MTD partition */
			bcm947xx_flash_parts[nparts].name = "crash";
			bcm947xx_flash_parts[nparts].size = ROUNDUP(0x4000, mtd->erasesize);
			bcm947xx_flash_parts[nparts].offset = offset;
			bcm947xx_flash_parts[nparts].mask_flags = 0;
			nparts++;
		}
#endif
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (dual_image_on) {
			offset = image_second_offset;
			rfs_off = lookup_flash_rootfs_offset(mtd, &offset, size, &trx_size);
			/* When the second image doesn't exist,
			 * set the rootfs use the same offset with the kernel
			 */
			if (rfs_off == size)
				rfs_off = offset;

			vmlz_off = offset;
			/* Setup kernel2 MTD partition */
			bcm947xx_flash_parts[nparts].name = "linux2";
			bcm947xx_flash_parts[nparts].size = mtd->size - image_second_offset;
			/* Reserve for NVRAM */
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(nvram_space, mtd->erasesize);

#ifdef BCMCONFMTD
			bcm947xx_flash_parts[nparts].size -= (mtd->erasesize *4);
#endif
#ifdef PLC
			/* Reserve for PLC */
			bcm947xx_flash_parts[nparts].size -= ROUNDUP(0x1000, mtd->erasesize);
#endif
			bcm947xx_flash_parts[nparts].offset = image_second_offset;
			knl_size = bcm947xx_flash_parts[nparts].size;
			offset = bcm947xx_flash_parts[nparts].offset + knl_size;
			nparts++;

			/* Setup rootfs MTD partition */
			bcm947xx_flash_parts[nparts].name = "rootfs2";
			bcm947xx_flash_parts[nparts].size =
				knl_size - (rfs_off - image_second_offset);
			bcm947xx_flash_parts[nparts].offset = rfs_off;
			/* forces on read only */
			bcm947xx_flash_parts[nparts].mask_flags = MTD_WRITEABLE;
			nparts++;
		}
#endif	/* CONFIG_FAILSAFE_UPGRADE */

	} else {
		if (!bootsz)
			bootsz = boot_partition_size(sfl_info->base);

		printk("Boot partition size = %d(0x%x)\n", bootsz, bootsz);

		/* Size pmon */
		if (maxsize)
			bootsz = maxsize;
		bcm947xx_flash_parts[nparts].name = "boot";
		bcm947xx_flash_parts[nparts].size = bootsz;
		bcm947xx_flash_parts[nparts].offset = top;
		//bcm947xx_flash_parts[nparts].mask_flags = MTD_WRITEABLE; /* forces on read only */
		offset = bcm947xx_flash_parts[nparts].size;
		nparts++;
	}

#ifdef BCMCONFMTD
	/* Setup CONF MTD partition */
	bcm947xx_flash_parts[nparts].name = "confmtd";
	bcm947xx_flash_parts[nparts].size = mtd->erasesize * 4;
	bcm947xx_flash_parts[nparts].offset = offset;
	offset = bcm947xx_flash_parts[nparts].offset + bcm947xx_flash_parts[nparts].size;
	nparts++;
#endif	/* BCMCONFMTD */

#ifdef PLC
	/* Setup plc MTD partition */
	bcm947xx_flash_parts[nparts].name = "plc";
	bcm947xx_flash_parts[nparts].size = ROUNDUP(0x1000, mtd->erasesize);
	bcm947xx_flash_parts[nparts].offset =
		size - (ROUNDUP(nvram_space, mtd->erasesize) + ROUNDUP(0x1000, mtd->erasesize));
	nparts++;
#endif

	/* R1D - Setup board_data partition */
	if (nvram_match("boardnum", "32") && nvram_match("boardtype", "0x0665")
	        && nvram_match("boardrev", "0x1301") && nvram_match("model", "R1D")) {
		bcm947xx_flash_parts[nparts].name = "board_data";
		bcm947xx_flash_parts[nparts].size = ROUNDUP(0x10000, mtd->erasesize);
		bcm947xx_flash_parts[nparts].offset = 0xFE0000;
		nparts++;
	}

	/* Netgear EX7000 - Setup board_data partition */
	if (nvram_match("boardnum", "679") &&
	    nvram_match("boardtype", "0x0646") &&
	    nvram_match("boardrev", "0x1100")) {
		bcm947xx_flash_parts[nparts].name = "board_data";
		bcm947xx_flash_parts[nparts].size = ROUNDUP(0x10000, mtd->erasesize); /* 64K */
		bcm947xx_flash_parts[nparts].offset = size - ROUNDUP(nvram_space, mtd->erasesize) - bcm947xx_flash_parts[nparts].size;
		nparts++;
		/*
		 * bcmsflash: squash filesystem found at block 35
		 * Creating 13 MTD partitions on "bcmsflash":
		 * 0x000000000000-0x000000040000 : "boot"
		 * 0x000000040000-0x000000f60000 : "linux"
		 * 0x000000239380-0x000000f60000 : "rootfs"
		 * 0x000000f60000-0x000000f70000 : "ML1"
		 * 0x000000f70000-0x000000f80000 : "ML2"
		 * 0x000000f80000-0x000000f90000 : "ML3"
		 * 0x000000f90000-0x000000fa0000 : "ML4"
		 * 0x000000fa0000-0x000000fb0000 : "ML5"
		 * 0x000000fb0000-0x000000fc0000 : "ML6"
		 * 0x000000fc0000-0x000000fd0000 : "ML7"
		 * 0x000000fd0000-0x000000fe0000 : "POT"
		 * 0x000000fe0000-0x000000ff0000 : "board_data"
		 * 0x000000ff0000-0x000001000000 : "nvram"
		 * Note: For FT - keep board_data partition -> used for WL parameters! ML1 to ML7 and POT not needed
		 */
	}

	/* Setup nvram MTD partition */
	bcm947xx_flash_parts[nparts].name = "nvram";
	bcm947xx_flash_parts[nparts].size = ROUNDUP(nvram_space, mtd->erasesize);

	/* R1D */
	if (nvram_match("boardnum", "32") && nvram_match("boardtype", "0x0665")
	        && nvram_match("boardrev", "0x1301") && nvram_match("model", "R1D")) {
			bcm947xx_flash_parts[nparts].offset = 0xFF0000;
	} else {
		if (maxsize)
			bcm947xx_flash_parts[nparts].offset = (size - 0x10000) - bcm947xx_flash_parts[nparts].size;
		else
			bcm947xx_flash_parts[nparts].offset = size - bcm947xx_flash_parts[nparts].size;
	}

	nparts++;

	return bcm947xx_flash_parts;
}

EXPORT_SYMBOL(init_mtd_partitions);

#endif /* CONFIG_MTD_PARTITIONS */


#ifdef	CONFIG_MTD_NFLASH

#define NFLASH_PARTS_NUM	7
static struct mtd_partition bcm947xx_nflash_parts[NFLASH_PARTS_NUM] = {{0}};

static uint
lookup_nflash_rootfs_offset(hndnand_t *nfl, struct mtd_info *mtd, int offset, size_t size)
{
	struct romfs_super_block *romfsb;
	struct cramfs_super *cramfsb;
	struct dummy_super_block *squashfsb;
	struct trx_header *trx;
	unsigned char *buf;
	uint blocksize, pagesize, mask, blk_offset, off, shift = 0;
	int ret;

	pagesize = nfl->pagesize;
	buf = (unsigned char *)kmalloc(pagesize, GFP_KERNEL);
	if (!buf) {
		printk("lookup_nflash_rootfs_offset: kmalloc fail\n");
		return 0;
	}

	romfsb = (struct romfs_super_block *) buf;
	cramfsb = (struct cramfs_super *) buf;
	squashfsb = (void *) buf;
	trx = (struct trx_header *) buf;

	/* Look at every block boundary till 16MB; higher space is reserved for application data. */
	blocksize = mtd->erasesize;
	printk("lookup_nflash_rootfs_offset: offset = 0x%x\n", offset);
	for (off = offset; off < offset + size; off += blocksize) {
		mask = blocksize - 1;
		blk_offset = off & ~mask;
		if (hndnand_checkbadb(nfl, blk_offset) != 0)
			continue;
		memset(buf, 0xe5, pagesize);
		if ((ret = hndnand_read(nfl, off, pagesize, buf)) != pagesize) {
			printk(KERN_NOTICE
			       "%s: nflash_read return %d\n", mtd->name, ret);
			continue;
		}

		/* Try looking at TRX header for rootfs offset */
		if (le32_to_cpu(trx->magic) == TRX_MAGIC) {
			mask = pagesize - 1;
			off = offset + (le32_to_cpu(trx->offsets[1]) & ~mask) - blocksize;
			shift = (le32_to_cpu(trx->offsets[1]) & mask);
			romfsb = (struct romfs_super_block *)((unsigned char *)romfsb + shift);
			cramfsb = (struct cramfs_super *)((unsigned char *)cramfsb + shift);
			squashfsb = (struct squashfs_super_block *)
				((unsigned char *)squashfsb + shift);
			continue;
		}

		/* romfs is at block zero too */
		if (romfsb->word0 == ROMSB_WORD0 &&
		    romfsb->word1 == ROMSB_WORD1) {
			printk(KERN_NOTICE
			       "%s: romfs filesystem found at block %d\n",
			       mtd->name, off / blocksize);
			break;
		}

		/* so is cramfs */
		if (cramfsb->magic == CRAMFS_MAGIC) {
			printk(KERN_NOTICE
			       "%s: cramfs filesystem found at block %d\n",
			       mtd->name, off / blocksize);
			break;
		}

		if (squashfsb->s_magic == SQUASHFS_MAGIC) {
			printk(KERN_NOTICE
			       "%s: squash filesystem with lzma found at block %d\n",
			       mtd->name, off / blocksize);
			break;
		}

	}

	if (buf)
		kfree(buf);

	return shift + off;
}

struct mtd_partition *
init_nflash_mtd_partitions(hndnand_t *nfl, struct mtd_info *mtd, size_t size)
{
	int bootdev;
	int knldev;
	int nparts = 0;
	uint32 offset = 0;
	uint shift = 0;
	uint32 top = 0;
	uint32 bootsz;
	uint32 nvsz = 0;
	uint32 bootossz = nfl_boot_os_size(nfl);
	uint boardnum = bcm_strtoul(nvram_safe_get("boardnum"), NULL, 0);

	/* EA6700 */
	if (((boardnum == 1) || (nvram_get("boardnum") == NULL)) &&
	    nvram_match("boardtype", "0xF646") &&
	    nvram_match("boardrev", "0x1100")) {
		bootossz = 0x4000000;
		nvsz = 0x100000;
	}
	/* EA6900 */
	else if (((boardnum == 1) || (nvram_get("boardnum") == NULL)) &&
		 nvram_match("boardtype","0xD646") &&
		 nvram_match("boardrev","0x1100")) {
		bootossz = 0x4000000;	
		nvsz = 0x100000;
	}
	/* Linksys EA6350v2 */
	/* 0x000000080000-0x000000180000 : "nvram" */
	else if (nvram_match("t_fix1", "EA6350v2") || /* FT backup --> fast detection OR if cfe changes/deletes nv variables! */
		 (nvram_match("boardnum","20150309") &&
		  nvram_match("boardtype", "0xE646") &&
		  nvram_match("boardrev", "0x1200"))) {
		nvsz = 0x100000; /* nflash0.nvram        Toshiba NAND flash offset 80000 size 1024KB */
	}

#ifdef CONFIG_FAILSAFE_UPGRADE
	char *img_boot = nvram_get(BOOTPARTITION);
	char *imag_1st_offset = nvram_get(IMAGE_FIRST_OFFSET);
	char *imag_2nd_offset = nvram_get(IMAGE_SECOND_OFFSET);
	unsigned int image_first_offset = 0;
	unsigned int image_second_offset = 0;
	char dual_image_on = 0;

	/* The image_1st_size and image_2nd_size are necessary if the Flash does not have any
	 * image
	 */
	dual_image_on = (img_boot != NULL && imag_1st_offset != NULL && imag_2nd_offset != NULL);

	if (dual_image_on) {
		image_first_offset = simple_strtol(imag_1st_offset, NULL, 10);
		image_second_offset = simple_strtol(imag_2nd_offset, NULL, 10);
		printk("The first offset=%x, 2nd offset=%x\n", image_first_offset,
			image_second_offset);

	}
#endif	/* CONFIG_FAILSAFE_UPGRADE */

	bootdev = soc_boot_dev((void *)sih);
	knldev = soc_knl_dev((void *)sih);

	if (bootdev == SOC_BOOTDEV_NANDFLASH) {
		bootsz = boot_partition_size(nfl->base);
		if (bootsz > mtd->erasesize) {
			/* Prepare double space in case of bad blocks */
			bootsz = (bootsz << 1);
		} else {
			/* CFE occupies at least one block */
			bootsz = mtd->erasesize;
		}
		printk("Boot partition size = %d(0x%x)\n", bootsz, bootsz);

		/* Size pmon */
		bcm947xx_nflash_parts[nparts].name = "boot";
		bcm947xx_nflash_parts[nparts].size = bootsz;
		bcm947xx_nflash_parts[nparts].offset = top;
		//bcm947xx_nflash_parts[nparts].mask_flags = MTD_WRITEABLE; /* forces on read only */
		offset = bcm947xx_nflash_parts[nparts].size;
		nparts++;

		/* Setup NVRAM MTD partition */
		bcm947xx_nflash_parts[nparts].name = "nvram";
		if (nvsz)
			bcm947xx_nflash_parts[nparts].size = nvsz;
		else
			bcm947xx_nflash_parts[nparts].size = nfl_boot_size(nfl) - offset;
		bcm947xx_nflash_parts[nparts].offset = offset;

		offset = nfl_boot_size(nfl);
		nparts++;
	}

	if (knldev == SOC_KNLDEV_NANDFLASH) {
		/* Setup kernel MTD partition */
		bcm947xx_nflash_parts[nparts].name = "linux";
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (dual_image_on) {
			bcm947xx_nflash_parts[nparts].size =
				image_second_offset - image_first_offset;
		} else
#endif
		{
			bcm947xx_nflash_parts[nparts].size = nparts ? 
				(bootossz - nfl_boot_size(nfl)) :
				nfl_boot_os_size(nfl);
		}
		
		/* Change linux size from default 0x2000000 for NETGEAR models*/
		/* R6900, R7000 and R6700v1 */
		/* Stock R7000 is 0x2200000 */
		if (nvram_match("boardnum", "32") &&
			 nvram_match("boardtype", "0x0665") &&
			 nvram_match("boardrev", "0x1301")) {
			bcm947xx_nflash_parts[nparts].size += 0x200000;
		}
		/* R6400, R6400v2, R6700v3 and XR300 */
		/* Stock R6400 is 0x6d00000 */
		else if (nvram_match("boardnum", "32") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1601")) {
			bcm947xx_nflash_parts[nparts].size += 0x1200000;
		}
#ifdef CONFIG_SMP
		/* AC1450, R6300V2 / R6250 */
		/* Stock R6250 is 0x2180000 */
		else if (nvram_match("boardnum","679") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1110")) {
			offset += 0x180000; /* Leave NETGEAR partitions alone */
			bcm947xx_nflash_parts[nparts].size += 0x0000;
		}
#else /* single core */
		/* R6200v2 */
		/* Stock R6200v2 is 0x2200000 */
		else if (nvram_match("boardnum","679") &&
		         nvram_match("boardtype", "0x0646") &&
		         nvram_match("boardrev", "0x1110")) {
				bcm947xx_nflash_parts[nparts].size += 0x200000;
		}
#endif /* CONFIG_SMP */
		/* Linksys EA6350v2 */
		/* 0x000000200000-0x000001f00000 : "linux" */
		else if (nvram_match("t_fix1", "EA6350v2") || /* FT backup --> fast detection OR if cfe changes/deletes nv variables! */
			 (nvram_match("boardnum","20150309") &&
			  nvram_match("boardtype", "0xE646") &&
			  nvram_match("boardrev", "0x1200"))) {
			bcm947xx_nflash_parts[nparts].size -= 0x100000;
		}
		
		bcm947xx_nflash_parts[nparts].offset = offset;

		shift = lookup_nflash_rootfs_offset(nfl, mtd, offset,
			bcm947xx_nflash_parts[nparts].size);

#ifdef CONFIG_FAILSAFE_UPGRADE
		if (dual_image_on)
			offset = image_second_offset;
		else
#endif
		offset = bootossz;
		nparts++;

		/* Setup rootfs MTD partition */
		bcm947xx_nflash_parts[nparts].name = "rootfs";
#ifdef CONFIG_FAILSAFE_UPGRADE
		if (dual_image_on)
			bcm947xx_nflash_parts[nparts].size = image_second_offset - shift;
		else
#endif
		bcm947xx_nflash_parts[nparts].size = bootossz - shift;
		bcm947xx_nflash_parts[nparts].offset = shift;
		bcm947xx_nflash_parts[nparts].mask_flags = MTD_WRITEABLE;
		offset = nfl_boot_os_size(nfl);

		/* Adjust rootfs size from default 0x2000000 for NETGEAR models*/
		/* R6900, R7000 and R6700v1 */
		if (nvram_match("boardnum", "32") &&
		    nvram_match("boardtype", "0x0665") &&
		    nvram_match("boardrev", "0x1301")) {
			bcm947xx_nflash_parts[nparts].size += 0x200000;
		}
		/* R6400, R6400v2, R6700v3 and XR300 */
		/* Stock R6400 is 0x6d00000 */
		else if (nvram_match("boardnum", "32") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1601")) {
			bcm947xx_nflash_parts[nparts].size += 0x1200000;
		}
#ifdef CONFIG_SMP
		/* AC1450, R6300V2 and R6250 */
        	/* Stock R6250 is 0x2180000 */
		else if (nvram_match("boardnum","679") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1110")) {
			bcm947xx_nflash_parts[nparts].size += 0x180000;
		}
#else /* single core */
		/* Stock R6200v2 is 0x2200000 */
		else if (nvram_match("boardnum","679") &&
		         nvram_match("boardtype", "0x0646") &&
		         nvram_match("boardrev", "0x1110")) {
				bcm947xx_nflash_parts[nparts].size += 0x200000;
		}
#endif /* CONFIG_SMP */
		/* Linksys EA6350v2 */
		/* 0x0000003e6098-0x000001f00000 : "rootfs" */
		else if (nvram_match("t_fix1", "EA6350v2") || /* FT backup --> fast detection OR if cfe changes/deletes nv variables! */
			 (nvram_match("boardnum","20150309") &&
			  nvram_match("boardtype", "0xE646") &&
			  nvram_match("boardrev", "0x1200"))) {
			bcm947xx_nflash_parts[nparts].size -= 0x100000;
		}
    
    /* Adjustments for JFFS are here: /linux-2.6.36/drivers/mtd/bcm947xx/nand/brcmnand.c */

		nparts++;

#ifdef CONFIG_DUAL_TRX /* ASUS Setup 2nd kernel MTD partition */
                bcm947xx_nflash_parts[nparts].name = "linux2";
                bcm947xx_nflash_parts[nparts].size = NFL_BOOT_OS_SIZE;
                bcm947xx_nflash_parts[nparts].offset = NFL_BOOT_OS_SIZE;
                nparts++;
                /* Setup rootfs MTD partition */
                bcm947xx_nflash_parts[nparts].name = "rootfs2";
                bcm947xx_nflash_parts[nparts].size = NFL_BOOT_OS_SIZE - shift;
                bcm947xx_nflash_parts[nparts].offset = NFL_BOOT_OS_SIZE + shift;
                bcm947xx_nflash_parts[nparts].mask_flags = MTD_WRITEABLE;
                nparts++;
#endif /* End of ASUS 2nd FW partition*/

		/* Set NETGEAR board_data partition */
    		/* R6900, R7000 and R6700v1 */
		if (nvram_match("boardnum", "32") &&
		    nvram_match("boardtype", "0x0665") &&
		    nvram_match("boardrev", "0x1301")) {
			bcm947xx_nflash_parts[nparts].name = "board_data";
			bcm947xx_nflash_parts[nparts].size = 0x40000;
			bcm947xx_nflash_parts[nparts].offset = 0x2200000;
			nparts++;
		}
		/* R6400, R6400v2, R6700v3 and XR300 */
		else if (nvram_match("boardnum", "32") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1601")) {
			bcm947xx_nflash_parts[nparts].name = "board_data";
			bcm947xx_nflash_parts[nparts].size = 0x80000;
			bcm947xx_nflash_parts[nparts].offset = 0x7200000;
			nparts++;
		}
#ifdef CONFIG_SMP
		/* AC1450, R6300V2 and R6250 */
		else if (nvram_match("boardnum","679") &&
			 nvram_match("boardtype", "0x0646") &&
			 nvram_match("boardrev", "0x1110")) {
			bcm947xx_nflash_parts[nparts].name = "board_data";
			bcm947xx_nflash_parts[nparts].size = 0x20000;
			bcm947xx_nflash_parts[nparts].offset = 0x200000;
			nparts++;
		}
#else /* single core */
		/* R6200v2 */
		else if (nvram_match("boardnum","679") &&
		         nvram_match("boardtype", "0x0646") &&
		         nvram_match("boardrev", "0x1110")) {
				bcm947xx_nflash_parts[nparts].name = "board_data";
				bcm947xx_nflash_parts[nparts].size = 0x40000;
				bcm947xx_nflash_parts[nparts].offset = 0x2200000;
				nparts++;
		}
#endif /* CONFIG_SMP */

#ifdef CONFIG_FAILSAFE_UPGRADE
		/* Setup 2nd kernel MTD partition */
		if (dual_image_on) {
			bcm947xx_nflash_parts[nparts].name = "linux2";
			bcm947xx_nflash_parts[nparts].size = bootossz - image_second_offset;
			bcm947xx_nflash_parts[nparts].offset = image_second_offset;
			shift = lookup_nflash_rootfs_offset(nfl, mtd, image_second_offset,
			                                    bcm947xx_nflash_parts[nparts].size);
			nparts++;
			/* Setup rootfs MTD partition */
			bcm947xx_nflash_parts[nparts].name = "rootfs2";
			bcm947xx_nflash_parts[nparts].size = bootossz - shift;
			bcm947xx_nflash_parts[nparts].offset = shift;
			bcm947xx_nflash_parts[nparts].mask_flags = MTD_WRITEABLE;
			nparts++;
		}
#endif	/* CONFIG_FAILSAFE_UPGRADE */

	}

#ifdef PLAT_NAND_JFFS2
	/* Setup the remainder of NAND Flash as FFS partition */
	if( size > offset ) {
		bcm947xx_nflash_parts[nparts].name = "ffs";
		bcm947xx_nflash_parts[nparts].size = size - offset ;
		bcm947xx_nflash_parts[nparts].offset = offset;
		bcm947xx_nflash_parts[nparts].mask_flags = 0;
		bcm947xx_nflash_parts[nparts].ecclayout = mtd->ecclayout;
		nparts++;
	}
#endif

	return bcm947xx_nflash_parts;
}

/* LR: Calling this function directly violates Linux API conventions */
EXPORT_SYMBOL(init_nflash_mtd_partitions);
#endif /* CONFIG_MTD_NFLASH */

#ifdef CONFIG_CRASHLOG
extern char *get_logbuf(void);
extern char *get_logsize(void);

void nvram_store_crash(void)
{
	struct mtd_info *mtd = NULL;
	int i;
	char *buffer;
	unsigned char buf[16];
	int buf_len;
	int len;

	printk("Trying to store crash\n");

	mtd = get_mtd_device_nm("crash");

	if (!IS_ERR(mtd)) {

		buf_len = get_logsize();
		buffer = get_logbuf();
		if (buf_len > mtd->size)
			buf_len = mtd->size;

		memset(buf,0,sizeof(buf));
		mtd->read(mtd, 0, sizeof(buf), &len, buf);
		for (len=0;len<sizeof(buf);len++)
			if (buf[len]!=0xff) {
				printk("Could not save crash, partition not clean\n");
				break;
			}
		if (len == sizeof(buf)) {
			mtd->write(mtd, 0, buf_len, &len, buffer);
			if (buf_len == len)
				printk("Crash Saved\n");
		}
	} else {
		printk("Could not find NVRAM partition\n");
	}
}
#endif /* CONFIG_CRASHLOG */
