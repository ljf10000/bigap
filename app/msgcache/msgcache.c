/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      msgcache
#endif

#define __MSGCACHE__
#define __DEAMON__

#include "utils.h"

OS_INITER;

#ifndef MSG_BUFSIZE
#define MSG_BUFSIZE       (1*1024*1024-1)
#endif

// chenxf add begin
#ifndef MSG_CPATH
#define MSG_CPATH          "/home/macaronn/projD/s-wall/msgdir"
#endif

#ifndef MSGCACHE_CFG
#define MSGCACHE_CFG            MSG_CPATH "/msgcache.config"
#endif

#ifndef MSG_LOGFILE_PATH
#define MSG_LOGFILE_PATH   "/tmp/jmsg"
#endif

#ifndef MSG_REPORT_TICKS
#define MSG_REPORT_TICKS                    6000 /* ms */
#endif

#define MC_CFG_GLOBAL         "globalcfg"
#define MC_CFG_MSGPATH        "jmsg_path"

#define MC_CFG_APPS           "apps"
#define MC_CFG_APPPRIOR       "app_pri"
#define MC_CFG_ITEMAMOUNT     "msg_limit"
#define MC_CFG_TXMETHOD       "report"
#define MC_CFG_STOREINTERVAL  "store_interval"
#define MC_CFG_INSTANTPRIOR   "instant_pri"
#define MC_CFG_REPORTINTERVAL "report_interval"

#define MC_APP_HASHSIZE       1024

#define appname_to_index(app_name) os_str_bkdr(app_name)%(MC_APP_HASHSIZE-1)

typedef struct {
	uint32 store_flag;
    char *jmsg_buf;
    struct list_head jobjCache_node;
} jobj_cache_t;

typedef struct {
    char *appName;
    char *txMethod;
	
    int appPrior;
    int itemAmount;
    int storeInterval;

	int instant_pri;
	int report_interval;

	struct list_head jlogdApp_node;
    struct list_head jobjCache_list;
} jlogd_app_t;

STATIC jlogd_app_t default_cfg = {
	.appName    = "default",

	.appPrior   = 3,
	.itemAmount = 2000,
	.storeInterval = 1,

	.instant_pri = 7,
	.report_interval = 10,
};

// chenxf add end

static char msgb[1 + MSG_BUFSIZE];

typedef struct {
    char *name;
    int family;
    int fd;
    os_sockaddr_t addr;
    
    struct {
        STREAM stream;
        
        env_string_t *envar;

        uint32 count;
    } log, cache;
} jlog_server_t;

#define JLOGD_SERVER(_name, _family) { \
    .name       = _name,        \
    .family     = _family,      \
    .fd         = INVALID_FD,   \
    .addr = {                   \
        .c  = {                 \
            .sa_family=_family, \
        }                       \
    },                          \
}   /* end */

enum {
    JLOGD_USERVER,
    
    JLOGD_END_SERVER
};

/*
*  chenxf try to make it clear of how structure jlogd_t, jlogd_app_t, jobj_cache_t organize
*  
*jlogd_t 
*     |jlogd_appA|jlogd_appB|jlod_appC|  ---> organize as hash table, japps_hash
*	|--> jlogd_appA <---> jlogd_appB <---> jlogd_appC <---> jlogd_appD      ---> pointer list, app_list
* 		     |--> jobj_cacheA    |--> jobj_cacheB    ---> cache in each app
*/

typedef struct {
    
    uint64 seq;
    uint32 event;
    char mac[1+MACSTRINGLEN_L];

    loop_t loop;
    
    jlog_server_t server[JLOGD_END_SERVER];
	char *jmsg_path;

	struct list_head app_list;  /*For furture use*/
	struct list_head japps_hash[MC_APP_HASHSIZE];
} jlogd_t;

STATIC jlogd_t jlogd = {
    
    .mac    = {0},
    .loop   = LOOP_INITER,
    .server = {
        [JLOGD_USERVER] = JLOGD_SERVER("userver", AF_UNIX),
    },
    .app_list = LIST_HEAD_INIT(jlogd.app_list),
    .jmsg_path = MSG_LOGFILE_PATH,
};

