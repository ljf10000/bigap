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
    int err, ret;
    
    struct mmc *mmc = find_mmc_device(0);
    if (!mmc) {
        os_println("no-found mmc");
        
        return -EINVAL;
    }

    err = mmc_init(mmc);
    if (err) {
        os_println("init mmc error:%d", err);

        return native_error(err);
    }
    
    ret = mmc->block_dev.block_read(0, begin, count, buf);
    if (ret != count){
        os_println("read emmc(block) error, begin:0x%x, count:0x%x", begin, count);
        
        return -EIO;
    }

    debug_ok("read emmc(block) ok, begin:0x%x, count:0x%x", begin, count);
    
    return ret << 9;
}

static int 
__write_emmc(uint32 begin, void *buf, int count)
{
    int err, ret;
    
    struct mmc *mmc = find_mmc_device(0);
    if (!mmc) {
        os_println("no-found mmc");
        
        return -EINVAL;
    }

    err = mmc_init(mmc);
    if (err) {
        os_println("init mmc error:%d", err);

        return native_error(err);
    }
    
    ret = mmc->block_dev.block_write(0, begin, count, buf);
    if (ret != count) {
        os_println("write emmc(block) error, begin:0x%x, count:0x%x, buf:0x%x", begin, count, buf);
        return -EIO;
    }

    debug_ok("write emmc(block) ok, begin:0x%x, count:0x%x", begin, count);
    
    return ret << 9;
}

int benv_emmc_read(uint32 begin, void *buf, int size)
{
    int ret;

    ret = __read_emmc(begin>>9, buf, size>>9);
    if (ret<0) {
        return ret;
    }

    return size;
}

int benv_emmc_write(uint32 begin, void *buf, int size)
{
    int i, err = -1;

    for (i=0; i<10; i++) {
        os_usleep(10*1000);
        
        err = __write_emmc(begin>>9, buf, size>>9);
        if (size==err) {
            return size;
        }
    }
    
    return err;
}

#define __benv_start(_env, _idx)    (BENV_START + benv_offset(_env, _idx))

int __benv_read(int env, int idx)
{
    /*
    * emmc==>block
    */
    debug_io("read benv[%d:%d] ...", env, idx);
    if (BENV_BLOCK_SIZE!=benv_emmc_read(__benv_start(env, idx), benv_block(env, idx), BENV_BLOCK_SIZE)) {
         os_println("read benv[%d:%d] error", env, idx);
        
        return -EIO;
    }
    debug_io("read benv[%d:%d] ok", env, idx);
    
    /*
    * block==>mirror
    */
    os_memcpy(benv_mirror(env, idx), benv_block(env, idx), BENV_BLOCK_SIZE);

    return 0;
}

int __benv_write(int env, int idx)
{
    debug_io("save benv[%d:%d] ...", env, idx);
    if (BENV_BLOCK_SIZE!=benv_emmc_write(__benv_start(env, idx), benv_block(env, idx), BENV_BLOCK_SIZE)) {
         os_println("save benv[%d:%d] error", env, idx);

        return -EIO;
    }
    debug_io("save benv[%d:%d] ok", env, idx);
    
    os_memcpy(benv_mirror(env, idx), benv_block(env, idx), BENV_BLOCK_SIZE);
    
    return 0;
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
bootcmd_change(void)
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
bootargs_change(void)
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

    char *rootrw = benv_info(__benv_info_pcba_rootrw);
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
bootenv_check(void)
{
    static struct {
        char *k;
        char *v;
    } map[] = {
        {
            .k = "bootargs",
            .v = CONFIG_BOOTARGS,
        },
        {
            .k = "bootcmd",
            .v = PRODUCT_BOOTCOMMAND,
        },
    };
    int i;

    for (i=0; i<os_count_of(map); i++) {
        if (NULL==getenv(map[i].k)) {
            setenv(map[i].k, map[i].v);
        }
    }
}

static void
change_bootenv(void)
{
    bootenv_check();
    bootargs_change();
    bootcmd_change();
}

static void 
benv_boot_check(void) 
{
    benv_show_cookie();

    benv_check();
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
        
    benv_mark(__benv_mark_uptimes) += 1;
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
    if (os_streq("force", getenv("benvsave"))) {
        os_println("benv force save ...");
        benv_save();
        os_println("benv force save end");
    }
    
    if (bootenv_dirty) {
        os_println("bootenv update ...");

        env_crc_update();
        /*
        * include benv
        */
        saveenv();

        bootenv_dirty = false;
        os_println("bootenv update ok.");
    } else {
        benv_save();
    }
}

static void
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
    if (mid != benv_mark(__benv_mark_cid_mid)) {
        benv_mark(__benv_mark_cid_mid) = mid;
    }

    /*
    * try save benv psn
    */
    uint32 psn = bcookie_cid_psn(emmc_cid);
    if (psn != benv_mark(__benv_mark_cid_psn)) {
        benv_mark(__benv_mark_cid_psn) = psn;
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
