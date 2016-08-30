/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
/*
* must before mmc.h
*/
#include "benv_boot_common.c"
#include <mmc.h>

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#endif
#define CONFIG_BOOTCOMMAND              PRODUCT_BOOTCOMMAND

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

    mmc_init(mmc);

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

    mmc_init(mmc);
    
    ret = mmc->block_dev.block_write(0, begin, count, buf);
    if (ret != count) {
        os_println("write emmc(block) error, begin:0x%x, count:0x%x", begin, count);
        return -EIO;
    }

    debug_ok("write emmc(block) ok, begin:0x%x, count:0x%x", begin, count);
    benv_println("write emmc(block) ok, begin:0x%x, count:0x%x", begin, count);
    
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

    benv_println("benv save block:%d ...", idx);
    if (BENV_BLOCK_SIZE==benv_emmc_write(BENV_START + offset, obj, BENV_BLOCK_SIZE)) {
        benv_println("benv save block:%d ok", idx);
        
        return 0;
    } else {
        benv_println("benv save block:%d error");
        
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
    
    char *array[PRODUCT_FIRMWARE_COUNT] = {
        [0] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART0),
        [1] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART1),
        [2] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART2),
        [3] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART3),
        [4] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART4),
        [5] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART5),
        [6] = __SYMBOL_TO_STRING(PRODUCT_KERNEL_BLKSTART6),
    };
    
    return __change_bootenv(
                "bootcmd", 
                PRODUCT_BOOTCMD_LOADK_BEGIN, 
                array[idx]);
}

static bool
change_bootargs(void)
{
    int idx = __benv_current;
    
    char *array[PRODUCT_FIRMWARE_COUNT] = {
        [0] = "13",
        [1] = "14",
        [2] = "15",
        [3] = "16",
        [4] = "17",
        [5] = "18",
        [6] = "19",
    };

    char *rootrw = benv_info_get(__benv_info_pcba_rootrw);
    if (false==os_streq(rootrw, PRODUCT_ROOTFS_RW) && 
        false==os_streq(rootrw, PRODUCT_ROOTFS_RO)) {
        rootrw = PRODUCT_ROOTFS_MODE;
    }

    __change_bootenv(
            "bootargs", 
            "rootwait ", 
            rootrw);

    return __change_bootenv(
                "bootargs", 
                "root=" PRODUCT_MASTER(PRODUCT_DEV_FLASH_MASTER), 
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
    benv_show_cookie();
    
    if (false==is_benv_cookie_deft()) {
        if (is_benv_cookie_fixed()) {
            /*
            * boot cookie(fixed) == benv cookie(fixed)
            *   so, boot maybe have upgraded
            *       benv cookie need recover
            */
            os_println("benv cookie update...");
        } else {
            /*
            * boot cookie(fixed) != benv cookie(fixed)
            *   so, benv maybe destroy
            *       benv cookie/info need recover
            */
            os_println("benv cookie/info recover...");
            
            __benv_deft_info();
        }
        __benv_deft_cookie();
        
        benv_save();
        
        benv_show_cookie();
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

    benv_show_os();
    
    /*
    * try current
    */
    if (is_benv_good_firmware(current)) {
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
    if (is_benv_good_firmware(buddy)) {
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
    if (is_benv_good_firmware(best)) {
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
    benv_dirty_os();
    
    benv_mark_add(__benv_mark_uptimes, 1);
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
    benv_save();
    
    if (bootenv_dirty) {
        env_crc_update();
        saveenv();

        bootenv_dirty = false;
    }
}

void
md_boot_init(void)
{    
    bcookie_fake(&botp.header);
    bcookie_fake(&bcid.header);

#if PRODUCT_BCOOKIE_ENABLE
    /*
    * load boot cid
    */
    bcookie_load((struct bcookie *)&bcid, sizeof(bcid));
    /*
    * if boot cid != emmc cid
    *   update boot cid
    */
    if (false==os_arrayeq(bcid.cid, emmc_cid)) {
        os_arraydcpy(bcid.cid, emmc_cid);
        bcookie_save((struct bcookie *)&bcid, sizeof(bcid));
    }
#endif

    /*
    * try save benv mid
    */
    uint32 mid = bcookie_cid_mid(emmc_cid);
    if (mid != benv_mark_get(__benv_mark_cid_mid)) {
        benv_mark_set(__benv_mark_cid_mid, mid);
    }

    /*
    * try save benv psn
    */
    uint32 psn = bcookie_cid_psn(emmc_cid);
    if (psn != benv_mark_get(__benv_mark_cid_psn)) {
        benv_mark_set(__benv_mark_cid_psn, psn);
    }
    
    bcookie_cid_dump(emmc_cid);
}

/*
* call it in fastboot
*/
static void
benv_boot(void)
{
    benv_boot_init();
    benv_init();
    md_boot_init();
    
    benv_boot_check();
    benv_boot_select();
    benv_boot_save();
}
/******************************************************************************/
