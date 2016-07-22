/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#if IS_PRODUCT_LTEFI_MD

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

#ifndef DIAL_TRYS
#define DIAL_TRYS   3
#endif

struct dial_table {
    char *name;
    int (*handle)(void);
};

static int dial_auto(void);

static struct {
    int interval;   // second
    int trys;
    int status;
    bool showstatus;
    HI_3G_CARD_S card;
    env_cache_t  env;
} dial = {
    .trys       = DIAL_TRYS,
    .status     = -1,
    .showstatus = true,
};

static int
__dial_showstatus(int status)
{
    static char* list[] = hisi_3g_status_list;

    if (is_good_hisi_3g_status(status)) {
        os_println("%s", list[status]);
    } else {
        os_println(__unknow);
    }
	
    return 0;
}

static int
__dial_showop(HI_3G_OPERATOR_S *op, int count)
{
    int i;
    
	for (i = 0; i < count; i++)
	{
	    os_println("operator[%d]:", i);
	    os_println(__tab "netmode:%s",  op[i].anetmode);
	    os_println(__tab "long   :%s",  op[i].alongoperator);
	    os_println(__tab "short  :%s",  op[i].ashortoperator);
	    os_println(__tab "numeric:%s",  op[i].anumericoperator);
	    os_println(__tab "rat    :%s",  op[i].arat);
	}
	
    return 0;
}

static int dial_scan(void)
{
    int card_num_allways_1;
    int err;
    
    err = hisi_3g_scan(&dial.card, 1, &card_num_allways_1);
        debug_ok_error(err, "3g scan");
    
    return err;
}

static int dial_init(void)
{
    int err;
    
    err = hisi_3g_init(&dial.card);
        debug_ok_error(err, "3g init");

    return err;
}

static int dial_getstatus(void)
{
    int err;

    err = hisi_3g_get_status(&dial.card, &dial.status);
    if (err<0) {
        debug_error("3g get status error:%d", err);
    }

    if (dial.showstatus) {
        __dial_showstatus(dial.status);
    }
    
    return err;
}

static int dial_judgestatus(void)
{
    if (HI_3G_CARD_STATUS_DISCONNECTED!=dial.status) {
        debug_error("3g status[%d] not ready", dial.status);
        
        return -ENOREADY;
    }
    
    return 0;
}

static int
dial_getallop(void)
{
    HI_3G_OPERATOR_S op[MAX_OPERATOR_NUM];
    int op_num = 0;
        
    int err = hisi_3g_get_all_operators(&dial.card, op, MAX_OPERATOR_NUM, &op_num);
    if (err<0) {
        debug_error("3g get all operators error:%d", err);
        
        return 0;
    }

	__dial_showop(op, op_num);

    return 0;
}

static int
dial_getcurop(void)
{
    HI_3G_OPERATOR_S op;
        
    int err = hisi_3g_get_current_operator(&dial.card, &op);
    if (err<0) {
        debug_error("3g get current operators error:%d", err);
        
        return 0;
    }
	
	__dial_showop(&op, 1);
	
    return 0;
}

static int
dial_setapn(void)
{
    int err;
    
    if (NULL==dial.env.apn) {
        return -EFORMAT;
    }
    
    err = hisi_3g_set_apn(&dial.card, dial.env.apn);
        debug_ok_error(err, "3g setapn");

    return err;
}

static int
dial_getapn(void)
{
    HI_3G_PDP_S pdp;
    int err;
        
    err = hisi_3g_get_apn(&dial.card, &pdp);
    if (err<0) {
        debug_error("3g get apn error:%d", err);
        
        return err;
    }

    os_println("cid   : %s", pdp.acid);
    os_println("type  : %s", pdp.apdptype);
    os_println("apn   : %s", pdp.aapn);
    os_println("ipaddr: %s", pdp.apdpipaddr);
    os_println("d_comp: %s", pdp.adcomp);
    os_println("h_comp: %s", pdp.ahcomp);

    return 0;
}

static int
dial_getqua(void)
{
    int err, rssi = -1, ber = -1;
    
    err = hisi_3g_get_quality(&dial.card, &rssi, &ber);
    if (err<0) {
        debug_error("3g get qua error:%d", err);
        
        return err;
    }

    os_println("rssi:%d", rssi);
    os_println("ber :%d", ber);

    return 0;
}

