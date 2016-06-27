/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#include "benv_boot_common.c"
#include <mmc.h>

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#endif
/*
* kernel0 begin(block count) =  12M/512 =  24K = 0x06000
* kernel1 begin(block count) =  28M/512 =  56K = 0x0E000
* kernel2 begin(block count) =  44M/512 =  88K = 0x16000
* kernel3 begin(block count) =  60M/512 = 120K = 0x1E000
* kernel4 begin(block count) =  76M/512 = 152K = 0x26000
* kernel5 begin(block count) =  92M/512 = 184K = 0x2E000
* kernel6 begin(block count) = 108M/512 = 216K = 0x36000
*
* kernel  size (block count) = 16M/512 =  32K = 0x8000
*/
#define CONFIG_BOOTCOMMAND_BEGIN        "mmc read 0 0x1000000 "
#define CONFIG_BOOTCOMMAND_KERNEL0      "0x06000"
#define CONFIG_BOOTCOMMAND_KERNEL1      "0x0E000"
#define CONFIG_BOOTCOMMAND_KERNEL2      "0x16000"
#define CONFIG_BOOTCOMMAND_KERNEL3      "0x1E000"
#define CONFIG_BOOTCOMMAND_KERNEL4      "0x26000"
#define CONFIG_BOOTCOMMAND_KERNEL5      "0x2E000"
#define CONFIG_BOOTCOMMAND_KERNEL6      "0x36000"
#define CONFIG_BOOTCOMMAND_END          " 0x8000;bootm 0x1000000"
#define CONFIG_BOOTCOMMAND              \
            CONFIG_BOOTCOMMAND_BEGIN    \
            CONFIG_BOOTCOMMAND_KERNEL1  \
            CONFIG_BOOTCOMMAND_END      \
            /* end */
#if 0
static uint32 kernel_offset[OS_FIRMWARE_COUNT] = {
    [0] = 0x06000,
    [1] = 0x0E000,
    [2] = 0x16000,
    [3] = 0x1E000,
    [4] = 0x26000,
    [5] = 0x2E000,
    [6] = 0x36000,
};

/*
* rootfs0 begin(block count) =  124M/512 =  248K = 0x03E000
* rootfs1 begin(block count) =  380M/512 =  760K = 0x0BE000
* rootfs2 begin(block count) =  636M/512 = 1272K = 0x13E000
* rootfs3 begin(block count) =  892M/512 = 1784K = 0x1BE000
* rootfs4 begin(block count) = 1148M/512 = 2298K = 0x23E800
* rootfs5 begin(block count) = 1404M/512 = 2808K = 0x2BE000
* rootfs6 begin(block count) = 1660M/512 = 3320K = 0x33E000
*
* rootfs  size (block count) = 256M/512 = 512K = 0x80000
*/
static uint32 rootfs_offset[OS_FIRMWARE_COUNT] = {
    [0] = 0x03E000,
    [1] = 0x0BE000,
    [2] = 0x13E000,
    [3] = 0x1BE000,
    [4] = 0x23E800,
    [5] = 0x2BE000,
    [6] = 0x33E000,
};
#endif

/*
* bootargs/bootcmd is dirty???
*/
bool bootenv_dirty;
uint32 emmc_cid[4];

static struct bcookie_otp botp = BCOOKIE_OBJ(BCOOKIE_OTP);
static struct bcookie_cid bcid = BCOOKIE_OBJ(BCOOKIE_CID);

static int 
__read_emmc(uint32 begin, void *buf, int count)
{
    struct mmc *mmc = find_mmc_device(0);
    int ret;

    if (!mmc) {
        debug_error("init mmc error");
        return -EINVAL;
    }

    ret = mmc->block_dev.block_read(0, begin, count, buf);
    if (ret != count){
        debug_error("read emmc(block) error, begin:0x%x, count:0x%x", begin, count);
        return -EIO;
    }

    return ret << 9;
}