#define jlogserver0         (&jlogd.server[0])
#define jlogservermax       (&jlogd.server[os_count_of(jlogd.server)])

#define foreach_server(_server) \
    for(_server=jlogserver0; _server<jlogservermax; _server++)

STATIC int
msgcache_head_jadd(jlog_server_t *server)
{
    int len = 0, pri, err;
    jobj_t obj = NULL;
    
    obj = jobj_byjson(msgb);
    if (NULL==obj) {
        len = -EBADJSON; goto error;
    }

    jobj_t header = jobj_get(obj, JLOG_KEY_HEADER);
    if (NULL==header) {
        __debug_error("no-found header");
        
        len = -EFORMAT; goto error;
    }
    
    jobj_t opri = jobj_get(header, JLOG_KEY_PRI);
    if (NULL==opri) {
        __debug_error("no-found pri");
        
        len = -EFORMAT; goto error;
    }
    pri = LOG_PRI(jobj_get_i32(opri));
    
    err = jobj_add_u64(header, JLOG_KEY_SEQ, ++jlogd.seq);
    if (err<0) {
        __debug_error("add seq error");
        
        len = err; goto error;
    }

    char *mac = os_getmacby(SCRIPT_GETBASEMAC);
    if (mac) {
        err = jobj_add_string(header, JLOG_KEY_MAC, mac);
        if (err<0) {
            __debug_error("add mac error");
            
            len = err; goto error;
        }
    }

    char *json = jobj_json(obj);
    if (NULL==json) {
        __debug_error("obj==>json failed");
        
        len = -EFORMAT; goto error;
    }
    len = os_strlen(json);
    
    os_strmcpy(msgb, json, len);
    
error:
    jobj_put(obj);
    
    return len;
}


//chenxf add begin

STATIC int 
msgcache_cache_to_file(void)
{
	jlogd_app_t  *japp = NULL;
	jobj_cache_t *jmsg_node = NULL;
	STREAM stream;
	int err;

	list_for_each_entry(japp, &jlogd.app_list, jlogdApp_node){
		list_for_each_entry(jmsg_node, &japp->jobjCache_list, jobjCache_node){
			stream = os_fopen(jlogd.jmsg_path, "a+");
		    err = os_fwrite(stream, jmsg_node->jmsg_buf, os_strlen(jmsg_node->jmsg_buf));
		}
	}
}

STATIC int 
msgcache_jmsg_node_add(char *jmsg_buf, uint32 store_flag, jlogd_app_t *japp_cur)
{	
	jobj_cache_t *jmsg_node = (jobj_cache_t *)os_malloc(sizeof(jobj_cache_t));
	jmsg_node->jmsg_buf = (char *)os_malloc(os_strlen(jmsg_buf)+1);
	jmsg_node->store_flag = store_flag;
	
	printf("start to add to jmsg cache in corresponding app\n");
	os_strcpy(jmsg_node->jmsg_buf, jmsg_buf);
	printf("##jmsg_buf: %s, length: %d\n", jmsg_buf, os_strlen(jmsg_buf));
	printf("##jmsg_node->jmsg_buf: %s\n", jmsg_node->jmsg_buf);
	list_add_tail(&jmsg_node->jobjCache_node, &japp_cur->jobjCache_list);
	printf("finish add to jmsg cache in corresponding app\n");
	return 0;
}

