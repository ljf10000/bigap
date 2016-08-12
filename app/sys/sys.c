/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      sys
#endif

#include "utils.h"
#include "benv/benv.h"
#include "fcookie/fcookie.h"

#if !(IS_PRODUCT_LTEFI_MD2 || IS_PRODUCT_LTEFI_MD3 || IS_PRODUCT_PC)
#error "just support ltefi md2/md3/pc board"
#endif

OS_INITER;
BENV_INITER;

#define DIR_USB_UPGRADE             PRODUCT_DIR_USB "/upgrade"
#define DIR_USB_ROOTFS              DIR_USB_UPGRADE "/rootfs"
#define DIR_USB_CONFIG              DIR_USB_UPGRADE "/config"
#define DIR_USB_DATA                DIR_USB_UPGRADE "/data"
#define DIR_USB_TOOL                DIR_USB_UPGRADE "/tool"
#define DIR_USB_OTHER               DIR_USB_UPGRADE "/other"

#define USB_FILE(_file)             DIR_USB_ROOTFS  "/" _file
#define USB_TOOL_FILE(_file)        DIR_USB_TOOL    "/" _file

#define SCRIPT_FIRMWARE             SCRIPT_FILE("formware_check.sh")
#define SCRIPT_MOUNT                SCRIPT_FILE("mount.script")
#define SCRIPT_HOTPLUG              SCRIPT_FILE("hotplug/disk.cb")

#define SCRIPT_XCOPY                PRODUCT_FILE("usr/sbin/xcopy")
#define SCRIPT_CURRENT              PRODUCT_FILE("usr/sbin/syscurrent")

#define SCRIPT_USBUPGRADE_INIT      SCRIPT_FILE("usbupgrade/init.cb")
#define SCRIPT_USBUPGRADE_OK        SCRIPT_FILE("usbupgrade/ok.cb")
#define SCRIPT_USBUPGRADE_FAIL      SCRIPT_FILE("usbupgrade/fail.cb")

#define __FILE_VERSION              "etc/" PRODUCT_FILE_VERSION
#define __FILE_ROOTFS_VERSION       __FILE_VERSION
#define __FILE_KERNEL_VERSION       __FILE_VERSION
#define __BIN_MD_PQPARAM            "image/" PRODUCT_BIN_MD_PQPARAM
#define __BIN_MD_BASEPARAM          "image/" PRODUCT_BIN_MD_BASEPARAM
#define __BIN_MD_KERNEL             "image/" PRODUCT_BIN_MD_KERNEL
#define __BIN_MD_BOOT               "image/" PRODUCT_BIN_MD_BOOT
#define __BIN_MD_BOOTENV            "image/" PRODUCT_BIN_MD_BOOTENV
#define __BIN_AP_BOOT               "image/" PRODUCT_BIN_AP_BOOT
#define __BIN_AP_BOOTENV            "image/" PRODUCT_BIN_AP_BOOTENV
#define __BIN_AP_FIRMWARE           "image/" PRODUCT_BIN_AP_FIRMWARE

#define FILE_ROOTFS_VERSION         PRODUCT_FILE(__FILE_ROOTFS_VERSION)
#define FILE_KERNEL_VERSION         PRODUCT_FILE(__FILE_KERNEL_VERSION)
#define BIN_MD_PQPARAM              PRODUCT_FILE(__BIN_MD_PQPARAM)
#define BIN_MD_BASEPARAM            PRODUCT_FILE(__BIN_MD_BASEPARAM)
#define BIN_MD_KERNEL               PRODUCT_FILE(__BIN_MD_KERNEL)
#define BIN_MD_BOOT                 PRODUCT_FILE(__BIN_MD_BOOT)
#define BIN_MD_BOOTENV              PRODUCT_FILE(__BIN_MD_BOOTENV)
#define BIN_AP_BOOT                 PRODUCT_FILE(__BIN_AP_BOOT)
#define BIN_AP_BOOTENV              PRODUCT_FILE(__BIN_AP_BOOTENV)
#define BIN_AP_FIRMWARE             PRODUCT_FILE(__BIN_AP_FIRMWARE)

#define USB_FILE_ROOTFS_VERSION     USB_FILE(__FILE_ROOTFS_VERSION)
#define USB_FILE_KERNEL_VERSION     USB_FILE(__FILE_KERNEL_VERSION)
#define USB_BIN_MD_PQPARAM          USB_FILE(__BIN_MD_PQPARAM)
#define USB_BIN_MD_BASEPARAM        USB_FILE(__BIN_MD_BASEPARAM)
#define USB_BIN_MD_KERNEL           USB_FILE(__BIN_MD_KERNEL)
#define USB_BIN_MD_BOOT             USB_FILE(__BIN_MD_BOOT)
#define USB_BIN_MD_BOOTENV          USB_FILE(__BIN_MD_BOOTENV)
#define USB_BIN_AP_BOOT             USB_FILE(__BIN_AP_BOOT)
#define USB_BIN_AP_BOOTENV          USB_FILE(__BIN_AP_BOOTENV)
#define USB_BIN_AP_FIRMWARE         USB_FILE(__BIN_AP_FIRMWARE)

#define __OBJ(_idx, _dev, _dir)     [_idx] = {.dev = _dev, .dir = _dir}
#define OBJ_KERNEL(_idx)            __OBJ(_idx, PRODUCT_IDEV_KERNEL(_idx),  PRODUCT_IDIR_KERNEL(_idx))
#define OBJ_ROOTFS(_idx)            __OBJ(_idx, PRODUCT_IDEV_ROOTFS(_idx),  PRODUCT_IDIR_ROOTFS(_idx))
#define OBJ_CONFIG(_idx)            __OBJ(_idx, PRODUCT_IDEV_CONFIG(_idx),  PRODUCT_IDIR_CONFIG(_idx))
#define OBJ_TOOL(_idx)              __OBJ(_idx, PRODUCT_IDEV_TOOL(_idx),    PRODUCT_IDIR_TOOL(_idx))
#define OBJ_DATA(_idx)              __OBJ(_idx, PRODUCT_IDEV_DATA(_idx),    PRODUCT_IDIR_DATA(_idx))

static struct {
    int current;
    int cmaster;    // config
    int tmaster;    // tool
    int dmaster;    // data
    int upgrade;
    int idx; /* rootfs idx */
    int resetby;
    
    bool dirty;
    benv_version_t version;
    benv_version_t old_version;
    
    env_cache_t env;