static int
dial_connect(void)
{
    int i, err = 0;

    /*
    * user & password CAN empty
    */
    if (NULL==dial.env.telephone) {
        return -EFORMAT;
    }

    for (i=0; i<dial.trys; i++) {
        err = hisi_3g_connect(&dial.card, dial.env.user, dial.env.password, dial.env.telephone, 0, NULL);
            debug_ok_error(err, "3g connect");
        if (0==err) {
            return 0;
        }

        if (dial.interval) {
            sleep(dial.interval);
        }
    }
    
    return err;
}

static struct dial_table dial_CMD[DIAL_CMD_END] = {
    [DIAL_CMD_SCAN]         = {.name = "scan",      dial_scan},
    [DIAL_CMD_INIT]         = {.name = "init",      dial_init},
    [DIAL_CMD_GETSTATUS]    = {.name = "getstatus", dial_getstatus},
    [DIAL_CMD_JUDGESTATUS]  = {.name = "judgestatus",   dial_judgestatus},
    [DIAL_CMD_SETAPN]       = {.name = "setapn",    dial_setapn},
    [DIAL_CMD_CONNECT]      = {.name = "connect",   dial_connect},
    [DIAL_CMD_GETALLOP]     = {.name = "getallop",  dial_getallop},
    [DIAL_CMD_GETCUROP]     = {.name = "getcurop",  dial_getcurop},
    [DIAL_CMD_GETAPN]       = {.name = "getapn",    dial_getapn},
    [DIAL_CMD_GETQUA]       = {.name = "getqua",    dial_getqua},
    [DIAL_CMD_AUTO]         = {.name = "auto",      dial_auto},
};

static struct dial_table *
__dial_table(char *name)
{
    int i;

    for (i=0; i<os_count_of(dial_CMD); i++) {
        if (os_streq(name, dial_CMD[i].name)) {
            return &dial_CMD[i];
        }
    }

    return NULL;
}

static int
dial_auto(void)
{
    int i, err;

    dial.showstatus = false;
    
    for (i=DIAL_CMD_AUTO_BEGIN; i<DIAL_CMD_AUTO_END; i++) {
        err = (*dial_CMD[i].handle)();
        if (err<0) {
            return err;
        }
    }

    return 0;
}

static int
cmd_dial_auto(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    return dial_auto();
}

static int
cmd_dial_test(int argc, char *argv[])
{
    char *list = os_strdup(argv[1]);
    char *name;
    struct dial_table *table;
    int err;
    
    (void)argc;
    
    os_strtok_foreach(name, list, ",") {
        table = __dial_table(name);
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
dial_usage(void)
{
#define USAGE_MODULE    __THIS_APPNAME " dial "
    os_eprintln(USAGE_MODULE "test [test-command-list]");
    os_eprintln(__tab " test-command:scan,init,getstatus,judgestatus,setapn,connect,getallop,getcurop,getapn,getqua,auto");

    os_eprintln(USAGE_MODULE "auto");
    os_eprintln(__tab " auto==test-command: scan,init,getstatus,judgestatus,setapn,connect");
#undef USAGE_MODULE

    return -EFORMAT;
}

static cmd_table_t cmd_dial[] = {
    CMD_TABLE_ENTRY(cmd_dial_test, 2, "test", NULL),
    CMD_TABLE_ENTRY(cmd_dial_auto, 1, "auto"),
};

static int
init_dial_env(void)
{
    dial.env.user = env_gets(ENV_USER, NULL);
    dial.env.password = env_gets(ENV_PASSWORD, NULL);
    dial.env.telephone = env_gets(ENV_TELEPHONE, NULL);
    dial.env.apn = env_gets(ENV_APN, NULL);
    
    dial.env.try = env_gets(ENV_TRY, NULL);
    if (dial.env.try) {
        int trys = os_atoi(dial.env.try);
        if (trys > 0) {
            dial.trys = trys;
        }
    }

    dial.env.interval = env_gets(ENV_INTERVAL, NULL);
    if (dial.env.interval) {
        int interval = os_atoi(dial.env.interval);
        if (interval > 0) {
            dial.interval = interval;
        }
    }

    return 0;
}

static int
dial_main(int argc, char *argv[])
{
    init_dial_env();

    return cmd_handle(cmd_dial, argc, argv, dial_usage);
}
#endif
/******************************************************************************/
