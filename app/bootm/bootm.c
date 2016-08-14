
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bootm.h"

#define BOOTENV_MMCBLK          "/dev/mmcblk0p2"

static int
env_common_check(char *value, char *array[], int count)
{
    int i;

    for (i=0; i<count; i++) {
        if (0==strcmp(array[i], value)) {
            return 0;
        }
    }

    return -1;
}

static int
env_rootfs_check(char *value)
{
    static char *array[] = { "0", "1", "2" };

    return env_common_check(value, array, countof_array(array));
}

static int
env_rootfsx_check(char *value)
{
    static char *array[] = { "o", "v", "f" };

    return env_common_check(value, array, countof_array(array));
}

static int
env_rootfsxerr_check(char *value)
{
    static char *array[] = { "0", "1", "2", "3" };

    return env_common_check(value, array, countof_array(array));
}

enum {
    BOOT_VER_COUNT  = 2,
    BOOT_VER_MIN    = 0,
    BOOT_VER_MAX    = 9999,
};

static int
env_bootver_check(char *value)
{
    int err = 0;
    int ver[BOOT_VER_COUNT];
    int i;
    
    err = sscanf(value, "%d.%d", &ver[0], &ver[1]);
    if (err<0) {
        return -errno;
    }

    for (i=0; i<BOOT_VER_COUNT; i++) {
        if (ver[i] < BOOT_VER_MIN || ver[i] > BOOT_VER_MAX) {
            return -1;
        }
    }
    
    return 0;
}

enum {
    ENV_F_CHANGED     = 0x01,
    ENV_F_HIDDEN      = 0x02,
    ENV_F_READONLY    = 0x04,
};

#define ENV_INITER(_name, _flag, _check) { \
    .name = _name,      \
    .flag = _flag,      \
    .check = _check,    \
}

#define ENV(_idx, _name, _flag, _check) \
    [_idx] = ENV_INITER(_name, _flag, _check)

#define ENV_PRIVATE(_idx) \
    ENV(_idx, AT_NAME_PRIVATE(_idx), ENV_F_HIDDEN, NULL)

