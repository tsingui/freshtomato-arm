#if !defined _FEATURES_H && !defined __need_uClibc_config_h
# error Never include <bits/uClibc_config.h> directly; use <features.h> instead
#endif

#define __UCLIBC_MAJOR__ 0
#define __UCLIBC_MINOR__ 9
#define __UCLIBC_SUBLEVEL__ 33
/* Automatically generated make config: don't edit */
/* Version: 0.9.33.2 */
/* Thu Mar  7 21:29:04 2024 */
#undef __TARGET_alpha__
#define __TARGET_arm__ 1
#undef __TARGET_avr32__
#undef __TARGET_bfin__
#undef __TARGET_c6x__
#undef __TARGET_cris__
#undef __TARGET_e1__
#undef __TARGET_frv__
#undef __TARGET_h8300__
#undef __TARGET_hppa__
#undef __TARGET_i386__
#undef __TARGET_i960__
#undef __TARGET_ia64__
#undef __TARGET_m68k__
#undef __TARGET_microblaze__
#undef __TARGET_mips__
#undef __TARGET_nios__
#undef __TARGET_nios2__
#undef __TARGET_powerpc__
#undef __TARGET_sh__
#undef __TARGET_sh64__
#undef __TARGET_sparc__
#undef __TARGET_v850__
#undef __TARGET_vax__
#undef __TARGET_x86_64__
#undef __TARGET_xtensa__

/* Target Architecture Features and Options */
#define __TARGET_ARCH__ "arm"
#define __FORCE_OPTIONS_FOR_ARCH__ 1
#define __CONFIG_ARM_EABI__ 1
#undef __COMPILE_IN_THUMB_MODE__
#define __USE_BX__ 1
#define __TARGET_SUBARCH__ ""

/* Using ELF file format */
#define __ARCH_ANY_ENDIAN__ 1
#define __ARCH_LITTLE_ENDIAN__ 1
#undef __ARCH_WANTS_BIG_ENDIAN__
#define __ARCH_WANTS_LITTLE_ENDIAN__ 1
#define __ARCH_HAS_MMU__ 1
#define __ARCH_USE_MMU__ 1
#define __UCLIBC_HAS_FLOATS__ 1
#undef __UCLIBC_HAS_FPU__
#define __UCLIBC_HAS_SOFT_FLOAT__ 1
#define __DO_C99_MATH__ 1
#undef __DO_XSI_MATH__
#undef __UCLIBC_HAS_FENV__
#define __KERNEL_HEADERS__ "/home/pedro/buildroot-2012.02-ARM/output/toolchain/linux/include"
#define __HAVE_DOT_CONFIG__ 1

/* General Library Settings */
#define __DOPIC__ 1
#define __HAVE_SHARED__ 1
#undef __FORCE_SHAREABLE_TEXT_SEGMENTS__
#define __LDSO_LDD_SUPPORT__ 1
#undef __LDSO_CACHE_SUPPORT__
#define __LDSO_PRELOAD_ENV_SUPPORT__ 1
#undef __LDSO_PRELOAD_FILE_SUPPORT__
#undef __LDSO_STANDALONE_SUPPORT__
#undef __LDSO_PRELINK_SUPPORT__
#undef __UCLIBC_STATIC_LDCONFIG__
#define __LDSO_RUNPATH__ 1
#define __LDSO_SEARCH_INTERP_PATH__ 1
#define __LDSO_LD_LIBRARY_PATH__ 1
#undef __LDSO_NO_CLEANUP__
#define __UCLIBC_CTOR_DTOR__ 1
#undef __LDSO_GNU_HASH_SUPPORT__
#undef __HAS_NO_THREADS__
#undef __LINUXTHREADS_OLD__
#undef __LINUXTHREADS_NEW__
#define __UCLIBC_HAS_THREADS_NATIVE__ 1
#define __UCLIBC_HAS_THREADS__ 1
#define __UCLIBC_HAS_TLS__ 1
#define __PTHREADS_DEBUG_SUPPORT__ 1
#define __UCLIBC_HAS_SYSLOG__ 1
#define __UCLIBC_HAS_LFS__ 1
#undef __MALLOC__
#undef __MALLOC_SIMPLE__
#define __MALLOC_STANDARD__ 1
#define __MALLOC_GLIBC_COMPAT__ 1
#define __UCLIBC_DYNAMIC_ATEXIT__ 1
#undef __COMPAT_ATEXIT__
#define __UCLIBC_SUSV3_LEGACY__ 1
#undef __UCLIBC_SUSV3_LEGACY_MACROS__
#define __UCLIBC_SUSV4_LEGACY__ 1
#undef __UCLIBC_STRICT_HEADERS__
#undef __UCLIBC_HAS_STUBS__
#define __UCLIBC_HAS_SHADOW__ 1
#define __UCLIBC_HAS_PROGRAM_INVOCATION_NAME__ 1
#define __UCLIBC_HAS___PROGNAME__ 1
#define __UCLIBC_HAS_PTY__ 1
#define __ASSUME_DEVPTS__ 1
#define __UNIX98PTY_ONLY__ 1
#define __UCLIBC_HAS_GETPT__ 1
#define __UCLIBC_HAS_LIBUTIL__ 1
#define __UCLIBC_HAS_TM_EXTENSIONS__ 1
#define __UCLIBC_HAS_TZ_CACHING__ 1
#define __UCLIBC_HAS_TZ_FILE__ 1
#define __UCLIBC_HAS_TZ_FILE_READ_MANY__ 1
#define __UCLIBC_TZ_FILE_PATH__ "/etc/TZ"
#define __UCLIBC_FALLBACK_TO_ETC_LOCALTIME__ 1