static int 
__write_emmc(uint32 begin, void *buf, int count)
{
    struct mmc *mmc = find_mmc_device(0);
    int ret;

    if (!mmc) {
        os_println("init mmc error");
        return -EINVAL;
    }

    ret = mmc->block_dev.block_write(0, begin, count, buf);
    if (ret != count) {
        os_println("write emmc(block) error, begin:0x%x, count:0x%x", begin, count);
        return -EIO;
    }

    debug_ok("write emmc(block) ok, begin:0x%x, count:0x%x", begin, count);
    
    return ret << 9;
}

int 
benv_emmc_read(uint32 begin, void *buf, int size)
{
    int ret;

    ret = __read_emmc(begin>>9, buf, size>>9);
    if (ret<0) {
        return ret;
    }

    return size;
}

int 
benv_emmc_write(uint32 begin, void *buf, int size)
{
    int ret;

    ret = __write_emmc(begin>>9, buf, size>>9);
    if (ret<0) {
        return ret;
    }
    
    return size;
}


int
__benv_load(int idx /* benv's block */)
{
    int offset  = BENV_BLOCK_SIZE * idx;
    void *obj   = (char *)__benv_env + offset;
    
    if (BENV_BLOCK_SIZE==benv_emmc_read(BENV_START + offset, obj, BENV_BLOCK_SIZE)) {
        return 0;
    } else {
        return -EIO;
    }
}

int
__benv_save(int idx /* benv's block */)
{
    int offset  = BENV_BLOCK_SIZE * idx;
    void *obj   = (char *)__benv_env + offset;
    
    if (BENV_BLOCK_SIZE==benv_emmc_write(BENV_START + offset, obj, BENV_BLOCK_SIZE)) {
        return 0;
    } else {
        return -EIO;
    }
}

static bool
__change_bootenv(char *name, char *find, char *replace)
{
    char *env, *key, *value;
    
    env = getenv(name);
    if (NULL==env) {
        os_println("no-found env:%s", find);
        
        return false;
    }
    
    key = strstr(env, find);
    if (NULL==key) {
        os_println("no-found:%s in env:%s", find, env);
        
        return false;
    }

    value = key + strlen(find);

    int len = strlen(replace);
    if (0==memcmp(value, replace, len)) {
        return false;
    }
    
    memcpy(value, replace, len);

    bootenv_dirty = true;
    
    return true;
}

static bool
change_bootcmd(void)
{
    int idx = __benv_current;
    
    char *array[OS_FIRMWARE_COUNT] = {
        [0] = CONFIG_BOOTCOMMAND_KERNEL0,
        [1] = CONFIG_BOOTCOMMAND_KERNEL1,
        [2] = CONFIG_BOOTCOMMAND_KERNEL2,
#if OS_FIRMWARE_COUNT > 3
        [3] = CONFIG_BOOTCOMMAND_KERNEL3,
        [4] = CONFIG_BOOTCOMMAND_KERNEL4,
#if OS_FIRMWARE_COUNT > 5
        [5] = CONFIG_BOOTCOMMAND_KERNEL5,
        [6] = CONFIG_BOOTCOMMAND_KERNEL6,
#endif
#endif
    };
    
    return __change_bootenv(
                "bootcmd", 
                CONFIG_BOOTCOMMAND_BEGIN, 
                array[idx]);
}

static bool
change_bootargs(void)
{
    int idx = __benv_current;
    
    char *array[OS_FIRMWARE_COUNT] = {
#if 3==OS_FIRMWARE_COUNT
        [0] = "10",
        [1] = "11",
        [2] = "12",
#elif 5==OS_FIRMWARE_COUNT
        [0] = "11",
        [1] = "12",
        [2] = "13",
        [3] = "14",
        [4] = "15",
#elif 7==OS_FIRMWARE_COUNT
        [0] = "13",
        [1] = "14",
        [2] = "15",
        [3] = "16",
        [4] = "17",
        [5] = "18",
        [6] = "19",
#endif
    };

    char *rootrw = benv_info_get(__benv_info_pcba_rootrw);
    if (false==os_streq(rootrw, PRODUCT_ROOTFS_MODE_RW) && 
        false==os_streq(rootrw, PRODUCT_ROOTFS_MODE_RO)) {
        rootrw = PRODUCT_ROOTFS_MODE;
    }

    __change_bootenv(
            "bootargs", 
            "rootwait ", 
            rootrw);

    return __change_bootenv(
                "bootargs", 
                "root=" OS_MASTER(OS_DEV_FLASH_MASTER), 
                array[idx]);
}