STATIC jlogd_app_t *
msgcache_createapp_by_name(char *app_name)
{
	uint32 index = 0;
	bool hit = false;
	jlogd_app_t *japp_cur = NULL;
	
	/*the coming jmsg is already in the hash table, or need to create one? */
	index = appname_to_index(app_name);
	if (!list_empty(&jlogd.japps_hash[index])){

		list_for_each_entry(japp_cur, &jlogd.japps_hash[index], jlogdApp_node) {
			if (0 == os_strncmp(japp_cur->appName, app_name, os_strlen(app_name))) {
				printf("jlogd_createapp_by_name: find corresponding japp in hash table: %s\n",japp_cur->appName);
				hit = true;
				break;
			}
			else
				hit = false;

		}
	}
	else
		hit = false;
	
	if (false == hit) {
		japp_cur = (jlogd_app_t *)os_malloc(sizeof(jlogd_app_t));
	    if (NULL == japp_cur) {
 	       return -ENOSPACE;
 	    }

		os_memcpy(japp_cur, &default_cfg, sizeof(jlogd_app_t));
		INIT_LIST_HEAD(&japp_cur->jobjCache_list);
    	japp_cur->appName = os_strdup(app_name);

		printf("jlogd_createapp_by_name: hit failed, create japp, add to hash table: %s\n",japp_cur->appName);

		// add to hash table
		list_add(&japp_cur->jlogdApp_node, &jlogd.japps_hash[index]);

		// add to apps list
		list_add(&japp_cur->jlogdApp_node, &jlogd.app_list);
	}

	return japp_cur;
}

STATIC int 
msgcache_createapp_by_jobj(jobj_t jobj, char *app_name)
{
	jlogd_app_t *japps = (jlogd_app_t *)os_malloc(sizeof(jlogd_app_t));
    if (NULL==japps) {
        return -ENOSPACE;
    }
	
    INIT_LIST_HEAD(&japps->jobjCache_list);

    japps->appName = os_strdup(app_name);

	jobj_t txMethod = jobj_get(jobj, MC_CFG_TXMETHOD);
	if(NULL != txMethod)
	    japps->txMethod = os_strdup(jobj_get_string(txMethod));	

	jobj_t appPrior = jobj_get(jobj, MC_CFG_APPPRIOR);
	if(NULL != appPrior)
	    japps->appPrior = jobj_get_i32(appPrior);
	else
		japps->appPrior = default_cfg.appPrior;  // using default

	jobj_t itemAmount = jobj_get(jobj, MC_CFG_ITEMAMOUNT);
	if(NULL != itemAmount)
	    japps->itemAmount = jobj_get_i32(itemAmount);
	else
		japps->itemAmount = default_cfg.itemAmount;  // using default

	jobj_t storeInterval = jobj_get(jobj, MC_CFG_STOREINTERVAL);
	if(NULL != storeInterval)
	    japps->storeInterval = jobj_get_i32(storeInterval);
	else
		japps->storeInterval = default_cfg.storeInterval;  // using default

	jobj_t instant_pri = jobj_get(jobj, MC_CFG_INSTANTPRIOR);
	if(NULL != instant_pri)
	    japps->instant_pri = jobj_get_i32(instant_pri);
	else
		japps->instant_pri = default_cfg.instant_pri;  // using default

	jobj_t report_interval = jobj_get(jobj, MC_CFG_REPORTINTERVAL);
	if(NULL != report_interval)
	    japps->report_interval = jobj_get_i32(report_interval);
	else
		japps->report_interval = default_cfg.report_interval;  // using default

    printf("japps->appName  = %s\n", japps->appName);
    printf("japps->txMethod = %s\n", japps->txMethod);
    printf("japps->appPrior = %d\n", japps->appPrior);
	printf("japps->itemAmount    = %d\n", japps->itemAmount);
	printf("japps->storeInterval = %d\n\n", japps->storeInterval);
	printf("japps->instant_pri   = %d\n", japps->instant_pri);
	printf("japps->report_interval = %d\n\n", japps->report_interval);

	printf("app name hash value: %u, hash = %u\n", os_str_bkdr(app_name), appname_to_index(app_name));

	// add to hash table
	list_add(&japps->jlogdApp_node, &jlogd.japps_hash[appname_to_index(app_name)]);

	// add app list
	list_add(&japps->jlogdApp_node, &jlogd.app_list);
	
}

