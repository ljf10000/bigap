#ifndef __BOOTM_H_ba4cce6615024f03a0b0d14f42e5a32c__
#define __BOOTM_H_ba4cce6615024f03a0b0d14f42e5a32c__
/******************************************************************************/
#ifndef AT_ENV_LINE_SIZE
#define AT_ENV_LINE_SIZE        128
#endif

#ifndef AT_ENV_COUNT
#define AT_ENV_COUNT            100
#endif

#define AT_ROOTFS_OK            'o'
#define AT_ROOTFS_FAIL          'f'
#define AT_ROOTFS_VERFY         'v'

#define AT_ROOTFS_BASE          '7'

/*
* 只能新增表项，禁止修改已有表项的索引
*/
enum {
    AT_ENV_INIT             = 0,
    AT_ENV_ROOTFS           = 1,
    AT_ENV_ROOTFS1          = 2,
    AT_ENV_ROOTFS1ERR       = 3,
    AT_ENV_ROOTFS2          = 4,
    AT_ENV_ROOTFS2ERR       = 5,
    AT_ENV_PCBA_MAC         = 6,
    AT_ENV_PCBA_SN          = 7,
    AT_ENV_PCBA_MODEL       = 8,
    AT_ENV_PCBA_VERSION     = 9,
    AT_ENV_PSN              = 10,
    AT_ENV_MID              = 11,
    AT_ENV_RT               = 12,  /* runtimes */
    AT_ENV_NA               = 13,  /* no-auth */
    AT_ENV_UT               = 14,  /* up-times */
    AT_ENV_BOOTVER          = 15,
    AT_ENV_PCBA_VENDOR      = 16,
    AT_ENV_PCBA_COMPANY     = 17,
    AT_ENV_PRODUCT_MAC      = 18,
    AT_ENV_PRODUCT_SN       = 19,
    AT_ENV_PRODUCT_MODEL    = 20,
    AT_ENV_PRODUCT_VERSION  = 21,
    AT_ENV_PRODUCT_VENDOR   = 22,
    AT_ENV_PRODUCT_COMPANY  = 23,
    AT_ENV_PRODUCT_MANAGER  = 24,

    AT_ENV_OEM_MAC          = 33,
    AT_ENV_OEM_SN           = 34,
    AT_ENV_OEM_MODEL        = 35,
    AT_ENV_OEM_VERSION      = 36,
    AT_ENV_OEM_VENDOR       = 37,
    AT_ENV_OEM_COMPANY      = 38,
    AT_ENV_OEM_MANAGER      = 39,

    AT_ENV_PRIVATE          = 40,
    AT_ENV_PTEST            = (AT_ENV_COUNT-1),
};

#define AT_DEFT_INIT            "f00d1e"
#define AT_DEFT_BOOTVER         "1.2"
#define AT_DEFT_ROOTFS          "1"
#define AT_DEFT_ROOTFS1         "o"
#define AT_DEFT_ROOTFS2         "o"
#define AT_DEFT_ROOTFS1ERR      "0"
#define AT_DEFT_ROOTFS2ERR      "0"

#define AT_NAME(_name, _args...)    {_name, ##_args, NULL}

#define AT_NAME_INIT                AT_NAME("init")
#define AT_NAME_BOOTVER             AT_NAME("bootver",      "boot.version")
#define AT_NAME_ROOTFS              AT_NAME("rootfs",       "os/current")
#define AT_NAME_ROOTFS1             AT_NAME("rootfs1",      "os/rootfs/1/self")
#define AT_NAME_ROOTFS2             AT_NAME("rootfs2",      "os/rootfs/2/self")
#define AT_NAME_ROOTFS1ERR          AT_NAME("rootfs1err",   "os/rootfs/1/error")
#define AT_NAME_ROOTFS2ERR          AT_NAME("rootfs2err",   "os/rootfs/2/error")
#define AT_NAME_PSN                 AT_NAME("psn")
#define AT_NAME_MID                 AT_NAME("mid")
#define AT_NAME_RT                  AT_NAME("rt")
#define AT_NAME_NA                  AT_NAME("na")
#define AT_NAME_UT                  AT_NAME("ut")

