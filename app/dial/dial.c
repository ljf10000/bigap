/*******************************************************************************
Copyright (c) 2016-2018, Supper Wali Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      dial
#endif

#include "utils.h"

OS_INITER;

static char* STATUSLIST[] = hisi_3g_status_list;
static int STATUS = -1;
static HI_3G_CARD_S CARD;
static env_cache_t ENV;

static int
showop(HI_3G_OPERATOR_S *op, int count)
{
    int i;
    
	for (i = 0; i < count; i++)
	{
	    os_println("operator[%d]:", i);
	    os_println(__tab "netmode:%s", op[i].anetmode);
	    os_println(__tab "long:%s", op[i].alongoperator);
	    os_println(__tab "short:%s", op[i].ashortoperator);
	    os_println(__tab "numeric:%s", op[i].anumericoperator);
	    os_println(__tab "rat:%s", op[i].arat);
	}
	
    return 0;
}

static int __scan(void)
{
    int card_num_allways_1;
        
    return hisi_3g_scan(&CARD, 1, &card_num_allways_1);
}

static int __init(void)
{
    return hisi_3g_init(&CARD);
}

static int __getstatus(void)
{
    int err;

    err = hisi_3g_get_status(&CARD, &STATUS);
    if (is_good_hisi_3g_status(STATUS)) {
        os_println("%s", STATUSLIST[STATUS]);
    } else {
        os_println(__unknow);
    }
    
    return err;
}

static int __judgestatus(void)
{
    if (HI_3G_CARD_STATUS_DISCONNECTED!=STATUS) {
        return -ENOREADY;
    }
    
    return 0;
}

static int
__getallop(void)
{
    HI_3G_OPERATOR_S op[MAX_OPERATOR_NUM];
    int op_num = 0;
        
    int err = hisi_3g_get_all_operators(&CARD, op, MAX_OPERATOR_NUM, &op_num);
    if (err<0) {
        return 0;
    }

	showop(op, op_num);
	
    return 0;
}

static int
__getcurop(void)
{
    HI_3G_OPERATOR_S op;
        
    int err = hisi_3g_get_current_operator(&CARD, op);
    if (err<0) {
        return 0;
    }
	
	showop(&op, 1);
	
    return 0;
}

static int
__setapn(void)
{
    if (NULL==ENV.apn) {
        return -EFORMAT;
    }
    
    return hisi_3g_set_apn(&CARD, ENV.apn);
}

static int
__getapn(void)
{
    HI_3G_PDP_S pdp;
    int err;
        
    err = hisi_3g_get_apn(&CARD, &pdp);
    if (err<0) {
        return err;
    }

    os_println("cid:    %s", pdp.acid);
    os_println("type:   %s", pdp.apdptype);
    os_println("apn:    %s", pdp.aapn);
    os_println("ipaddr: %s", pdp.apdpipaddr);
    os_println("d_comp: %s", pdp.adcomp);
    os_println("h_comp: %s", pdp.ahcomp);

    return 0;
}

static int
__getqua(void)
{
    int err, rssi = -1, ber = -1;
    
    err = hisi_3g_get_quality(&CARD, &rssi, &ber);
    if (err<0) {
        return err;
    }

    os_println("rssi:%d", rssi);
    os_println("ber :%d", ber);

    return 0;
}

static int
__connect(void)
{
    int err;
    
    if (NULL==ENV.username ||
        NULL==ENV.password ||
        NULL==ENV.telephone ||
        NULL==ENV.apn) {
        return -EFORMAT;
    }
    
    err = hisi_3g_connect(&CARD, ENV.username, ENV.password, ENV.telephone, 0, NULL);
    if (err<0) {
        return err;
    }
    
    return 0;
}

enum {
    DIAL_CMD_AUTO_BEGIN = 0,
    DIAL_CMD_SCAN = DIAL_CMD_AUTO_BEGIN,
    DIAL_CMD_INIT,
    DIAL_CMD_GETSTATUS,
    DIAL_CMD_JUDGESTATUS,
    DIAL_CMD_SETAPN,
    DIAL_CMD_CONNECT,
    DIAL_CMD_AUTO_END,
    
    DIAL_CMD_GETALLOP = DIAL_CMD_AUTO_END,
    DIAL_CMD_GETCUROP,
    DIAL_CMD_GETAPN,
    DIAL_CMD_GETQUA,
    DIAL_CMD_AUTO,

    DIAL_CMD_END
};

struct dial_table {
    char *name;
    int (*handle)(void);
}

static int __auto(void);

static struct dial_table CMD[DIAL_CMD_END] = {
    [DIAL_CMD_SCAN]         = {.name = "scan",      __scan},
    [DIAL_CMD_INIT]         = {.name = "init",      __init},
    [DIAL_CMD_GETSTATUS]    = {.name = "getstatus", __getstatus},
    [DIAL_CMD_JUDGESTATUS]  = {.name = "judgestatus",   __judgestatus},
    [DIAL_CMD_SETAPN]       = {.name = "setapn",    __setapn},
    [DIAL_CMD_CONNECT]      = {.name = "connect",   __connect},
    [DIAL_CMD_GETALLOP]     = {.name = "getallop",  __getallop},
    [DIAL_CMD_GETCUROP]     = {.name = "getcurop",  __getcurop},
    [DIAL_CMD_GETAPN]       = {.name = "getapn",    __getapn},
    [DIAL_CMD_GETQUA]       = {.name = "getqua",    __getqua},
    [DIAL_CMD_AUTO]         = {.name = "auto",      __auto},
};

static struct dial_table *
__get_table(char *name)
{
    int i;

    for (i=0; i<os_count_of(CMD); i++) {
        if (os_streq(name, CMD[i].name)) {
            return &CMD[i];
        }
    }

    return NULL;
}

static int
__auto(void)
{
    int i, err;

    for (i=DIAL_CMD_AUTO_BEGIN; i<DIAL_CMD_AUTO_END; i++) {
        err = (*CMD[i].handle)();
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static int
cmd_auto(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return __auto();
}

static int
cmd_test(int argc, char *argv[])
{
    char *list = os_strdup(argv[1]);
    char *name;
    struct dial_table *table;
    int err;
    
    (void)argc;
    
    os_strtok_foreach(name, list, ",") {
        table = __get_table(name);
        if (NULL==table) {
            os_println("unknow command:%s", name);

            return -EFORMAT;
        }

        err = (*table->handle)();
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static int
usage(void)
{
    os_eprintln(__THIS_APPNAME " test [test-command-list]");
    os_eprintln(__tab " test-command:scan,init,getstatus,judgestatus,setapn,connect,getallop,getcurop,getapn,getqua,auto");

    os_eprintln(__THIS_APPNAME " auto");
    os_eprintln(__tab " auto==test-command: scan,init,getstatus,judgestatus,setapn,connect");

    return -EFORMAT;
}

static cmd_table_t cmd[] = {
    CMD_TABLE_ENTRY(cmd_test, 2, "test", NULL),
    CMD_TABLE_ENTRY(cmd_auto, 1, "auto"),
};

static int
init_env(void)
{
    ENV.user = env_gets(ENV_USER, NULL);
    ENV.password = env_gets(ENV_PASSWORD, NULL);
    ENV.telephone = env_gets(ENV_TELEPHONE, NULL);
    ENV.apn = env_gets(ENV_APN, NULL);

    return 0;
}

/*
* otp have enabled when boot
*/
static int
__main(int argc, char *argv[])
{
    init_env();
    
    return cmd_handle(cmd, argc, argv, usage);
}

#ifndef __BUSYBOX__
#define dial_main  main
#endif

/*
* otp have enabled when boot
*/
int dial_main(int argc, char *argv[])
{
    return os_call(hisi_init, hisi_fini, __main, argc, argv);
}
/******************************************************************************/