STATIC int
msgcache_jobj_analysis(void)
{
	jobj_t jobj = NULL;
	jlogd_app_t *japp_cur = NULL;
	int return_val = 0;
	char *appName = NULL;
	int pri = 0, index = 0;
	bool hit = false;
	
	jobj = jobj_byjson(msgb);
	jobj_t header = jobj_get(jobj, JLOG_KEY_HEADER);
	
	if (NULL==header) {
		__debug_error("no-found header");
		return -EFORMAT;
	}
	
	jobj_t app = jobj_get(header, JLOG_KEY_APP);
	if (NULL==app) {
		__debug_error("no-found app");
		return -EFORMAT;
	}
	appName = jobj_get_string(app);

	printf("jobj_analysis: app = %s\n",jobj_get_string(app));

	jobj_t msg_pri = jobj_get(header, JLOG_KEY_PRI);
	if (NULL==msg_pri) {
		__debug_error("no-found msg_pri");
		return -EFORMAT;
	}
	pri = jobj_get_int(msg_pri);

	printf("jobj_analysis: msg_pri = %d\n", pri);
	printf("jobj_analysis: msgb len = %d\n", os_strlen(msgb));

	/*the coming jmsg is already in the hash table, or need to create one? */
	japp_cur = msgcache_createapp_by_name(appName);

	/*send to server or add it to cache*/
	if (pri <= japp_cur->appPrior)
		printf("send it to server directly\n");
	else
		msgcache_jmsg_node_add(msgb, 0, japp_cur);

	return 0;
}

//chenxf add end

STATIC int
msgcache_jhandle(jlog_server_t *server)
{
    int err, len;
    os_sockaddr_t client = OS_SOCKADDR_INITER(server->family);
    sockaddr_len_t addrlen = os_sockaddr_len(&client.c);
    
    len = __io_recvfrom(server->fd, msgb, sizeof(msgb), 0, &client.c, &addrlen);
    if (len<0) {
        __debug_error("read error:%d", len);
        return len;
    }

    if (is_abstract_sockaddr(&client.c)) {
        set_abstract_sockaddr_len(&client.un, addrlen);

        __debug_trace("recv from:%s", get_abstract_path(&client.un));
    }
    
    msgb[len] = 0;
    __debug_trace("read:%s, len:%d", msgb, len);

	printf("jlogd_jhandle before jlogd_add: msgb: %s\n",msgb);
	
    err = msgcache_head_jadd(server);

	msgcache_jobj_analysis();
	printf("jlogd_jhandle after jlogd_add: msgb: %s\n",msgb);

    if (err<0) { /* yes, <0 */
        /* log */
    } else {
        len = err;
    }

    msgb[len++] = '\n';
    msgb[len] = 0;

}

STATIC int 
msgcache_handle(loop_watcher_t *watcher, time_t now)
{
    jlog_server_t *server = (jlog_server_t *)watcher->user;

    msgcache_jhandle(server);

    return 0;
}
//chenxf add begin

STATIC int
msgcache_report_timer(loop_watcher_t *watcher, time_t now)
{
	jlogd_app_t  *japp = NULL;
	jobj_cache_t *jmsg_node = NULL;
	uint32 report_cnt = 0;

	list_for_each_entry(japp, &jlogd.app_list, jlogdApp_node){

		printf("jlog report app name: %s\n",japp->appName);
		while(!list_empty(&japp->jobjCache_list)){
			jmsg_node = list_first_entry(&japp->jobjCache_list, jobj_cache_t, jobjCache_node);

			printf("start to report\n");
			if (report_cnt <= japp->report_interval){
				/*TBD: send logic, currently print it instead */
				printf("jlogd_report_timer: send to server, report_cnt = %u, report_int = %u, appName: %s\n",\
				report_cnt, japp->report_interval, japp->appName);
				printf("jlogd_report_timer: store_flag = %u, jmsg = %s\n", jmsg_node->store_flag, jmsg_node->jmsg_buf);

				list_del(&jmsg_node->jobjCache_node);
				os_free(jmsg_node->jmsg_buf);
				os_free(jmsg_node);
				report_cnt++;
			}
			else {
				printf("report_cnt = %u, report interval = %u\n", report_cnt, japp->report_interval);
				break;
			}
			printf("finish one action\n");
		}
		report_cnt = 0;
	}

    return 0;
}