#define AT_NAME_PCBA_VENDOR         AT_NAME("pcba.vendor",  "pcba/vendor")
#define AT_NAME_PCBA_COMPANY        AT_NAME("pcba.company", "pcba/company")
#define AT_NAME_PCBA_MODEL          AT_NAME("pcba.model",   "pcba/model")
#define AT_NAME_PCBA_MAC            AT_NAME("pcba.mac",     "pcba/mac")
#define AT_NAME_PCBA_SN             AT_NAME("pcba.sn",      "pcba/sn")
#define AT_NAME_PCBA_VERSION        AT_NAME("pcba.version", "pcba/version")

#define AT_NAME_PRODUCT_VENDOR      AT_NAME("product.vendor",   "product/vendor")
#define AT_NAME_PRODUCT_COMPANY     AT_NAME("product.company",  "product/company")
#define AT_NAME_PRODUCT_MODEL       AT_NAME("product.model",    "product/model")
#define AT_NAME_PRODUCT_MAC         AT_NAME("product.mac",      "product/mac")
#define AT_NAME_PRODUCT_SN          AT_NAME("product.sn",       "product/sn")
#define AT_NAME_PRODUCT_VERSION     AT_NAME("product.version",  "product/version")
#define AT_NAME_PRODUCT_MANAGER     AT_NAME("product.manager",  "product/manager")

#define AT_NAME_OEM_VENDOR          AT_NAME("oem.vendor",   "oem/vendor")
#define AT_NAME_OEM_COMPANY         AT_NAME("oem.company",  "oem/company")
#define AT_NAME_OEM_MODEL           AT_NAME("oem.model",    "oem/model")
#define AT_NAME_OEM_MAC             AT_NAME("oem.mac",      "oem/mac")
#define AT_NAME_OEM_SN              AT_NAME("oem.sn",       "oem/sn")
#define AT_NAME_OEM_VERSION         AT_NAME("oem.version",  "oem/version")
#define AT_NAME_OEM_MANAGER         AT_NAME("oem.manager",  "oem/manager")

#define AT_NAME_PRIVATE(_idx)       AT_NAME("idx" #_idx, "private" #_idx)

#define println(fmt, args...)   printf(fmt "\n", ##args)
#define dprintln println
#define countof_array(x)        (sizeof(x)/sizeof((x)[0]))


#define CONFIG_BOOTARGS_HEAD    \
    "root=/dev/mmcblk0p8"       \
        " "                     \
    /* end */

#define CONFIG_BOOTARGS_BODY    \
    "mem=2G"                    \
        " "                     \
    "console=ttyAMA0,115200"    \
        " "                     \
    "rootfstype=ext4"           \
        " "                     \
    "rootwait"                  \
        " "                     \
    "blkdevparts="              \
        "mmcblk0:"              \
        "512K(boot),"           \
        "512K(argv),"           \
        "4M(baseparam),"        \
        "4M(pqparam),"          \
        "4M(logo),"             \
        "8M(kernel),"           \
        "200M(rootfs0),"        \
        "200M(rootfs1),"        \
        "200M(rootfs2),"        \
        "3000M(rootfs_data),"   \
        "-(others)"             \
        " "                     \
    "mmz=ddr,0,0,300M"          \
    /* end */

#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif

#define CONFIG_BOOTARGS         \
    CONFIG_BOOTARGS_HEAD        \
    CONFIG_BOOTARGS_BODY        \
    /* end */

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#endif

#define CONFIG_BOOTCOMMAND "mmc read 0 0x1000000 0x6800 0x4000;bootm 0x1000000"
/******************************************************************************/
#endif /* __BOOTM_H_ba4cce6615024f03a0b0d14f42e5a32c__ */