    struct {
        char *dev;
        char *dir;
    } kernel[PRODUCT_FIRMWARE_COUNT], rootfs[PRODUCT_FIRMWARE_COUNT], tool[2], data[2], config[2];
} 
sys = {
    .current= PRODUCT_FIRMWARE_CURRENT,
    .upgrade= BENV_UPGRADE_COUNT,

    .kernel = {
        OBJ_KERNEL(0),
        OBJ_KERNEL(1),
        OBJ_KERNEL(2),
        OBJ_KERNEL(3),
        OBJ_KERNEL(4),
        OBJ_KERNEL(5),
        OBJ_KERNEL(6),
    },
    
    .rootfs = {
        OBJ_ROOTFS(0),
        OBJ_ROOTFS(1),
        OBJ_ROOTFS(2),
        OBJ_ROOTFS(3),
        OBJ_ROOTFS(4),
        OBJ_ROOTFS(5),
        OBJ_ROOTFS(6),
    },

    .config = {
        OBJ_CONFIG(0),
        OBJ_CONFIG(1),
    },

    .tool = {
        OBJ_TOOL(0),
        OBJ_TOOL(1),
    },

    .data = {
        OBJ_DATA(0),
        OBJ_DATA(1),
    },
};

#define __skips(_idx)           (benv_skips(sys.current) | os_bit(_idx))
#define __cslave                (!sys.cmaster)
#define __tslave                (!sys.tmaster)
#define __dslave                (!sys.dmaster)
#define __cmaster               (!__cslave) // config
#define __tmaster               (!__tslave) // tool
#define __dmaster               (!__dslave) // data

#define dev_kernel(_idx)        sys.kernel[_idx].dev
#define dev_rootfs(_idx)        sys.rootfs[_idx].dev
#define dev_config(_idx)        sys.config[_idx].dev
#define dev_tool(_idx)          sys.tool[_idx].dev
#define dev_data(_idx)          sys.data[_idx].dev

#define dev_kernel_current      dev_kernel(sys.current)
#define dev_rootfs_current      dev_rootfs(sys.current)
#define dev_config_master       dev_config(__cmaster)
#define dev_tool_master         dev_tool(__tmaster)
#define dev_data_master         dev_data(__dmaster)

#define dir_kernel(_idx)        sys.kernel[_idx].dir
#define dir_rootfs(_idx)        sys.rootfs[_idx].dir
#define dir_config(_idx)        sys.config[_idx].dir
#define dir_tool(_idx)          sys.tool[_idx].dir
#define dir_data(_idx)          sys.data[_idx].dir

#define dir_kernel_current      dir_kernel(sys.current)
#define dir_rootfs_current      dir_rootfs(sys.current)
#define dir_config_master       dir_config(__cmaster)
#define dir_tool_master         dir_data(__tmaster)
#define dir_data_master         dir_data(__dmaster)

#define is_current_rootfs_dev(_dev) os_streq(dev_rootfs_current, _dev)
#define is_current_rootfs_dir(_dir) os_streq(dir_rootfs_current, _dir)

#define get_rootfs_zone(_begin, _end) do{   \
    if (is_normal_benv_idx(sys.idx)) {      \
        _begin   = sys.idx;                 \
        _end     = _begin + 1;              \
    } else {                                \
        _begin   = 0;                       \
        _end     = PRODUCT_FIRMWARE_COUNT;  \
    }                                       \
}while(0)

static char *
rootfs_file(int idx, char *file)
{
    static char line[1+OS_FILENAME_LEN];

    os_saprintf(line, "%s/%s", dir_rootfs(idx), file);

    return line;
}

#define dev_obj(_obj, _idx)     dev_##_obj(_idx)
#define dir_obj(_obj, _idx)     dir_##_obj(_idx)

#define set_obj(_obj, _idx, _field, _value) do{ \
    benv_obj(_obj, _idx)->_field = _value;      \
                                                \
    sys.dirty = true;                           \
}while(0)

#define efsm(_err)  ((_err)?BENV_FSM_FAIL:BENV_FSM_OK)

#define SYS_PRINT   0

#if SYS_PRINT
#define sys_println(_fmt, _args...)     os_println(_fmt, ##_args)
#else
#define sys_println(_fmt, _args...)     os_do_nothing()
#endif

static int
__dd(char *dst, char *src)
{
    return os_p_system("dd if=%s of=%s bs=4096", src, dst);
}

static char *
get_pwdfile(void)
{
    static char pwd[1+FCOOKIE_FILE_LEN];
    
    return  sys.env.pwdfile?
            sys.env.pwdfile:
            __fcookie_file(FCOOKIE_RSYNC_PWDFILE, pwd);
}

static void
put_pwdfile(char *pwdfile)
{
    if (pwdfile && NULL==sys.env.pwdfile) {
        unlink(pwdfile);
    }
}

static int
__rsync(int idx, benv_version_t *version)
{
    int err;
    char new[1+BENV_VERSION_STRING_LEN];
    char old[1+BENV_VERSION_STRING_LEN];
    
    char *pwdfile = get_pwdfile();
    if (NULL==pwdfile) {
        return -EIO;
    }

    __benv_version_itoa(version, new);
    __benv_version_itoa(&sys.old_version, old);

    set_obj(rootfs, idx, upgrade, BENV_FSM_FAIL);
    err = os_p_system("rsync"
            " -acz --delete --force --stats --partial"
            " --exclude=/dev/*"
            " --timeout=%s"
            " --password-file=%s"
            " rsync://%s@%s:%s/%s/%s/rootfs/"
            " %s",
            sys.env.timeout,
            pwdfile,
            sys.env.user,
                sys.env.server,
                sys.env.port,
                sys.env.path,
                new,
            dir_rootfs(idx));
    set_obj(rootfs, idx, upgrade, efsm(err));
    put_pwdfile(pwdfile);

    jinfo("%o", 
        "upgrade", jobj_oprintf("%s%s%s%s%o%d",
                        "type", "cloud",
                        "obj", "rootfs",
                        "src", sys.env.server,
                        "dst", dir_rootfs(idx),
                        "version", jobj_oprintf("%s%s", 
                                        "old", old, 
                                        "new", new),
                        "error", err));

    return err;
}

static int
__xcopy(char *dst, char *src)
{
    int err;
    
    sys_println("before sync %s==>%s", src, dst);
    err = os_p_system(SCRIPT_XCOPY " %s %s", src, dst);
    sys_println("after  sync %s==>%s", src, dst);
    
    return err;
}

static int
__rcopy(int idx, char *dir, benv_version_t *version)
{
    int err;
    char new[1+BENV_VERSION_STRING_LEN];
    char old[1+BENV_VERSION_STRING_LEN];
    
    __benv_version_itoa(version, new);
    __benv_version_itoa(&sys.old_version, old);

    set_obj(rootfs, idx, upgrade, BENV_FSM_FAIL);
    err = __xcopy(dir_rootfs(idx), dir);
    set_obj(rootfs, idx, upgrade, efsm(err));
    
    jinfo("%o",
        "upgrade", jobj_oprintf("%s%s%s%s%o%d",
                        "type", "local",
                        "obj", "rootfs",
                        "src", dir,
                        "dst", dir_rootfs(idx),
                        "version", jobj_oprintf("%s%s", 
                                        "old", old, 
                                        "new", new),
                        "error", err));

    return err;
}