STATIC int
msgcache_init_timer(void)
{
    struct itimerspec jrt = OS_ITIMESPEC_INITER(os_second(MSG_REPORT_TICKS), os_nsecond(MSG_REPORT_TICKS));

    return os_loop_add_timer(&jlogd.loop, msgcache_report_timer, &jrt);
}

STATIC int
msgcache_load_config(jobj_t jcfg)
{
    int err, cfg_seq, count;
	char *create_jpath_cmd = NULL;

	jobj_foreach(jcfg, key_layer1, jcfg_tmp){
		if(0 == os_strncmp(MC_CFG_GLOBAL, key_layer1, os_strlen(MC_CFG_GLOBAL))){  // init global cfg
			jobj_t jmsg_path = jobj_get(jcfg_tmp, MC_CFG_MSGPATH);
			if(NULL != jmsg_path){
				char *jpath = NULL;
				uint32_t jlen = 0;
	    		jpath = os_strdup(jobj_get_string(jmsg_path));
				jlen  = os_strlen(jpath);
				printf("jlen: %u, jpath[jlen-1] = %c\n",jlen, jpath[jlen-1]);

				/* make sure the end of the string of path has / */
				if (jpath[jlen-1] != '/')
					os_asprintf(&jlogd.jmsg_path, "%s%s", jpath, "/");
			}
		}
		else if(0 == os_strncmp(MC_CFG_APPS, key_layer1, os_strlen(MC_CFG_APPS))){ // init apps cfg

			jobj_foreach(jcfg_tmp, key_layer2, japp_tmp){
				printf("key_layer2 == %s\n", key_layer2);
				msgcache_createapp_by_jobj(japp_tmp, key_layer2);
			}
		}
		else
			printf("wrong format, nothing to do!\n");
	}
	
	printf("jlogd.jmsg_path = %s\n", jlogd.jmsg_path);
	os_asprintf(&create_jpath_cmd, "%s %s", "mkdir -p", jlogd.jmsg_path);
	printf("create_jpath: %s\n", create_jpath_cmd);
	os_system_helper(create_jpath_cmd);

    return 0;
}

STATIC int
msgcache_init_config(void)
{
    int err, hindex;
    jobj_t jcfg = NULL;

	/* init hash table of apps in global variety jlogd */
	for (hindex = 0; hindex < MC_APP_HASHSIZE; hindex++) {
		INIT_LIST_HEAD(&jlogd.japps_hash[hindex]);
	}

	/* load jlogd.config*/
	printf("start to get jcfg from jlogd.config\n");
	
    jcfg = jobj_byfile(MSGCACHE_CFG);
    if (NULL==jcfg) {
        debug_error("invalid jlogd.config");
        
        err = -EBADCFG; goto error;
    }

    err = msgcache_load_config(jcfg);

error:
    jobj_put(jcfg);
    
    return err;
}