static struct {
    char **name;
    int flag;
    int (*check)(char *value);
} envctl[AT_ENV_COUNT] = {
    [0 ... (AT_ENV_COUNT-1)] = ENV_INITER("", ENV_F_HIDDEN, NULL),
    
    ENV(AT_ENV_INIT,    AT_NAME_INIT,       ENV_F_HIDDEN | ENV_F_READONLY,  NULL),
    ENV(AT_ENV_BOOTVER, AT_NAME_BOOTVER,    ENV_F_HIDDEN | ENV_F_READONLY,  NULL),
    ENV(AT_ENV_PSN,     AT_NAME_PSN,        ENV_F_HIDDEN | ENV_F_READONLY,  NULL),
    ENV(AT_ENV_MID,     AT_NAME_MID,        ENV_F_HIDDEN | ENV_F_READONLY,  NULL),

    ENV(AT_ENV_RT,              AT_NAME_RT,                 ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_UT,              AT_NAME_UT,                 ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_NA,              AT_NAME_NA,                 ENV_F_HIDDEN, NULL),
    
    ENV(AT_ENV_PCBA_VENDOR,     AT_NAME_PCBA_VENDOR,        ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PCBA_COMPANY,    AT_NAME_PCBA_COMPANY,       ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PCBA_MODEL,      AT_NAME_PCBA_MODEL,         ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PCBA_MAC,        AT_NAME_PCBA_MAC,           0, NULL),
    ENV(AT_ENV_PCBA_SN,         AT_NAME_PCBA_SN,            0, NULL),
    ENV(AT_ENV_PCBA_VERSION,    AT_NAME_PCBA_VERSION,       ENV_F_HIDDEN, NULL),
    
    ENV(AT_ENV_PRODUCT_VENDOR,  AT_NAME_PRODUCT_VENDOR,     ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_COMPANY, AT_NAME_PRODUCT_COMPANY,    ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_MODEL,   AT_NAME_PRODUCT_MODEL,      ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_MAC,     AT_NAME_PRODUCT_MAC,        ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_SN,      AT_NAME_PRODUCT_SN,         ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_VERSION, AT_NAME_PRODUCT_VERSION,    ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_PRODUCT_MANAGER, AT_NAME_PRODUCT_MANAGER,    ENV_F_HIDDEN, NULL),
    
    ENV(AT_ENV_OEM_VENDOR,      AT_NAME_OEM_VENDOR,         ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_COMPANY,     AT_NAME_OEM_COMPANY,        ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_MODEL,       AT_NAME_OEM_MODEL,          ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_MAC,         AT_NAME_OEM_MAC,            ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_SN,          AT_NAME_OEM_SN,             ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_VERSION,     AT_NAME_OEM_VERSION,        ENV_F_HIDDEN, NULL),
    ENV(AT_ENV_OEM_MANAGER,     AT_NAME_OEM_MANAGER,        ENV_F_HIDDEN, NULL),
    
    ENV(AT_ENV_ROOTFS,      AT_NAME_ROOTFS,     0,  env_rootfs_check),
    ENV(AT_ENV_ROOTFS1,     AT_NAME_ROOTFS1,    0,  env_rootfsx_check),
    ENV(AT_ENV_ROOTFS2,     AT_NAME_ROOTFS2,    0,  env_rootfsx_check),
    ENV(AT_ENV_ROOTFS1ERR,  AT_NAME_ROOTFS1ERR, 0,  env_rootfsxerr_check),
    ENV(AT_ENV_ROOTFS2ERR,  AT_NAME_ROOTFS2ERR, 0,  env_rootfsxerr_check),

    ENV_PRIVATE(40),
    ENV_PRIVATE(41),
    ENV_PRIVATE(42),
    ENV_PRIVATE(43),
    ENV_PRIVATE(44),
    ENV_PRIVATE(45),
    ENV_PRIVATE(46),
    ENV_PRIVATE(47),
    ENV_PRIVATE(48),
    ENV_PRIVATE(49),
    
    ENV_PRIVATE(50),
    ENV_PRIVATE(51),
    ENV_PRIVATE(52),
    ENV_PRIVATE(53),
    ENV_PRIVATE(54),
    ENV_PRIVATE(55),
    ENV_PRIVATE(56),
    ENV_PRIVATE(57),
    ENV_PRIVATE(58),
    ENV_PRIVATE(59),
    
    ENV_PRIVATE(60),
    ENV_PRIVATE(61),
    ENV_PRIVATE(62),
    ENV_PRIVATE(63),
    ENV_PRIVATE(64),
    ENV_PRIVATE(65),
    ENV_PRIVATE(66),
    ENV_PRIVATE(67),
    ENV_PRIVATE(68),
    ENV_PRIVATE(69),
    
    ENV_PRIVATE(70),
    ENV_PRIVATE(71),
    ENV_PRIVATE(72),
    ENV_PRIVATE(73),
    ENV_PRIVATE(74),
    ENV_PRIVATE(75),
    ENV_PRIVATE(76),
    ENV_PRIVATE(77),
    ENV_PRIVATE(78),
    ENV_PRIVATE(79),
    
    ENV_PRIVATE(80),
    ENV_PRIVATE(81),
    ENV_PRIVATE(82),
    ENV_PRIVATE(83),
    ENV_PRIVATE(84),
    ENV_PRIVATE(85),
    ENV_PRIVATE(86),
    ENV_PRIVATE(87),
    ENV_PRIVATE(88),
    ENV_PRIVATE(89),
    
    ENV_PRIVATE(90),
    ENV_PRIVATE(91),
    ENV_PRIVATE(92),
    ENV_PRIVATE(93),
    ENV_PRIVATE(94),
    ENV_PRIVATE(95),
    ENV_PRIVATE(96),
    ENV_PRIVATE(97),
    ENV_PRIVATE(98),
    ENV_PRIVATE(99),
};

static int
env_check(int idx, char *value)
{
    if (NULL==envctl[idx].check || 0==(*envctl[idx].check)(value)) {
        return 0;
    } else {
        return -1;
    }
}

static char bootenv[AT_ENV_COUNT][AT_ENV_LINE_SIZE];

enum {
    BLKSIZE     = 512,
    MULTIPLE    = BLKSIZE/AT_ENV_LINE_SIZE,
};

static int
env_write(STREAM f)
{
    int i, k, idx, ret = 0;

    for (i=0; i<AT_ENV_COUNT; i++) {
        if (ENV_F_CHANGED & envctl[i].flag) {
            int err = fseek(f, AT_ENV_LINE_SIZE*i, SEEK_SET);
            if (err<0) {
                println("seek %s error(%d), skip it", envctl[i].name, -errno);

                ret = -1; continue;
            }
            
            int count = fwrite(bootenv[i], AT_ENV_LINE_SIZE, 1, f);
            if (1 != count) {
                println("write %s=%s error(%d), skip it", envctl[i].name, bootenv[i], -errno);

                ret = -1; continue;
            }
        }
    }

    return ret;
}