static int
__umount(char *dir)
{
    return os_p_system("umount %s", dir);
}

#if 0
#define ROOTFS_MOUNT_MODE   ",data=journal"
#else
#define ROOTFS_MOUNT_MODE   __empty
#endif

#if 0
static int
__mount(char *dev, char *dir, bool readonly)
{
#if 0
mount has the following return codes (the bits can be ORed):

0:success
1 - incorrect invocation or permissions
2 - system error (out of memory, cannot fork, no more loop devices)
4 - internal mount bug
8 - user interrupt
16 - problems writing or locking /etc/mtab
32 - mount failure
64 - some mount succeeded
#endif
    int err;
    
    sys_println("before mount %s", dev);
    err = os_p_system("mount -t ext4 "
            "-o %s,noatime,nodiratime "
            "%s %s",
            readonly?"ro":"rw" ROOTFS_MOUNT_MODE,
            dev, dir);
    sys_println("after  mount %s", dev);

    return err;
}
#endif

static int
__is_readonly_default(void)
{
    return os_streq(PRODUCT_ROOTFS_RO, benv_info_get(__benv_info_pcba_rootrw));
}

static int
__remount(char *dir, bool readonly)
{
    int err = 0;
    
    if (__is_readonly_default()) {
        err = os_p_system("mount -o remount" ROOTFS_MOUNT_MODE ",%s %s &> /dev/null",
                    readonly?"ro":"rw",
                    dir);
    }

    return err;
}

static int
do_umount(char *dir)
{
    if (is_current_rootfs_dev(dir) /* yes, dir */ || is_current_rootfs_dir(dir)) {
        debug_trace("skip umount current rootfs %s", dir);
        
        return 0;
    } else {
        return __umount(dir);
    }
}

static int
do_mount(char *dev, char *dir, bool check, bool readonly, bool repair)
{
    return os_p_system(SCRIPT_MOUNT " %s %s %s %s %s", 
                dev, 
                dir,
                os_yesno(check),
                readonly?"ro":"rw",
                os_yesno(repair));
}

static int
do_mount_wait(int wait, char *dev, char *dir, bool check, bool readonly, bool repair)
{
    int i, err;

    for (i=0; i<wait; i++) {
        if (os_file_exist(dev)) {
            return do_mount(dev, dir, check, readonly, repair);
        }

        sleep(1);
    }

    return -ENOEXIST;
}

static int
mount_double(
    char *name, char *dir_master,
    char *dev0, char *dir0,
    char *dev1, char *dir1,
    int *master
)
{
    int err;
    int err0 = 0;
    int err1 = 0;
    
    err0 = do_mount(dev0, dir_master, 
            true,   /* check    */
            false,  /* readonly */
            true);  /* repair   */
    err1 = do_mount(dev1, dir1, 
            true,   /* check    */
            false,  /* readonly */
            true);  /* repair   */

    if (err0 || err1) {
        trace_assert(0, "mount %s error:%d and %s error:%d", dev0, err0, dev1, err1);
        
        return -EMOUNT;
    }
    else if (err0) {
        __xcopy(dir0, dir1);
    }
    
    else if (err1) {
        __xcopy(dir1, dir0);
    }
    
    *master = 0;
    
    return 0;
}

static int
mount_config(void)
{
    return mount_double("config", PRODUCT_DIR_CONFIG,
                dev_config(0), dir_config(0), 
                dev_config(1), dir_config(1),
                &sys.cmaster);
}

static int
mount_tool(void)
{
    return mount_double("tool", PRODUCT_DIR_TOOL,
                dev_tool(0), dir_tool(0), 
                dev_tool(1), dir_tool(1),
                &sys.tmaster);
}

static int
mount_data(void)
{
    return mount_double("data", PRODUCT_DIR_DATA,
                dev_data(0), dir_data(0), 
                dev_data(1), dir_data(1),
                &sys.dmaster);
}

static int
mount_other(void)
{
    return do_mount(PRODUCT_DEV_OTHER, PRODUCT_DIR_OTHER, 
                true,   /* check    */
                false,  /* readonly */
                true);  /* repair   */
}

static int
mount_rootfs(void)
{
    int i, err, errs = 0;
    bool readonly = __is_readonly_default();

    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        if (i!=sys.current) {
            err = do_mount(dev_rootfs(i), dir_rootfs(i), 
                    true,   /* check    */
                    readonly,
                    true);  /* repair   */
            if (err<0) {
                errs = err; /* NOT return */
            }
        }
    }

    return errs;
}

static int
mount_hd(void)
{
    return do_mount_wait(3, PRODUCT_DEV_HD, PRODUCT_DIR_HD, 
            true,   /* check    */
            false,  /* readonly */
            false); /* repair   */
}

static int
mount_sd(void)
{
    return do_mount_wait(2, PRODUCT_DEV_SD, PRODUCT_DIR_SD, 
            true,   /* check    */
            false,  /* readonly */
            false); /* repair   */
}

static int
mount_usb(void)
{
    if (false==os_file_exist(SCRIPT_HOTPLUG)) {
        return do_mount_wait(2, PRODUCT_DEV_USB, PRODUCT_DIR_USB, 
                true,   /* check    */
                false,  /* readonly */
                false); /* repair   */
    } else {
        return 0;
    }
}

static int
umount_double(char *dir_master, char *dir0, char *dir1)
{
    int err;

    err = do_umount(dir_master);
    if (err<0) {
        /* log */
    }

    err = do_umount(dir0);
    if (err<0) {
        /* log */
    }

    err = do_umount(dir1);
    if (err<0) {
        /* log */
    }

    return err;
}

static int
umount_config(void)
{
    return umount_double(PRODUCT_DIR_CONFIG, dir_config(0), dir_config(1));
}

static int
umount_tool(void)
{
    return umount_double(PRODUCT_DIR_TOOL, dir_tool(0), dir_tool(1));
}

static int
umount_data(void)
{
    return umount_double(PRODUCT_DIR_DATA, dir_data(0), dir_data(1));
}

static int
umount_other(void)
{
    return do_umount(PRODUCT_DIR_OTHER);
}

static int
umount_rootfs(void)
{
    int i, err, errs = 0;

    for (i=0; i<PRODUCT_FIRMWARE_COUNT; i++) {
        if (i!=sys.current) {
            err = do_umount(dir_rootfs(i));
            if (err<0) {
                errs = err;
            }
        }
    }

    return errs;
}