/* Advanced Library Settings */
#define __UCLIBC_PWD_BUFFER_SIZE__ 256
#define __UCLIBC_GRP_BUFFER_SIZE__ 256

/* Support various families of functions */
#define __UCLIBC_LINUX_MODULE_26__ 1
#undef __UCLIBC_LINUX_MODULE_24__
#define __UCLIBC_LINUX_SPECIFIC__ 1
#define __UCLIBC_HAS_GNU_ERROR__ 1
#define __UCLIBC_BSD_SPECIFIC__ 1
#define __UCLIBC_HAS_BSD_ERR__ 1
#undef __UCLIBC_HAS_OBSOLETE_BSD_SIGNAL__
#undef __UCLIBC_HAS_OBSOLETE_SYSV_SIGNAL__
#undef __UCLIBC_NTP_LEGACY__
#undef __UCLIBC_SV4_DEPRECATED__
#define __UCLIBC_HAS_REALTIME__ 1
#define __UCLIBC_HAS_ADVANCED_REALTIME__ 1
#define __UCLIBC_HAS_EPOLL__ 1
#define __UCLIBC_HAS_XATTR__ 1
#define __UCLIBC_HAS_PROFILING__ 1
#define __UCLIBC_HAS_CRYPT_IMPL__ 1
#undef __UCLIBC_HAS_SHA256_CRYPT_IMPL__
#undef __UCLIBC_HAS_SHA512_CRYPT_IMPL__
#define __UCLIBC_HAS_CRYPT__ 1
#define __UCLIBC_HAS_NETWORK_SUPPORT__ 1
#define __UCLIBC_HAS_SOCKET__ 1
#define __UCLIBC_HAS_IPV4__ 1
#define __UCLIBC_HAS_IPV6__ 1
#define __UCLIBC_HAS_RPC__ 1
#define __UCLIBC_HAS_FULL_RPC__ 1
#define __UCLIBC_HAS_REENTRANT_RPC__ 1
#define __UCLIBC_USE_NETLINK__ 1
#define __UCLIBC_SUPPORT_AI_ADDRCONFIG__ 1
#undef __UCLIBC_HAS_BSD_RES_CLOSE__
#define __UCLIBC_HAS_COMPAT_RES_STATE__ 1
#undef __UCLIBC_HAS_EXTRA_COMPAT_RES_STATE__
#define __UCLIBC_HAS_RESOLVER_SUPPORT__ 1
#define __UCLIBC_HAS_LIBRESOLV_STUB__ 1
#define __UCLIBC_HAS_LIBNSL_STUB__ 1
#undef __UCLIBC_DNSRAND_MODE_URANDOM__
#undef __UCLIBC_DNSRAND_MODE_CLOCK__
#define __UCLIBC_DNSRAND_MODE_PRNGPLUS__ 1
#undef __UCLIBC_DNSRAND_MODE_SIMPLECOUNTER__