#ifdef SUPPORT_LOAD_APPS
STATIC int
msgcache_apps_load(void) 
{
	DIR *jmsgdir;
	struct dirent *jmsgptr;
	char *file_name = NULL;
	int len;
    STREAM f = NULL;
    char *line = NULL;
	jlogd_app_t *japp_cur = NULL;

    line = (char *)os_malloc(1+OS_FILE_LEN);
    if (NULL==line) {
        printf("there is something wrong with malloc line\n");
    }
		
	/* to see if there is apps file in the dir, if it is, traverse each file, and load jlogs to the corresponding app,
	*  if we can not find the corresponding app, then create one, put into it
	*/
	if (NULL != jlogd.jmsg_path) {
		jmsgdir = opendir(jlogd.jmsg_path);
		while(NULL != (jmsgptr=readdir(jmsgdir))){
			if (os_strncmp(jmsgptr->d_name, ".", os_strlen(jmsgptr->d_name))!=0 && os_strncmp(jmsgptr->d_name, "..", os_strlen(jmsgptr->d_name))!=0) {
				os_asprintf(&file_name, "%s%s", jlogd.jmsg_path, jmsgptr->d_name);
				printf("jlogd_apps_load: app name: %s, file name: %s\n", jmsgptr->d_name, file_name);

				japp_cur = msgcache_createapp_by_name(jmsgptr->d_name);

				f = os_fopen(file_name, "r");
				if (NULL==f) {
					printf("there is something wrong with opening: %s\n", file_name);
				}

				while(!os_feof(f)) {
					line[0] = 0;
					os_freadline(f, line, OS_LINE_LEN);
					/*
					* cut tail
					*	"\n"
					*	"\r"
					*	"\r\n"
					*/
					len = os_strlen(line);
					if (len>=1 && '\n'==line[len-1]) {
						line[len-1] = 0; len -= 1;
					}
					else if (len>=2 && '\r'==line[len-2] && '\n'==line[len-1]) {
						line[len-2] = 0; len -= 2;
					}
				
					if (false==is_good_str(line)) {
						printf("jlogd_apps_load: bad string\n");
						continue;
					}
					printf("jlogd_apps_load: line read: %s, japp_cur name: %s\n", line, japp_cur->appName);

					msgcache_jmsg_node_add(line, 1, japp_cur);
				}
			}
		}
	}
}
#endif
//chenxf add end

STATIC int
msgcache_init_env(void) 
{
    jlog_server_t *server;
    int err;

    err = __env_copy(OS_ENV(UNIX), JLOG_UNIX, 
            get_abstract_path(&jlogd.server[JLOGD_USERVER].addr.un), 
            abstract_path_size);
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int
msgcache_init_server(jlog_server_t *server)
{
    int fd, err, protocol, addrlen;

    if (AF_UNIX==server->family) {
        protocol    = 0;
        addrlen     = get_abstract_sockaddr_len(&server->addr.un);
    } else {
        protocol    = IPPROTO_UDP;
        addrlen     = sizeof(sockaddr_in_t);
    }
    
    fd = socket(server->family, SOCK_DGRAM, protocol);
    if (false==is_good_fd(fd)) {
    	__debug_error("server %s create socket error:%d", server->name, -errno);
        return -errno;
    }
    os_closexec(fd);

    err = bind(fd, &server->addr.c, addrlen);
    if (err<0) {
        __debug_error("server %s bind error:%d", server->name, -errno);
        return -errno;
    }
    
    server->fd = fd;

    os_loop_add_normal(&jlogd.loop, fd, msgcache_handle, server);
    
    return 0;
}

STATIC int 
msgcache_init_all_server(void)
{
    jlog_server_t *server;
    int err;

    foreach_server(server) {
        err = msgcache_init_server(server);
            __debug_trace_error(err, "init %s", server->name);
        if (err < 0) {
            return err;
        }
    }
    
    return 0;
}

STATIC void
msgcache_usr1(int sig)
{
    __debug_signal("recv USR1");
    
    if (false==os_hasbit(jlogd.event, SIGUSR1)) {
        os_setbit(jlogd.event, SIGUSR1);
    }
}

STATIC int 
msgcache_main_helper(int argc, char *argv[])
{
    int err;

	msgcache_init_config();

#ifdef SUPPORT_LOAD_APPS
	msgcache_apps_load();
#endif
    err = msgcache_init_env();
        __debug_trace_error(err, "init env");
    if (err<0) {
        return err;
    }

    err = msgcache_init_all_server();
    if (err<0) {
        return err;
    }

	msgcache_init_timer();

    return os_loop(&jlogd.loop);
}

int allinone_main(int argc, char *argv[])
{
    int cut[] = {SIGUSR1};

    setup_signal(msgcache_usr1, cut);
    setup_signal_exit(NULL);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(msgcache_main_helper, argc, argv);
}
/******************************************************************************/