static int
umount_hd(void)
{
    return do_umount(PRODUCT_DIR_HD);
}

static int
umount_sd(void)
{
    return do_umount(PRODUCT_DIR_SD);
}

static int
umount_usb(void)
{
    if (false==os_file_exist(SCRIPT_HOTPLUG)) {
        return do_umount(PRODUCT_DIR_USB);
    } else {
        return 0;
    }
}

static struct {
    int (*mount)(void);
    int (*umount)(void);
} mounts[] = {
    {
         .mount =  mount_rootfs,
        .umount = umount_rootfs,
    },
    {
         .mount =  mount_config,
        .umount = umount_config,
    },
    {
         .mount =  mount_tool,
        .umount = umount_tool,
    },
    {
         .mount =  mount_data,
        .umount = umount_data,
    },
    {
         .mount =  mount_other,
        .umount = umount_other,
    },
    {
         .mount =  mount_sd,
        .umount = umount_sd,
    },
    {
         .mount =  mount_hd,
        .umount = umount_hd,
    },
    {
         .mount =  mount_usb,
        .umount = umount_usb,
    },
};

static benv_version_t *
__get_fversion(int idx, char *file, benv_version_t *version)
{
    char string[1+OS_LINE_LEN] = {0};
    int err;
    
    err = os_v_fgets(string, OS_LINE_LEN, "%s/%s", dir_rootfs(idx), file);
    if (err<0) {
        debug_error("read version file %s/%s error:%d", dir_rootfs(idx), file, err);
        
        return NULL;
    }

    if (__benv_version_atoi(version, string)) {
        debug_error("bad version file %s/%s", dir_rootfs(idx), file);
        
        return NULL;
    }

    return version;
}

static benv_version_t *
rootfs_version(int idx, benv_version_t *version)
{
    static benv_version_t version;
    
    return __get_fversion(idx, __FILE_ROOTFS_VERSION, &version);
}

static benv_version_t *
kernel_version(int idx, benv_version_t *version)
{
    static benv_version_t version;
    
    return __get_fversion(idx, __FILE_KERNEL_VERSION, &version);
}

static int
save(void)
{
    int err = 0;
    
    if (sys.dirty) {
        err = os_call(benv_open, benv_close, __benv_save, BENV_OS);
        
        sys.dirty = false;
    }

    return err;
}

static int
load(void)
{
    save();
    
    return os_call(benv_open, benv_close, __benv_load, BENV_OS);
}

/*
* upgrade prepare
*   set obj fsm failed
*   save obj version
*/
#define __upgrade_init(_obj, _idx, _version)    do{     \
    benv_obj(_obj, _idx)->upgrade = BENV_FSM_UNKNOW;    \
    benv_obj(_obj, _idx)->other   = BENV_FSM_UNKNOW;    \
    benv_obj(_obj, _idx)->self    = BENV_FSM_UNKNOW;    \
    benv_obj(_obj, _idx)->error   = 0;                  \
                                                        \
    os_objcpy(benv_obj_version(_obj, _idx), _version);  \
                                                        \
    os_objcpy(&sys.old_version, benv_obj_version(_obj, _idx)); \
    sys.dirty = true;                                   \
}while(0)

/*
* upgrade prepare
*   set rootfs and kernel fsm failed
*   save rootfs and kernel version
*/
static void
upgrade_init(int idx, benv_version_t *version)
{
    __upgrade_init(rootfs, idx, version);
    __upgrade_init(kernel, idx, version);

    save();
}


static int
__dir_rsync_init(int idx, benv_version_t *version)
{
    __upgrade_init(rootfs, idx, version);
    
    save();
    
    return __remount(dir_rootfs(idx), false);
}

static int
__dir_rsync_fini(int idx, benv_version_t *version)
{
    (void)version;
    
    save();
    
    return __remount(dir_rootfs(idx), true);
}

/*
* rsync cloud ==> rootfs idx
*/
static int
rsync(int idx, benv_version_t *version)
{
    return os_call_2(__dir_rsync_init, __dir_rsync_fini, __rsync, idx, version);
}

static int
__dir_rcopy_init(int dst, char *dir, benv_version_t *version)
{
    (void)dir;
    
    __upgrade_init(rootfs, dst, version);
    
    save();
    
    return __remount(dir_rootfs(dst), false);
}

static int
__dir_rcopy_fini(int dst, char *dir, benv_version_t *version)
{
    (void)dir;
    (void)version;
    
    save();
    
    return __remount(dir_rootfs(dst), true);
}

/*
* rsync src dir ==> rootfs idx
*/
static int
rcopy(int idx, char *dir, benv_version_t *version)
{
    return os_call_3(__dir_rcopy_init, __dir_rcopy_fini, __rcopy, idx, dir, version);
}

static int
__rdd_init(int dst, int src)
{
    __upgrade_init(rootfs, dst, benv_rootfs_version(src));
    
    return save();
}

static int
__rdd_fini(int dst, int src)
{
    (void)dst;
    (void)src;
    
    return save();
}

static int
__rdd(int dst, int src)
{
    int err;
    char new[1+BENV_VERSION_STRING_LEN];
    char old[1+BENV_VERSION_STRING_LEN];
    
    __benv_version_itoa(benv_rootfs_version(src), new);
    __benv_version_itoa(&sys.old_version, old);

    err = __dd(dev_rootfs(dst), dev_rootfs(src));
    set_obj(rootfs, dst, upgrade, efsm(err));

    jinfo("%o",
        "upgrade", jobj_oprintf("%s%s%s%s%o%d",
                        "type", "local",
                        "obj", "rootfs",
                        "src", dev_rootfs(src),
                        "dst", dev_rootfs(dst),
                        "version", jobj_oprintf("%s%s", 
                                        "old", old, 
                                        "new", new),
                        "error", err));

    return err;
}

/*
* dd rootfs src ==> rootfs dst
*/
static int
rdd(int dst, int src)
{
    return os_call_2(__rdd_init, __rdd_fini, __rdd, dst, src);
}

static int
__kdd_bydev_init(int dst, int src)
{
    (void)src;
    
    __upgrade_init(kernel, dst, benv_kernel_version(src));
    
    return save();
}

static int
__kdd_bydev_fini(int dst, int src)
{
    (void)dst;
    (void)src;
    
    return save();
}

