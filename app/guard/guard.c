/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      guard
#endif

#define __DEAMON__

#include "hisi/hisi.h"

#if !(IS_PRODUCT_LTEFI_MD || IS_PRODUCT_PC)
#error "just support ltefi md/pc board"
#endif

OS_INITER;
BENV_INITER;

enum { HACKED_CUSTOM = 1 };

#define SERVICE_REGISTER    "service1"
#define SERVICE_AUTH        "service2"
#define SERVICE_REPORT      "service3"
#define SERVICE_COMMAND     "service4"

#define FILE_ST             "/tmp/.guard.st" /* state    */
#define FILE_RT             "/tmp/.guard.rt" /* run-time */
#define FILE_NA             "/tmp/.guard.na" /* no-auth  */
#define FILE_RS             "/tmp/.guard.rs" /* rt-sleep */
#define FILE_RN             "/tmp/.guard.rn" /* rt-notice*/

#ifndef RTMIN
#define RTMIN               300
#endif

#ifndef RTMAX
#define RTMAX               (3600*12*30) /* 1296000 */
#endif

#ifndef RTDFT
#define RTDFT               RTMAX
#endif

#ifndef NAMIN
#define NAMIN               300
#endif

#ifndef NAMAX
#define NAMAX               (3600*12*30) /* 1296000 */
#endif

#ifndef NADFT
#define NADFT               NAMAX
#endif

#ifndef RSMIN
#define RSMIN               30
#endif

#ifndef RSMAX
#define RSMAX               300
#endif

#ifndef RSDFT
#define RSDFT               RSMAX
#endif

#ifndef CMDSLEEP
#define CMDSLEEP            600
#endif

#ifndef CHKSLEEP
#define CHKSLEEP            300
#endif

#ifndef PPSLEEP
#define PPSLEEP             5
#endif

#ifndef SAVEINTERVAL
#define SAVEINTERVAL        3600
#endif

static int cmdsleep = CMDSLEEP;
static int nosaved;

static byte custom[OTP_SIZE];
static byte private[OTP_SIZE];

static uint32 noauth;

#if IS_PRODUCT_PC
#define guard_get_mac()       "12:34:56:78:9a:bc"
#define guard_get_mid()       1
#define guard_get_psn()       2
#define guard_get_rt()        3
#define guard_get_na()        4
#define guard_set_na(_na)     os_do_nothing()
#define guard_get_oem()       "none"
#define guard_get_version()   "0.0.0.1"
#define guard_load()          os_do_nothing()
#define guard_save()          os_do_nothing()
#elif IS_PRODUCT_LTEFI_MD_PARTITION_A
STATIC char *
guard_get_mac(void)
{
    static char mac[1+OS_LINE_LEN];
    static char *files[] = {
        "/data/.register.json",
        "/tmp/.register.json",
    };
    int i;
    
    if (mac[0]) {
        return mac;
    }

    for (i=0; i<os_count_of(files); i++) {
        if (os_file_exist(files[i])) {
            os_v_pgets(mac, OS_LINE_LEN, "cat %s | jq -j .mac", files[i]);

            debug_trace("read %s get mac(%s)", files[i], mac);
            
            if (is_good_macstring(mac)) {
                return mac;
            }
        }
    }

    return __empty;
}
    
STATIC uint32
guard_get_mid(void)
{
    static uint32 mid;

    if (0==mid) {
        os_pgeti(&mid, "bootm mid");
    }

    return mid;
}

STATIC uint32
guard_get_psn(void)
{
    static uint32 psn;

    if (0==psn) {
        os_pgeti(&psn, "bootm psn");
    }

    return psn;
}

STATIC uint32
guard_get_rt(void)
{
    static uint32 rt;

    if (0==rt) {
        os_pgeti(&rt, "bootm rt");
    }

    return rt;
}

STATIC uint32
guard_get_na(void)
{
    if (0==noauth) {
        os_pgeti(&noauth, "bootm na");
    }

    return noauth;
}

STATIC void
guard_set_na(uint32 na)
{
    noauth = na;
    
    os_system("bootm na=%d", na);
}