static void
change_bootenv(void)
{
    change_bootargs();
    change_bootcmd();
}

static void 
benv_boot_check(void) 
{
    __benv_show_cookie();
    
    if (false==is_benv_cookie_deft()) {
        os_println("benv first init...");
        
        __benv_deft_cookie();
        __benv_deft_os();
        
        benv_save();
        
        __benv_show_cookie();
    }
}

#if 0
static void
percentage(int count)
{
    static uint32 last            = 0;
    static uint32 last_percent    = 0;
    uint32 persend;
    
    last++;
    persend = last*100/count;
    if (persend > last_percent) {
        last_percent = persend;
        
        os_println("%d%%", persend);
    }
}

static int
copy_emmc(
    uint32 dst,
    uint32 src,
    uint32 count
)
{
    int i, err;
    char buf[BENV_BLOCK_SIZE];

    for (i=0; i<count; i++) {
        err = __read_emmc(src + i, buf, 1);
        if (err<0) {
            return err;
        }

        err = __write_emmc(dst + i, buf, 1);
        if (err<0) {
            return err;
        }

        percentage(count);
    }

    return 0;
}
#endif

static void
benv_select(void)
{
    int skips  = benv_skips(0);
    int current = __benv_current;

    __benv_show_os();
    
    /*
    * try current
    */
    if (__benv_firmware_is_good(current)) {
        os_println("current firmware%d is good", current);

        goto selected;
    }
    else {
        os_println("current firmware%d is bad", current);
    }

    /*
    * try buddy
    */
    int buddy = benv_find_first_good_buddy(firmware, current, skips);
    if (__benv_firmware_is_good(buddy)) {
        os_println("buddy firmware%d is good", buddy);

        __benv_current = buddy;
        
        goto selected;
    }
    else {
        os_println("NO found good buddy firmware");
    }

    /*
    * try best
    */
    int best = benv_find_best(firmware, skips);
    if (__benv_firmware_is_good(best)) {
        os_println("the best firmware%d is good", best);

        __benv_current = best;
        
        goto selected;
    }
    else {
        os_println("all firmware is bad");
    }
    
    /*
    * force 0
    */
    os_println("force firmware0");
    __benv_current = 0;

selected:
    benv_rootfs(__benv_current)->error++;
    benv_kernel(__benv_current)->error++;
    
    benv_mark_add(__benv_mark_uptimes, 1);
    
    bootenv_dirty = true;
}

static void 
benv_boot_select(void) 
{
    benv_select();
    
    change_bootenv();
}

static void
benv_boot_save(void)
{
    if (bootenv_dirty) {
        env_crc_update();
        saveenv();

        bootenv_dirty = false;
    }
}

void
md_boot_init(void)
{
    uint32 zero[4] = {0};
    
    bcookie_add_ref(&botp.header);
    bcookie_add_ref(&bcid.header);

    bcookie_cid_dump(&bcid);
    
    if (os_arrayeq(bcid.cid, zero)) {
        os_arraydcpy(bcid.cid, emmc_cid);

        bcookie_save((struct bcookie *)&bcid, sizeof(bcid));
        
        benv_mark_set(__benv_mark_cid_mid, bcookie_cid_mid(bcid.cid));
        benv_mark_set(__benv_mark_cid_psn, bcookie_cid_psn(bcid.cid));
        
        bootenv_dirty = true;
    }
}

/*
* call it in fastboot
*/
static void
benv_boot(void)
{
    benv_boot_init();
    
    __benv_init();
    
    md_boot_init();
    
    benv_boot_check();
    benv_boot_select();
    benv_boot_save();
}
/******************************************************************************/