static int
__kdd_bydev(int dst, int src)
{
    int err;
    char new[1+BENV_VERSION_STRING_LEN];
    char old[1+BENV_VERSION_STRING_LEN];
    
    os_strcpy(new, benv_kernel_version_string(src));
    __benv_version_itoa(&sys.old_version, old);
    
    err = __dd(dev_kernel(dst), dev_kernel(src));
    set_obj(kernel, dst, upgrade, efsm(err));

    jinfo("%o",
        "upgrade", jobj_oprintf("%s%s%s%s%o%d",
                        "type", "local",
                        "obj", "kernel",
                        "src", dev_kernel(src),
                        "dst", dev_kernel(dst),
                        "version", jobj_oprintf("%s%s", 
                                        "old", old, 
                                        "new", new),
                        "error", err));

    return err;
}

/*
* dd kernel src ==> kernel dst
*/
static int
kdd_bydev(int dst, int src)
{
    return os_call_2(__kdd_bydev_init, __kdd_bydev_fini, __kdd_bydev, dst, src);
}

static int
__kdd_byfile_init(int idx, char *file, benv_version_t *version)
{
    (void)file;
    
    __upgrade_init(kernel, idx, version);
    
    return save();
}

static int
__kdd_byfile_fini(int idx, char *file, benv_version_t *version)
{
    (void)idx;
    (void)file;
    (void)version;
    
    return save();
}

static int
__kdd_byfile(int idx, char *file, benv_version_t *version)
{
    int err;
    char new[1+BENV_VERSION_STRING_LEN];
    char old[1+BENV_VERSION_STRING_LEN];
    
    __benv_version_itoa(version, new);
    __benv_version_itoa(&sys.old_version, old);
    
    err = __dd(dev_kernel(idx), file);
    set_obj(kernel, idx, upgrade, efsm(err));

    jinfo("%o",
        "upgrade", jobj_oprintf("%s%s%s%s%o%d",
                        "type", "local",
                        "obj", "kernel",
                        "src", file,
                        "dst", dev_kernel(idx),
                        "version", jobj_oprintf("%s%s", 
                                        "old", old, 
                                        "new", new),
                        "error", err));

    return err;
}

/*
* dd kernel file ==> kernel idx
*/
static int
kdd_byfile(int idx, char *file, benv_version_t *version)
{
    return os_call_3(__kdd_byfile_init, __kdd_byfile_fini, __kdd_byfile, idx, file, version);
}

static int
bdd(char *obj, char *dev, char *file)
{
    int err;

    err = __dd(dev, file);
    
    jinfo("%o",
        "upgrade", jobj_oprintf("%s%s%s%s%d",
                        "type", "local",
                        "obj", obj,
                        "src", file,
                        "dst", dev,
                        "error", err));

    return err;
}

static int
switch_to(int idx)
{
    jinfo("%o", 
        "switch", jobj_oprintf("%d%d",
                        "from", __benv_current,
                        "to", idx));

    benv_rootfs(idx)->error   = 0;
    benv_kernel(idx)->error   = 0;
    
    benv_rootfs(idx)->upgrade = BENV_FSM_OK;
    benv_kernel(idx)->upgrade = BENV_FSM_OK;
    
    benv_rootfs(idx)->self    = BENV_FSM_UNKNOW;
    benv_kernel(idx)->self    = BENV_FSM_UNKNOW;
    
    __benv_current = idx;
    sys.dirty = true;
    save();
    
    return 0;
}

static void 
debug_rootfs_upgrade(int idx, char *master, char *rootfs)
{
    debug_ok("%s upgrade %s rootfs%d %s==>%s ", 
        master, 
        rootfs, 
        idx, 
        benv_rootfs_version_string(idx), 
        sys.env.version);
}

static int
get_upgrade_byversion(benv_version_t *version, int skips)
{
    int idx;
    
    /*
    * try get bad by version
    */
    idx = benv_find_first_bad_byversion(rootfs, version, skips);
    if (is_benv_good_rootfs(idx)) {
        debug_rootfs_upgrade(idx, "master", "good");

        return idx;
    }

    /*
    * try get good(maybe fake) by version
    */
    idx = benv_find_first_good_byversion(rootfs, version, skips);
    if (is_benv_good_rootfs(idx)) {
        debug_rootfs_upgrade(idx, "master", "bad");

        return idx;
    }

    /*
    * try get first by version
    */
    idx = benv_find_first_byversion(rootfs, version, skips);
    if (is_good_benv_idx(idx)) {
        debug_rootfs_upgrade(idx, "master", "first");

        return idx;
    }

    return -ENOEXIST;
}

static int
get_upgrade(int skips)
{
    int idx = get_upgrade_byversion(&sys.version, skips);
    if (is_good_benv_idx(idx)) {
        return idx;
    }
    
    idx = benv_find_worst(rootfs, skips);
    if (is_good_benv_idx(idx)) {
        debug_rootfs_upgrade(idx, "master", "worst");
    } else {
        trace_assert(0, "no found worst slave");
        idx = benv_first_idx(sys.current, skips);
        debug_rootfs_upgrade(idx, "slave", "force");
    }

    return idx;
}

static int
repair_kernel(int idx, benv_version_t *version)
{
    int err = kdd_byfile(idx, rootfs_file(idx, __BIN_MD_KERNEL), version);

    jcrit("%s%d%d", 
        "repair", "kernel",
        "index", idx,
        "error", err);
                    
    return err;
}

static bool
is_rootfs_need_repair(int idx)
{
    if (false==is_benv_good_rootfs(idx)) {
        jcrit("%d%s%s",
            "rootfs", idx,
            "state", "bad",
            "todo", "repair");

        return true;
    }
    else if (false==os_file_exist(SCRIPT_FIRMWARE)) {
        jcrit("%d%s%s",
            "rootfs", idx,
            "nofound", SCRIPT_FIRMWARE,
            "todo", "repair");

        return true;
    }
    else if (0!=os_p_system(SCRIPT_FIRMWARE " %d", idx)) {
        jcrit("%d%s%s",
            "rootfs", idx,
            "check", "failed",
            "todo", "repair");

        return true;
    }

    return false;
}

static int
repair_rootfs(int idx)
{
    benv_version_t version;
    int err = 0;
    
    if (false==is_rootfs_need_repair(idx)) {
        debug_ok("rootfs%d needn't repair", idx);
        
        return 0;
    }

    /*
    * get rootfs idx's version
    */
    os_objcpy(&version, benv_rootfs_version(idx));
    /*
    * find a good rootfs with same version
    */
    int buddy = benv_find_first_good_byversion(rootfs, &version, __skips(idx));
    jcrit("%s%d%s%s%d",
        "repair", "rootfs",
        "index", idx,
        "version", benv_version_itoa(&version),
        "find", "buddy",
        "index", idx);

    if (is_benv_good_rootfs(buddy)) {
        err = rcopy(idx, dir_rootfs(buddy), &version);
        jcrit("%s%d%d%d",
            "repair", "rootfs",
            "index", idx,
            "buddy", buddy,
            "error", err);
    } else {
        err = rsync(idx, &version);
        jcrit("%s%d%d",
            "repair", "rootfs",
            "index", idx,
            "error", err);
    }
    
    if (err<0) {
        return 0;
    }
    
    return repair_kernel(idx, &version);
}