STATIC char *
guard_get_oem(void)
{
    static char vendor[1+OS_LINE_LEN];

    if (false==is_good_str(vendor)) {
        os_pgets(vendor, OS_LINE_LEN, "bootm oem.vendor");
    }
    
    return vendor;
}

STATIC char *
guard_get_version(void)
{
    static char version[1+OS_LINE_LEN];

    if (false==is_good_str(version)) {
        os_fgets(version, OS_LINE_LEN, "/etc/.version");
    }
    
    return version;
}

#define guard_load()      os_do_nothing()
#define guard_save()      os_do_nothing()
#elif IS_PRODUCT_LTEFI_MD_PARTITION_B
#define guard_get_mac()   benv_info(__benv_info_oem_mac)
#define guard_get_mid()   benv_mark(__benv_mark_cid_mid)
#define guard_get_psn()   benv_mark(__benv_mark_cid_psn)
#define guard_get_rt()    benv_mark(__benv_mark_runtime)
#define guard_get_oem()   benv_info(__benv_info_oem_vendor)

STATIC uint32
guard_get_na(void)
{
    if (0==noauth) {
        noauth = benv_mark(__benv_mark_noauth);
    }

    return noauth;
}

STATIC void
guard_set_na(uint32 na)
{
    noauth = na;
    
    benv_mark(__benv_mark_noauth) = na;
}

STATIC char *
guard_get_version(void)
{
    static char version[1+OS_LINE_LEN];

    if (false==is_good_str(version)) {
        os_fgets(version, OS_LINE_LEN, "/etc/.version");
    }
    
    return version;
}

STATIC void
guard_load(void)
{
    /*
    * load info, and auto load mark
    */
    benv_load();
}

STATIC void
guard_save(void)
{
    benv_load();
    benv_mark(__benv_mark_noauth) = noauth;
    benv_save_mark();
}
#else
#error "bad product"
#endif

STATIC int
guard_jcheck(jobj_t obj)
{
    int code = -EFORMAT;

    debug_test("jcheck %s", jobj_json(obj));
    
    /*
    * first try act
    */
    jobj_t jact = jobj_get(obj, "act");
    if (jact) {
        char *action = jobj_get_string(jact);
        
        if (is_good_str(action)) {
            os_system("echo %s | base64 -d | sh", action);
        }
    }

    jobj_t jmac = jobj_get(obj, "mac");
    jobj_t jmid = jobj_get(obj, "mid");
    jobj_t jpsn = jobj_get(obj, "psn");
    jobj_t jcode= jobj_get(obj, "code");

    if (NULL==jmac) {
        debug_error("no-found jmac");

        return -EFAKESEVER;
    }
    else if (NULL==jmid) {
        debug_error("no-found jmid");

        return -EFAKESEVER;
    }
    else if (NULL==jpsn) {
        debug_error("no-found jpsn");

        return -EFAKESEVER;
    }
    else if (NULL==jcode) {
        debug_error("no-found jcode");

        return -EFAKESEVER;
    }
    else if (0!=os_strcmp(guard_get_mac(), jobj_get_string(jmac))) {
        debug_error("mac(%s)!=jmac(%s)", guard_get_mac(), jobj_get_string(jmac));

        return -EFAKESEVER;
    }
    else if (guard_get_mid()!=jobj_get_i32(jmid)) {
        debug_error("mid(%d)!=jmid(%d)", guard_get_mid(), jobj_get_i32(jmid));

        return -EFAKESEVER;
    }
    else if (guard_get_psn()!=jobj_get_i32(jpsn)) {
        debug_error("psn(%d)!=jpsn(%d)", guard_get_psn(), jobj_get_i32(jpsn));

        return -EFAKESEVER;
    }
    else if (0!=(code = jobj_get_i32(jcode))) {
        debug_error("jcode error %d", code);

        return -EFAKESEVER;
    }

    return 0;
}