/* String and Stdio Support */
#undef __UCLIBC_HAS_STRING_GENERIC_OPT__
#define __UCLIBC_HAS_STRING_ARCH_OPT__ 1
#define __UCLIBC_HAS_CTYPE_TABLES__ 1
#define __UCLIBC_HAS_CTYPE_SIGNED__ 1
#undef __UCLIBC_HAS_CTYPE_UNSAFE__
#undef __UCLIBC_HAS_CTYPE_CHECKED__
#define __UCLIBC_HAS_CTYPE_ENFORCED__ 1
#define __UCLIBC_HAS_WCHAR__ 1
#define __UCLIBC_HAS_LOCALE__ 1
#undef __UCLIBC_BUILD_ALL_LOCALE__
#define __UCLIBC_BUILD_MINIMAL_LOCALE__ 1
#undef __UCLIBC_PREGENERATED_LOCALE_DATA__
#define __UCLIBC_BUILD_MINIMAL_LOCALES__ "en_US"
#define __UCLIBC_HAS_XLOCALE__ 1
#define __UCLIBC_HAS_HEXADECIMAL_FLOATS__ 1
#undef __UCLIBC_HAS_GLIBC_DIGIT_GROUPING__
#define __UCLIBC_HAS_GLIBC_CUSTOM_PRINTF__ 1
#define __UCLIBC_PRINTF_SCANF_POSITIONAL_ARGS__ 9
#undef __UCLIBC_HAS_STDIO_BUFSIZ_NONE__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_256__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_512__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_1024__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_2048__
#define __UCLIBC_HAS_STDIO_BUFSIZ_4096__ 1
#undef __UCLIBC_HAS_STDIO_BUFSIZ_8192__
#define __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_NONE__ 1
#undef __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_4__
#undef __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_8__
#undef __UCLIBC_HAS_STDIO_SHUTDOWN_ON_ABORT__
#undef __UCLIBC_HAS_STDIO_GETC_MACRO__
#undef __UCLIBC_HAS_STDIO_PUTC_MACRO__
#define __UCLIBC_HAS_STDIO_AUTO_RW_TRANSITION__ 1
#undef __UCLIBC_HAS_FOPEN_LARGEFILE_MODE__
#define __UCLIBC_HAS_FOPEN_EXCLUSIVE_MODE__ 1
#undef __UCLIBC_HAS_FOPEN_CLOSEEXEC_MODE__
#define __UCLIBC_HAS_GLIBC_CUSTOM_STREAMS__ 1
#define __UCLIBC_HAS_PRINTF_M_SPEC__ 1
#define __UCLIBC_HAS_ERRNO_MESSAGES__ 1
#undef __UCLIBC_HAS_SYS_ERRLIST__
#define __UCLIBC_HAS_SIGNUM_MESSAGES__ 1
#undef __UCLIBC_HAS_SYS_SIGLIST__
#define __UCLIBC_HAS_GNU_GETOPT__ 1
#define __UCLIBC_HAS_STDIO_FUTEXES__ 1
#define __UCLIBC_HAS_GNU_GETSUBOPT__ 1

/* Big and Tall */
#define __UCLIBC_HAS_REGEX__ 1
#undef __UCLIBC_HAS_REGEX_OLD__
#define __UCLIBC_HAS_FNMATCH__ 1
#undef __UCLIBC_HAS_FNMATCH_OLD__
#undef __UCLIBC_HAS_WORDEXP__
#define __UCLIBC_HAS_NFTW__ 1
#define __UCLIBC_HAS_FTW__ 1
#undef __UCLIBC_HAS_FTS__
#define __UCLIBC_HAS_GLOB__ 1
#define __UCLIBC_HAS_GNU_GLOB__ 1
#undef __UCLIBC_HAS_UTMPX__

/* Library Installation Options */
#define __RUNTIME_PREFIX__ "/"
#define __DEVEL_PREFIX__ "/usr/"
#define __MULTILIB_DIR__ "lib"
#define __HARDWIRED_ABSPATH__ 1

/* Security options */
#undef __UCLIBC_BUILD_PIE__
#undef __UCLIBC_HAS_ARC4RANDOM__
#define __UCLIBC_HAS_SSP__ 1
#undef __UCLIBC_HAS_SSP_COMPAT__
#undef __SSP_QUICK_CANARY__
#define __PROPOLICE_BLOCK_ABRT__ 1
#undef __PROPOLICE_BLOCK_SEGV__
#undef __UCLIBC_BUILD_SSP__
#define __UCLIBC_BUILD_RELRO__ 1
#define __UCLIBC_BUILD_NOW__ 1
#define __UCLIBC_BUILD_NOEXECSTACK__ 1

/* Development/debugging options */
#define __CROSS_COMPILER_PREFIX__ "/home/pedro/buildroot-2012.02-ARM/output/host/hndtools-arm-linux-2.6.36-uclibc-4.5.3/bin/arm-brcm-linux-uclibcgnueabi-"
#define __UCLIBC_EXTRA_CFLAGS__ ""
#undef __DODEBUG__
#define __DOSTRIP__ 1
#undef __DOASSERTS__
#undef __SUPPORT_LD_DEBUG__
#undef __SUPPORT_LD_DEBUG_EARLY__
#undef __UCLIBC_MALLOC_DEBUGGING__
#undef __UCLIBC_HAS_BACKTRACE__
#define __WARNINGS__ "-Wall"
#undef __EXTRA_WARNINGS__
#undef __DOMULTI__
#undef __UCLIBC_MJN3_ONLY__