static int
reset_kernel(int idx)
{
    int err = kdd_byfile(idx, rootfs_file(idx, __BIN_MD_KERNEL), benv_kernel_version(idx));

    jcrit("%s%d%d%d",
        "reset", "kernel",
        "index", idx,
        "by", sys.resetby,
        "error", err);

    return err;
}

static int
reset_rootfs(int idx, int resetby)
{
    int err = rcopy(idx, dir_rootfs(resetby), benv_rootfs_version(resetby));
    
    jcrit("%s%d%d%d",
        "reset", "rootfs",
        "index", idx,
        "by", sys.resetby,
        "error", err);

    return err;
}

/*
* idx: the rootfs will rsync by cloud
* src: the source rootfs
*/
static int
upgrade(int idx, int src)
{
    int err = 0;
    benv_version_t *version = &sys.version;
    
    /*
    * 1. not force mode
    * 2. rootfsX/kernelX is good
    * 3. rootfsX/kernelX version == upgrade version
    */
    if (NULL==sys.env.force
        && is_benv_good_firmware(idx)
        && benv_version_eq(version, benv_rootfs_version(idx))
        && benv_version_eq(version, benv_kernel_version(idx))) {

        jinfo("%d%o",
            "rootfs", idx,
            "upgrade", jobj_oprintf("%s", 
                            "info", "needn't upgrade: "
                                    "not at force mode and "
                                    "target rootfs version==upgrade version"));

        os_println("needn't upgrade: not at force mode, "
            "target rootfs%d version==upgrade version(%s)",
            idx, sys.env.version);

        return 0;
    }

    upgrade_init(idx, version);

    if (idx==src) {
        err = rsync(idx, version);
    } else {
        /*
        * NOW, NOT goto here
        */
        err = rcopy(idx, dir_rootfs(src), version);
    }
    if (err<0) {
        return err;
    }

    /*
    * now, rootfsX have upgrade version
    *
    * repair kernel
    */
    err = repair_kernel(idx, version);
    if (err<0) {
        return err;
    }
    
    switch_to(idx);

    return err;
}

static int
usbupgrade(void)
{
    char kernel_version[1+OS_LINE_LEN] = {0};
    char rootfs_version[1+OS_LINE_LEN] = {0};
    benv_version_t version;
    int i, err, begin, end; 
    bool kernel_exist = false;
    
    if (0==sys.current) {
        return -ENOSUPPORT;
    }
    
    if (access(USB_FILE_KERNEL_VERSION, 0) || 
        access(USB_FILE_ROOTFS_VERSION, 0)) {
        debug_trace("no found version file, needn't usbupgrade");

        return 0;
    }
    
    err = os_fgets(kernel_version, OS_LINE_LEN, USB_FILE_KERNEL_VERSION);
    if (err<0) {
        debug_error("get %s error:%d", USB_FILE_KERNEL_VERSION, err);
        
        return err;
    }
    
    err = os_fgets(rootfs_version, OS_LINE_LEN, USB_FILE_ROOTFS_VERSION);
    if (err<0) {
        debug_error("get %s error:%d", USB_FILE_ROOTFS_VERSION, err);
        
        return err;
    }
    
    err = __benv_version_atoi(&version, rootfs_version);
    if (err<0) {
        debug_error("bad rootfs version:%s", rootfs_version);
        
        return err;
    }
    
    if (benv_version_eq(benv_rootfs_version(0), &version) && NULL==sys.env.force) {
        debug_trace("usb(%s)==rootfs0, needn't ubsupgrade", rootfs_version);
        
        return 0;
    }

    if (0==access(USB_BIN_MD_BOOT, 0)) {
        bdd("boot", PRODUCT_DEV_BOOT, USB_BIN_MD_BOOT);

        jinfo("%s", "usbupgrade", "boot");
    }

    if (0==access(USB_BIN_MD_BOOTENV, 0)) {
        bdd("bootenv", PRODUCT_DEV_BOOTENV, USB_BIN_MD_BOOTENV);

        jinfo("%s", "usbupgrade", "bootenv");
    }

    if (0==access(USB_BIN_MD_KERNEL, 0)) {
        kernel_exist = true;
    }

    get_rootfs_zone(begin, end);
    
    for (i=begin; i<end; i++) {
        if (i!=sys.current) {
            if (kernel_exist) {
                __benv_version_atoi(&version, kernel_version);
                err = kdd_byfile(i, USB_BIN_MD_KERNEL, &version);
                
                jinfo("%s%d%d",
                    "usbupgrade", "kernel",
                    "index", i,
                    "error", err);
                
                if (err<0) {
                    return err;
                }
            }
            
            __benv_version_atoi(&version, rootfs_version);
            err = rcopy(i, DIR_USB_ROOTFS, &version);
            
            jinfo("%s%d%d",
                "usbupgrade", "rootfs",
                "index", i,
                "error", err);
                    
            if (err<0) {
                return err;
            }
        }
    }
    
    for (i=0; i<2; i++) {
        if (0==access(DIR_USB_CONFIG, 0)) {
            err = __xcopy(dir_tool(i), DIR_USB_CONFIG);

            jinfo("%s%d%d",
                "usbupgrade", "config",
                "index", i,
                "error", err);
            
            if (err<0) {
                return err;
            }
        }
        
        if (0==access(DIR_USB_TOOL, 0)) {
            err = __xcopy(dir_tool(i), DIR_USB_TOOL);
            
            jinfo("%s%d%d",
                "usbupgrade", "tool",
                "index", i,
                "error", err);
                
            if (err<0) {
                return err;
            }
        }
        
        if (0==access(DIR_USB_DATA, 0)) {
            err = __xcopy(dir_tool(i), DIR_USB_DATA);
            
            jinfo("%s%d%d",
                "usbupgrade", "data",
                "index", i,
                "error", err);
                
            if (err<0) {
                return err;
            }
        }
    }
    
    return 0;
}