static int 
env_read(STREAM f)
{
    int count;
    
    count = fread((void *)bootenv, AT_ENV_LINE_SIZE, AT_ENV_COUNT, f);
    if (AT_ENV_COUNT != count) {
        println("read " BOOTENV_MMCBLK " error");

        return -1;
    }
    
    return 0;
}

void usage(int argc, char *argv[])
{
    println("%s ==> show all env", argv[0]);
    println("%s name ==> show env by name", argv[0]);
    println("%s name1=value1 name2=value2 ... ==> set env by name and value", argv[0]);
}

static int
getidx_byname(char *name)
{
    int i;

    for (i=0; i<AT_ENV_COUNT; i++) {
        char *p;

        for (p = envctl[i].name[0]; p; p++) {
            if (0==strcmp(name, p)) {
                return i;
            }
        }
    }

    return -1;
}

static char *
getvalue_byname(char *name)
{
    int idx = getidx_byname(name);

    if (idx<0) {
        return NULL;
    } else {
        return bootenv[idx];
    }
}

#ifndef __BUSYBOX__
#define bootm_main  main
#endif

int bootm_main(int argc, char *argv[])
{
    STREAM f = NULL;
    int count;
    int err = 0;
    int len;
    int idx;
    int i;
    char *name;
    char *value;
    
    f = fopen(BOOTENV_MMCBLK, "rb+");
    if (NULL==f) {
        println("can not open " BOOTENV_MMCBLK);
        
        err = -1; goto exit;
    }
    
    if (env_read(f)) {
        err = -1; goto exit;
    }
    
    /*
    * display all
    */
    if (1==argc) {
        for (i=1; i<AT_ENV_COUNT; i++) {
            if (bootenv[i][0] && !(ENV_F_HIDDEN & envctl[i].flag)) {
                println("%s=%s", envctl[i].name, bootenv[i]);
            }
        }

        err = 0; goto exit;
    }
    else if (2==argc) {
        char *name = argv[1];
        /*
        * help
        */
        if (0==strcmp("-h", name) || 0==strcmp("--help", name)) {
            usage(argc, argv);
            err = 0; goto exit;
        }
        /*
        * get by name
        */
        else if (NULL==strchr(name, '=')) {
            value = getvalue_byname(name);
            if (NULL==value) {
                println("argv[1](%s) bad name", name);

                err = -1;
            }
            else if (0==value[0]) {
                /* empty */
                err = 0;
            }
            else {
                println("%s", value);
                
                err = 0;
            }

            goto exit;
        }
    }
    
    /*
    * set by name
    */
    for (i=1; i<argc; i++) {
        char line[2*AT_ENV_LINE_SIZE] = {0};
        len = strlen(argv[i]);
        
        /*
        * check input length
        */
        if (len > (2*AT_ENV_LINE_SIZE-1)) {
            println("argv[%d](%s) too long", i, argv[i]);

            err = -1; goto exit;
        }
        
        strcpy(line, argv[i]);

        /*
        * get name
        */
        name = line;
        value = strchr(line, '=');
        if (NULL==value) {
            println("argv[%d](%s) should as xxx=xxxx", i, argv[i]);

            err = -1; goto exit;
        }
        *value = 0; value++;
        
        /*
        * check name
        */
        idx = getidx_byname(name);
        if (idx<0) {
            println("argv[%d](%s) bad name(%s)", i, argv[i], name);
            
            err = -1; goto exit;
        }
        
        /*
        * check value
        */
        if (0==value[0]) {
            bootenv[idx][0] = 0;
        }
        else if (strlen(value) > (AT_ENV_LINE_SIZE-1)) {
            println("argv[%d](%s) value length max %d", i, argv[i], (AT_ENV_LINE_SIZE-1));

            err = -1; goto exit;
        }
        else if (env_check(idx, value) < 0) {
            println("argv[%d](%s) value is invalid", i, argv[i]);

            err = -1; goto exit;
        }
        else if (ENV_F_READONLY & envctl[idx].flag) {
            println("argv[%d](%s) is readonly", i, argv[i]);

            err = -1; goto exit;
        }
        else {
            strcpy(bootenv[idx], value);
            
            envctl[idx].flag |= ENV_F_CHANGED;
        }
    }
    
    if (env_write(f)) {
        err = -1; goto exit;
    }
    
exit:
    if (f) {
        fclose(f);
    }
    
    return err;
}