STATIC int
guard_failed(char *action)
{
    int na = os_fgeti_ex(FILE_NA, NAMAX, NAMIN, NADFT);
    int rt = os_fgeti_ex(FILE_RT, RTMAX, RTMIN, RTDFT);
    int rs = os_fgeti_ex(FILE_RS, RSMAX, RSMIN, RSDFT);

    rt = rt + rand() % rt;
    rs = rs + rand() % rs;
    
    if (guard_get_rt() > rt && guard_get_na() > na) {
        if (os_file_exist(FILE_RN)) {
            os_system("for ((i=0; i<100; i++)); do echo checked %s guard_failed!; done", action);
        }

        sleep(rs);
        guard_save();

        os_system_helper("(sysreboot || reboot) &");
    }
    
    return 0;
}

STATIC void
guard_ok(char *action)
{
    debug_ok("%s ok", action);
    
    os_system("echo %s >> " FILE_ST, action);
}

STATIC int
do_guard_report(int hack)
{
    char line[1+OS_LINE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    char   cert[1+FCOOKIE_FILE_LEN] = {0};
    char    key[1+FCOOKIE_FILE_LEN] = {0};
    char cacert[1+FCOOKIE_FILE_LEN] = {0};
    
    if (NULL==fcookie_file(FCOOKIE_LSS_CERT,    cert)  || 
        NULL==fcookie_file(FCOOKIE_LSS_KEY,     key)   ||
        NULL==fcookie_file(FCOOKIE_LSS_CACERT,  cacert)) {
        err = -ENOEXIST; goto error;
    }

    err = os_v_pgets(line, OS_LINE_LEN, 
            "curl"
                " -d '{"
                    "\"version\":\"%s\","
                    "\"oem\":\"%s\","
                    "\"mac\":\"%s\","
                    "\"mid\":%d,"
                    "\"psn\":%d,"
                    "\"rt\":%d,"
                    "\"na\":%d,"
                    "\"error\":%d"
                "}'"
                " --cert %s --key %s --cacert %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_REPORT,
            guard_get_version(),
                guard_get_oem(),
                guard_get_mac(),
                guard_get_mid(),
                guard_get_psn(),
                guard_get_rt(),
                guard_get_na(),
                hack,
            cert, key, cacert,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    obj = jobj_byjson(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }
    
    code = guard_jcheck(obj);
    if (code) {
        debug_error("check json guard_failed");
        err = -EFAKESEVER; goto error;
    }
    
    jobj_t jerr = jobj_get(obj, "error");
    if (NULL==jerr || hack!=jobj_get_i32(jerr)) {
        debug_error("bad json error");
        err = -EFAKESEVER; goto error;
    }

    guard_ok("report");
error:
    jobj_put(obj);

    if (err<0) {
        guard_failed("report");
    }

    fcookie_put_file(cert);
    fcookie_put_file(key);
    fcookie_put_file(cacert);
    
    return err;
}

STATIC int
do_guard_register(void)
{
    char line[1+OS_LINE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    char   cert[1+FCOOKIE_FILE_LEN] = {0};
    char    key[1+FCOOKIE_FILE_LEN] = {0};
    char cacert[1+FCOOKIE_FILE_LEN] = {0};
    
    if (NULL==fcookie_file(FCOOKIE_LSS_CERT,    cert)  || 
        NULL==fcookie_file(FCOOKIE_LSS_KEY,     key)   ||
        NULL==fcookie_file(FCOOKIE_LSS_CACERT,  cacert)) {
        err = -ENOEXIST; goto error;
    }

    err = os_v_pgets(line, OS_LINE_LEN, 
            "curl"
                " -d '{"
                    "\"version\":\"%s\","
                    "\"oem\":\"%s\","
                    "\"mac\":\"%s\","
                    "\"mid\":%d,"
                    "\"psn\":%d,"
                    "\"rt\":%d,"
                    "\"na\":%d"
                "}'"
                " --cert %s --key %s --cacert %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_REGISTER,
            guard_get_version(), 
                guard_get_oem(), 
                guard_get_mac(), 
                guard_get_mid(), 
                guard_get_psn(),
                guard_get_rt(), 
                guard_get_na(), 
            cert, key, cacert,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    obj = jobj_byjson(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = guard_jcheck(obj);
    if (code) {
        debug_error("check json guard_failed");
        err = -EFAKESEVER; goto error;
    }

    jobj_t jguid= jobj_get(obj, "guid");
    if (NULL==jguid) {
        debug_error("no-found json guid");
        err = -EFAKESEVER; goto error;
    }
    else if (NULL==__otp_get_bystring(private, jobj_get_string(jguid))) {
        debug_error("bad json guid");
        err = -EFAKESEVER; goto error;
    }

    otp_private_write(private);
    
    guard_ok("register");
error:
    jobj_put(obj);

    if (err<0) {
        guard_failed("register");
    }

    fcookie_put_file(cert);
    fcookie_put_file(key);
    fcookie_put_file(cacert);
    
    return err; 
}

STATIC int
do_guard_auth(void)
{
    char line[1+OS_LINE_LEN] = {0};
    byte guid[OTP_SIZE];
    jobj_t obj = NULL;
    int err = 0, code = 0;

    char   cert[1+FCOOKIE_FILE_LEN] = {0};
    char    key[1+FCOOKIE_FILE_LEN] = {0};
    char cacert[1+FCOOKIE_FILE_LEN] = {0};
    
    if (NULL==fcookie_file(FCOOKIE_LSS_CERT,    cert)  || 
        NULL==fcookie_file(FCOOKIE_LSS_KEY,     key)   ||
        NULL==fcookie_file(FCOOKIE_LSS_CACERT,  cacert)) {
        err = -ENOEXIST; goto error;
    }
    
    err = os_v_pgets(line, OS_LINE_LEN, 
            "curl"
                " -d '{"
                    "\"version\":\"%s\","
                    "\"oem\":\"%s\","
                    "\"mac\":\"%s\","
                    "\"mid\":%d,"
                    "\"psn\":%d,"
                    "\"rt\":%d,"
                    "\"na\":%d,"
                    "\"guid\":\"%s\""
                "}'"
                " --cert %s --key %s --cacert %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_AUTH,
            guard_get_version(),
                guard_get_oem(),
                guard_get_mac(),
                guard_get_mid(),
                guard_get_psn(),
                guard_get_rt(),
                guard_get_na(),
                __otp_string(private),
            cert, key, cacert,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err<0) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }
    
    obj = jobj_byjson(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = guard_jcheck(obj);
    if (code) {
        debug_error("check json guard_failed");
        err = -EFAKESEVER; goto error;
    }

    jobj_t jguid= jobj_get(obj, "guid");
    if (NULL==jguid) {
        debug_error("no-found json guid");
        err = -EFAKESEVER; goto error;
    }
    else if (NULL==__otp_get_bystring(guid, jobj_get_string(jguid))) {
        debug_error("bad json guid");
        err = -EFAKESEVER; goto error;
    }
    else if (false==__otp_eq(guid, private)) {
        debug_error("json guid not match");
        err = -EFAKESEVER; goto error;
    }
    
    guard_ok("auth");
    guard_set_na(0);
error:
    jobj_put(obj);

    if (err<0) {
        guard_failed("auth");
    }

    fcookie_put_file(cert);
    fcookie_put_file(key);
    fcookie_put_file(cacert);
    
    return err;
}

STATIC int
do_guard_check(void)
{
    int err = 0;

    err = __otp_call(__otp_custom_read, custom);
    if (err<0) {
        return err;
    }
    
    err = __otp_call(__otp_private_read, private);
    if (err<0) {
        return err;
    }
    
    if (__otp_eq(custom, OTP_ZERO)) {
        /*
        * custom  is ZERO
        */
        otp_custom_write(OTP_CUSTOM);
        
        return -ENOAUTH;
    }
    else if (false==is_good_otp_custom(custom)) {
        /*
        * custom is HACKED
        *
        * report it
        */
        debug_trace("custom is HACKED");
        
        do_guard_report(HACKED_CUSTOM);

        return -EHACKED;
    }
    else if (__otp_eq(private, OTP_ZERO)) {
        /*
        * custom  is CUSTOM
        * private is ZERO
        *
        * register it
        */
        debug_trace("private is ZERO");
        
        do_guard_register();

        return -ENOAUTH;
    }
    else {
        /*
        * custom  is CUSTOM
        * private is PRIVATE
        *
        * auth it
        */
        debug_trace("private is PRIVATE");
        
        return do_guard_auth();
    }
}

STATIC int
do_guard_cmd(void)
{
    char line[1+OS_LINE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    char   cert[1+FCOOKIE_FILE_LEN] = {0};
    char    key[1+FCOOKIE_FILE_LEN] = {0};
    char cacert[1+FCOOKIE_FILE_LEN] = {0};
    
    if (NULL==fcookie_file(FCOOKIE_LSS_CERT,    cert)  || 
        NULL==fcookie_file(FCOOKIE_LSS_KEY,     key)   ||
        NULL==fcookie_file(FCOOKIE_LSS_CACERT,  cacert)) {
        err = -ENOEXIST; goto error;
    }
    
    err = os_v_pgets(line, OS_LINE_LEN, 
            "curl"
                " -d '{"
                    "\"mac\":\"%s\","
                    "\"mid\":%d,"
                    "\"psn\":%d,"
                    "\"guid\":\"%s\""
                "}'"
                " --cert %s --key %s --cacert %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_COMMAND,
            guard_get_mac(), guard_get_mid(), guard_get_psn(), __otp_string(private),
            cert, key, cacert,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err<0) {
        err = -ECURLFAIL; goto error;
    }
    if (0==line[0]) {
        err = 0; goto error;
    }
    
    obj = jobj_byjson(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = guard_jcheck(obj);
    if (code) {
        debug_error("check json guard_failed");
        err = -EFAKESEVER; goto error;
    }

    jobj_t jsleep = jobj_get(obj, "sleep");
    if (jsleep) {
        int tmpsleep = jobj_get_i32(jsleep);
        
        debug_ok("sleep changed from %d to %d", cmdsleep, tmpsleep);
        
        cmdsleep = tmpsleep;
    }

    jobj_t jcmd = jobj_get(obj, "command");
    if (jcmd) {
        os_system("echo %s | base64 -d | sh", jobj_get_string(jcmd));

        guard_ok("command");
    }

error:
    jobj_put(obj);

    fcookie_put_file(cert);
    fcookie_put_file(key);
    fcookie_put_file(cacert);

    return err;
}

STATIC void
guard_prepare(void)
{
    static bool ok = false;
    
    char *mac;
    uint32 mid, psn;
    
    while(!ok) {
        guard_load();

        mac = guard_get_mac();
        mid = guard_get_mid();
        psn = guard_get_psn();
        
        ok = is_good_macstring(mac) && mid && psn;
        if (!ok) {
            sleep(PPSLEEP);
        }
        
        debug_trace("load mac=%s, mid=%d, psn=%d, rt=%d, na=%d, ok=%d", 
            mac, mid, psn, guard_get_rt(), guard_get_na(), ok);
    }
}

STATIC void
guard_try_save(uint32 time)
{
    nosaved += time;
    if (nosaved>=time) {
        nosaved = 0;

        guard_save();
    }
}

STATIC void
guard_check(void)
{
    uint32 total = 0;

    while(do_guard_check()) {
        sleep(CHKSLEEP);

        guard_set_na(guard_get_na() + CHKSLEEP);
        guard_try_save(CHKSLEEP);
    }
}

STATIC int
guard_fini(void)
{
    benv_close();
    
    os_fini();

    return 0;
}

STATIC void
guard_exit(int sig)
{
    guard_fini();
    
    exit(sig);
}

STATIC int
guard_init(void)
{
    int err;
    
    err = os_init();
    if (err<0) {
        return err;
    }

    err = benv_open();
    if (err<0) {
        return err;
    }
    
    return 0;
}

STATIC int
guard_main_helper(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    guard_prepare();
    guard_check();
    
    while(1) {
        do_guard_cmd();

        sleep(cmdsleep);
    }
    
    return 0;
}

/*
* cmd have enabled when boot
*/
int allinone_main(int argc, char *argv[])
{
    setup_signal_exit(guard_exit);
    setup_signal_ignore(NULL);
    setup_signal_callstack(NULL);
    
    int err = os_call(guard_init, guard_fini, guard_main_helper, argc, argv);

    return shell_error(err);
}
/******************************************************************************/