/*
* just repair one firmware with the best rootfs
*/
static int
super_startup(void)
{
    int skips = os_bit(0);
    int best, idx;
    
    os_println("super startup");
    jcrit("%s%d", "superstartup", "begin");
        
    /*
    * 1. find best rootfs
    */
    best = benv_find_best(rootfs, skips);
    
    if (is_benv_bad_rootfs(best)) {
        /*
        * no found good rootfs, 
        *   use rootfs0 repair rootfs1
        *   force rootfs1 as best
        */
        best = 1;

        jcrit("%s",
            "superstartup", "no found good rootfs, use rootfs0 repair rootfs1");

        rdd(1, 0);
    }
    idx = best;

    /*
    * 2. check kernel
    */
    if (is_benv_good_kernel(idx)) {
        jwarning("%s%d",
            "superstartup", "firmware is good, why in super ???",
            "firmware", idx);

        goto ok;
    }

    /*
    * 3. repair kernel
    */
    best = benv_find_best(kernel, skips);
    if (is_benv_bad_kernel(best)) {
        jcrit("%s%d",
            "superstartup", "no found good kernel, use kernel0 repair",
            "kernel", idx);
            
        best = 0;
    }
    kdd_bydev(idx, best);
    
    jcrit("%s", "superstartup", "end");

    /*
    * switch and reboot
    */
ok:
    switch_to(idx);
    
    return os_p_system("(sleep 5; sysreboot) &");
}

/*
* firmware idx is normal startup ok
*   update OTHER ok
*/
#define __update_other_ok(_obj, _idx, _skips)  do{  \
    int i;                                          \
                                                    \
    os_firmware_foreach(i) {                        \
        if (false==is_benv_skip(_skips, i)          \
            && is_benv_good_obj(_obj, i)            \
            && benv_obj_version_eq(_obj, i, _idx)){ \
            debug_ok("tagged " #_obj "%d's other is ok", i); \
            benv_obj(_obj, i)->other = BENV_FSM_OK; \
        }                                           \
    }                                               \
}while(0)

#define __normal_startup(_obj)                  do{ \
    int current = sys.current;                      \
                                                    \
    if (BENV_FSM_UNKNOW==benv_obj(_obj, current)->self) { \
        int skips = benv_skips(current);            \
        __update_other_ok(_obj, current, skips);    \
    }                                               \
                                                    \
    benv_obj(_obj, current)->error  = 0;            \
    benv_obj(_obj, current)->self   = BENV_FSM_OK;  \
                                                    \
    sys.dirty = true;                               \
}while(0)

#define __fail_startup(_obj)                    do{ \
    int current = sys.current;                      \
                                                    \
    benv_obj(_obj, current)->error  = 3;            \
    benv_obj(_obj, current)->self   = BENV_FSM_UNKNOW; \
                                                    \
    sys.dirty = true;                               \
}while(0)

static int
normal_startup(void)
{
    __normal_startup(rootfs);
    __normal_startup(kernel);

    save();
    
    os_println("normal startup");
    jinfo("%s", "startup", "normal");
    
    return 0;
}

static int
fail_startup(void)
{
    __fail_startup(rootfs);
    __fail_startup(kernel);

    save();
    
    os_println("fail startup");
    jalert("%s", "startup", "fail");
    
    return 0;
}

static int
init_env(void)
{
    char *env;
    
    env = env_gets(ENV_TIMEOUT, oem_rsync_timeout);
    if (env) {
        if (0==os_atoi(env)) {
            debug_error("bad timeout:%s", env);
            
            return -EFORMAT;
        }
        
        sys.env.timeout = env;
    }
    
    env = env_gets(ENV_PWDFILE, NULL);
    if (env) {
        sys.env.pwdfile = env;
    }

    env = env_gets(ENV_VERSION, NULL);
    if (env) {
        if (__benv_version_atoi(&sys.version, env)) {
            debug_error("bad version:%s", env);
            
            return -EFORMAT;
        }
        
        sys.env.version = env;
    }

    env = env_gets(ENV_UPGRADE, NULL);
    if (env) {
        if (0==os_atoi(env)) {
            debug_error("bad upgrade:%s", env);
            
            return -EFORMAT;
        }

        sys.env.upgrade = env;
        sys.upgrade = os_atoi(env);
    }
    
    env = env_gets(ENV_ROOTFS, NULL);
    if (env) {
        int idx = os_atoi(env);
        
        if (idx>=0 && false==is_normal_benv_idx(idx)) {
            debug_error("bad rootfs idx:%s", env);
            
            return -EFORMAT;
        }
        else if (idx==sys.current) {
            debug_error("can not upgrade current rootfs%s", env);
            
            return -EFORMAT;
        }
        
        sys.env.rootfs = env;
        sys.idx = idx;
    }

    env = env_gets(ENV_RESETBY, NULL);
    if (env) {
        int idx = os_atoi(env);
        
        if (idx>=0 && false==is_normal_benv_idx(idx)) {
            debug_error("bad resetby idx:%s", env);
            
            return -EFORMAT;
        }
        else if (idx==sys.current) {
            debug_error("can not resetby current rootfs%s", env);
            
            return -EFORMAT;
        }

        sys.env.resetby = env;
        sys.resetby = idx;
    }
    
    env = env_gets(ENV_FORCE, NULL);
    if (env) {
        sys.env.force = env;
    }
    
    env = env_gets(ENV_SERVER, oem_rsync_server);
    if (env) {
        sys.env.server = env;
    }

    env = env_gets(ENV_PORT, oem_rsync_port);
    if (env) {
        if (0==os_atoi(env)) {
            debug_error("bad port:%s", env);
            
            return -EFORMAT;
        }
        
        sys.env.port = env;
    }

    env = env_gets(ENV_USER, oem_rsync_user);
    if (env) {
        sys.env.user = env;
    }

    env = env_gets(ENV_PATH, oem_rsync_path);
    if (env) {
        sys.env.path = env;
    }

    return 0;
}

static int
get_current(void)
{
    int i, err;
    char line[1+OS_LINE_LEN] = {0};
    char *dev, *p;
    
    err = os_pgets(line, OS_LINE_LEN, "cat " PRODUCT_PROC_CMDLINE);
    if (err<0) {
        debug_error("read " PRODUCT_PROC_CMDLINE " error:%d", err);
        
        goto error;
    }

    dev = os_strstr(line, PRODUCT_PROC_CMDLINE_ROOT);
    if (NULL==dev) {
        debug_error("no found rootfs in " PRODUCT_PROC_CMDLINE);
        
        goto error;
    }
    dev += sizeof(PRODUCT_PROC_CMDLINE_ROOT) - 1;

    p = os_strchr(dev, ' ');
    if (p) {
        *p = 0;
    }
    
    os_firmware_foreach_all(i) {
        if (os_streq(dev, dev_rootfs(i))) {
            return i;
        }
    }

error:
    error_assert(0, "no found good rootfs in " PRODUCT_PROC_CMDLINE);
    jalert("%s", "getcurrent", "no found good rootfs in " PRODUCT_PROC_CMDLINE);
    
#ifdef __PC__
    return __benv_current;
#else
    return PRODUCT_FIRMWARE_CURRENT;
#endif
}

static int
init_current(void)
{
    int i, err, old = sys.current;

    err = os_pgeti(&sys.current, SCRIPT_CURRENT);
        debug_trace_error(err, SCRIPT_CURRENT);
    if (err<0) {
        sys.current = get_current();
    }

    debug_trace("current %d==>%d", old, sys.current);

    return 0;
}

static int
__fini(void)
{
    os_file_unlock();

    os_fini();

    return 0;
}

static int
__init(void)
{
    int err;
    
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);

    err = os_init();
    if (err<0) {
        return err;
    }

    err = init_current();
    if (err<0) {
        return err;
    }

    err = init_env();
    if (err<0) {
        return err;
    }

    err = os_file_lock(false);
    if (err<0) {
        return err;
    }

    err = load();
    if (err<0) {
        return err;
    }

    return 0;
}

static int
usage(void)
{
    os_eprintln(__THIS_APPNAME " usb");
    os_eprintln(__THIS_APPNAME " repair");
    os_eprintln(__THIS_APPNAME " upgrade");
    os_eprintln(__THIS_APPNAME " startup");

    return -EFORMAT;
}

static int
cmd_reset(int argc, char *argv[])
{
    int skips = __skips(sys.resetby);
    int i, err, first=0;
    
    if (0==sys.current) {
        return -ENOSUPPORT;
    }

    for (i=1; i<PRODUCT_FIRMWARE_COUNT; i++) {
        if (false==is_benv_skip(skips, i)) {
            err = reset_rootfs(i, sys.resetby);
            if (0==err) {
                reset_kernel(i);
            }

            if (!first) {
                first = i;
            }
        }
	}

    if (first) {
        switch_to(first);

        /*
        * set current failed, wait to repair by other
        */
        upgrade_init(sys.current, benv_rootfs_version(sys.resetby));

        return os_p_system("sysreboot &");
    }
    
	return 0;
}

static int
cmd_repair(int argc, char *argv[])
{
    int skips = __skips(0);
    int i, err, begin, end;

    if (0==sys.current) {
        return -ENOSUPPORT;
    }
    else if (false==__is_readonly_default()) {
        jinfo("%s%s%s",
            "rootfs", "writable",
            "repair", "false");

        return 0;
    }

    get_rootfs_zone(begin, end);

    for (i=begin; i<end; i++) {
        if (false==is_benv_skip(skips, i)) {
            repair_rootfs(i);
        }
	}

	return 0;
}


static int
cmd_upgrade(int argc, char *argv[])
{
    int i, idx = -ENOEXIST, err = 0;
    
    if (0==sys.current) {
        jinfo("%o",
            "upgrade", jobj_oprintf("%s", 
                            "info", "rootfs0 not support upgrade"));

        return -ENOSUPPORT;
    }
    else if (NULL==sys.env.version) {
        jinfo("%o",
            "upgrade", jobj_oprintf("%s", 
                            "info", "not input upgrade version"));

        return -EFORMAT;
    }
    
    if (sys.env.rootfs) {
        /*
        * select upgrade rootfs idx
        */
        idx = sys.idx;
        debug_trace("upgrade rootfs%d by env select", idx);

        if (idx<0) {
            /*
            * force upgrade all
            */
            for (i=1; i<PRODUCT_FIRMWARE_COUNT; i++) {
                if (i!=sys.current) {
                    err = upgrade(i, i);
                    if (err<0) {
                        return err;
                    }
                }
            }
        } else {
            err = upgrade(idx, idx);
        }
    }
    else {
        /*
        * NOT select rootfs idx, auto select
        */
        int skips = __skips(0);
        for (i=0; i<sys.upgrade; i++) {
            idx = get_upgrade(skips);
            debug_trace("upgrade rootfs%d by auto select", idx);

            err = upgrade(idx, idx);
            if (err<0) {
                return err;
            }

            skips |= os_bit(idx);
        }
    }
    
    return err;
}

static int
cmd_startup(int argc, char *argv[])
{
    if (0==sys.current) {
        return super_startup();
    } else {
        return normal_startup();
    }
}

static int
cmd_startfail(int argc, char *argv[])
{
    if (0==sys.current) {
        return super_startup();
    } else {
        return fail_startup();
    }
}

static int
cmd_usbupgrade(int argc, char *argv[])
{
    int err;

    __os_system(DIR_USB_ROOTFS SCRIPT_USBUPGRADE_INIT "&");
    err = usbupgrade();
    if (0==err) {
        __os_system(DIR_USB_ROOTFS SCRIPT_USBUPGRADE_OK "&");
    } else {
        __os_system(DIR_USB_ROOTFS SCRIPT_USBUPGRADE_FAIL "&");
    }

    return err;
}

static int
cmd_mount(int argc, char *argv[])
{
    int i, err, errs = 0;
    
    for (i=0; i<os_count_of(mounts); i++) {
        err = (*mounts[i].mount)();
        if (err<0) {
            errs = err;
        }
    }

    return errs;
}

static int
cmd_umount(int argc, char *argv[])
{
    int i, err, errs = 0;
    
    for (i=os_count_of(mounts) - 1; i>=0; i--) {
        err = (*mounts[i].umount)();
        if (err<0) {
            errs = err;
        }
    }
    
    return errs;
}

static cmd_table_t cmd[] = {
    CMD_TABLE_ENTRY(cmd_mount,      1, "mount"),
    CMD_TABLE_ENTRY(cmd_umount,     1, "umount"),
    CMD_TABLE_ENTRY(cmd_startup,    1, "startup"),
    CMD_TABLE_ENTRY(cmd_startfail,  1, "startfail"),
    CMD_TABLE_ENTRY(cmd_usbupgrade, 1, "usbupgrade"),
    CMD_TABLE_ENTRY(cmd_upgrade,    1, "upgrade"),
    CMD_TABLE_ENTRY(cmd_repair,     1, "repair"),
    CMD_TABLE_ENTRY(cmd_reset,      1, "reset"),
};

/*
* otp have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    return cmd_handle(cmd, argc, argv, usage);
}

#ifndef __BUSYBOX__
#define sys_main  main
#endif

/*
* otp have enabled when boot
*/
int sys_main(int argc, char *argv[])
{
    return os_call(__init, __fini, __main, argc, argv);
}

#ifdef __BUSYBOX__
int sysmount_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_mount, argc, argv);
}

int sysreset_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_reset, argc, argv);
}

int sysrepair_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_repair, argc, argv);
}

int sysstartup_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_startup, argc, argv);
}

int sysumount_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_umount, argc, argv);
}

int sysupgrade_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_upgrade, argc, argv);
}

int sysusbupgrade_main(int argc, char *argv[])
{
    return os_call(__init, __fini, cmd_usbupgrade, argc, argv);
}

#endif
/******************************************************************************/
