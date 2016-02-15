/*******************************************************************************
Copyright (c) 2015-2016, xxx Networks. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      guard
#endif

#define __DEAMON__
#include "utils.h"
#include "otp/otp.h"
#include "benv/bcookie.h"
#include "fcookie/fcookie.h"

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

static int cmdsleep = CMDSLEEP;

static byte custom[OTP_SIZE];
static byte private[OTP_SIZE];

#if IS_PRODUCT_PC
#define get_mac()       "12:34:56:78:9a:bc"
#define get_mid()       1
#define get_psn()       2
#define get_rt()        3
#define get_na()        4
#define set_na(_na)     os_do_nothing()
#define get_version()   "0.0.0.1"
#define get_oem()       "none"
#define __prepare() os_do_nothing()
#elif 1==PRODUCT_VERSION
static char *
get_mac(void)
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
    
static uint32_t
get_mid(void)
{
    static uint32_t mid;

    if (0==mid) {
        os_pgeti(&mid, "bootm mid");
    }

    return mid;
}

static uint32_t
get_psn(void)
{
    static uint32_t psn;

    if (0==psn) {
        os_pgeti(&psn, "bootm psn");
    }

    return psn;
}

static uint32_t
get_rt(void)
{
    static uint32_t rt;

    if (0==rt) {
        os_pgeti(&rt, "bootm rt");
    }

    return rt;
}

static uint32_t na;

static uint32_t
get_na(void)
{
    if (0==na) {
        os_pgeti(&na, "bootm na");
    }

    return na;
}

static void
set_na(int NA)
{
    na = NA;
    
    os_system("bootm na=%d", NA);
}

static inline char *
get_version(void)
{
    static char version[1+OS_LINE_LEN];

    if (false==is_good_str(version)) {
        os_fgets(version, OS_LINE_LEN, "/etc/.version");
    }
    
    return version;
}

static inline char *
get_oem(void)
{
    static char vendor[1+OS_LINE_LEN];

    if (false==is_good_str(vendor)) {
        os_pgets(vendor, OS_LINE_LEN, "bootm oem.vendor");
    }
    
    return vendor;
}

#define __prepare() os_do_nothing()
#elif 2==PRODUCT_VERSION
#define get_mac()   benv_info_get(__benv_info_product_mac)
#define get_mid()   benv_mark_get(__benv_mark_cid_mid)
#define get_psn()   benv_mark_get(__benv_mark_cid_psn)
#define get_rt()    benv_mark_get(__benv_mark_runtime)
#define get_na()    benv_mark_get(__benv_mark_noauth)
#define set_na(_na) benv_mark_set(__benv_mark_noauth, _na)

static inline char *
get_version(void)
{
    static char version[1+OS_LINE_LEN];

    if (false==is_good_str(version)) {
        os_fgets(version, OS_LINE_LEN, "/etc/.version");
    }
    
    return version;
}

#define get_oem()   benv_info_get(__benv_info_oem_vendor)

static void
__prepare(void)
{
    benv_open();
    benv_load_mark();
    benv_load_info_named();
    benv_close();
}
#else
#error "bad product"
#endif

static void
prepare(void)
{
    static bool ok = false;
    
    while(false==ok) {
        __prepare();
        
        char *mac = get_mac();
        uint32_t mid = get_mid();
        uint32_t psn = get_psn();
        uint32_t rt  = get_rt();
        uint32_t na  = get_na();
        
        ok = is_good_macstring(mac) && mid && psn;
        if (false==ok) {
            sleep(PPSLEEP);
        }
        
        debug_trace("load mac=%s, mid=%d, psn=%d, rt=%d, na=%d, ok=%d", 
            mac, mid, psn, rt, na, ok);
    }
}

static int
jcheck(jobj_t obj)
{
    int code = -EFORMAT;

    debug_test("jcheck %s", jobj_string(obj));
    
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
    else if (0!=os_strcmp(get_mac(), jobj_get_string(jmac))) {
        debug_error("mac(%s)!=jmac(%s)", get_mac(), jobj_get_string(jmac));

        return -EFAKESEVER;
    }
    else if (get_mid()!=jobj_get_int(jmid)) {
        debug_error("mid(%d)!=jmid(%d)", get_mid(), jobj_get_int(jmid));

        return -EFAKESEVER;
    }
    else if (get_psn()!=jobj_get_int(jpsn)) {
        debug_error("psn(%d)!=jpsn(%d)", get_psn(), jobj_get_int(jpsn));

        return -EFAKESEVER;
    }
    else if (0!=(code = jobj_get_int(jcode))) {
        debug_error("jcode error %d", code);

        return -EFAKESEVER;
    }

    return 0;
}

static int
get_val(char *file, int max, int min, int deft)
{
    int val = deft;
    
    if (os_file_exist(file)) {
        os_pgeti(&val, file);

        val = os_safe_value(val, min,  max);
    }

    return val;
}

static int
failed(char *action)
{
    int na = get_val(FILE_NA, NAMAX, NAMIN, NADFT);
    int rt = get_val(FILE_RT, RTMAX, RTMIN, RTDFT);
    int rs = get_val(FILE_RS, RSMAX, RSMIN, RSDFT);

    rt = rt + rand() % rt;
    rs = rs + rand() % rs;
    
    if (get_rt() > rt && get_na() > na) {
        if (os_file_exist(FILE_RN)) {
            os_system("for ((i=0; i<100; i++)); do echo checked %s failed!; done");
        }

        sleep(rs);
        
        os_system("reboot &");
    }
    
    return 0;
}

static void
ok(char *action)
{
    debug_ok("%s ok", action);
    os_system("echo %s >> " FILE_ST, action);
}

static int
do_report(int hack)
{
    char line[1+OS_LINE_LEN] = {0};
    char cert[1+FCOOKIE_FILE_LEN] = {0};
    char key[1+FCOOKIE_FILE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    if (NULL==__fcookie_file(FCOOKIE_LSS_CERT, cert)) {
        debug_error("no cert %d", FCOOKIE_LSS_CERT);
        err = -ENOEXIST; goto error;
    }

    if (NULL==__fcookie_file(FCOOKIE_LSS_KEY, key)) {
        debug_error("no cert %d",  FCOOKIE_LSS_KEY);
        err = -ENOEXIST; goto error;
    }

#if 0
curl -d '{"mac":"00:1f:64:01:01:01","mid":1,"psn":2,"error":1}' -k --cert ./client.crt --key ./client.key -u LSS:LTEFISecurityServer2012-2015 -v https://its.raytight.com:9999/service3
#endif

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
                " -k --cert %s --key %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_REPORT,
            get_version(),
                get_oem(),
                get_mac(),
                get_mid(),
                get_psn(),
                get_rt(),
                get_na(),
                hack,
            cert, key,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    obj = jobj(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }
    
    code = jcheck(obj);
    if (code) {
        debug_error("check json failed");
        err = -EFAKESEVER; goto error;
    }
    
    jobj_t jerr = jobj_get(obj, "error");
    if (NULL==jerr || hack!=jobj_get_int(jerr)) {
        debug_error("bad json error");
        err = -EFAKESEVER; goto error;
    }

    ok("report");
error:
    jobj_put(obj);

    if (err) {
        failed("report");
    }

    if (key[0]) {
        unlink(key);
    }

    if (cert[0]) {
        unlink(cert);
    }
    
    return err;
}

static int
do_register(void)
{
    char line[1+OS_LINE_LEN] = {0};
    char cert[1+FCOOKIE_FILE_LEN] = {0};
    char key[1+FCOOKIE_FILE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    if (NULL==__fcookie_file(FCOOKIE_LSS_CERT, cert)) {
        debug_error("no cert %d", FCOOKIE_LSS_CERT);
        err = -ENOEXIST; goto error;
    }

    if (NULL==__fcookie_file(FCOOKIE_LSS_KEY, key)) {
        debug_error("no cert %d", FCOOKIE_LSS_KEY);
        err = -ENOEXIST; goto error;
    }
    
#if 0
    curl -d '{"mac":"00:1f:64:01:01:01","mid":1,"psn":2}' -k --cert ./client.crt --key ./client.key -u LSS:LTEFISecurityServer2012-2015 -v https://its.raytight.com:9999/service1
#endif

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
                " -k --cert %s --key %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_REGISTER,
            get_version(), 
                get_oem(), 
                get_mac(), 
                get_mid(), 
                get_psn(),
                get_rt(), 
                get_na(), 
            cert, key,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }

    obj = jobj(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = jcheck(obj);
    if (code) {
        debug_error("check json failed");
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

    __otp_call(__otp_private_write, private);
    
    ok("register");
error:
    jobj_put(obj);

    if (err) {
        failed("register");
    }

    if (key[0]) {
        unlink(key);
    }

    if (cert[0]) {
        unlink(cert);
    }
    
    return err; 
}

static int
do_auth(void)
{
    char line[1+OS_LINE_LEN] = {0};
    char cert[1+FCOOKIE_FILE_LEN] = {0};
    char key[1+FCOOKIE_FILE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    if (NULL==__fcookie_file(FCOOKIE_LSS_CERT, cert)) {
        debug_error("no cert %d", FCOOKIE_LSS_CERT);
        err = -ENOEXIST; goto error;
    }

    if (NULL==__fcookie_file(FCOOKIE_LSS_KEY, key)) {
        debug_error("no cert %d", FCOOKIE_LSS_KEY);
        err = -ENOEXIST; goto error;
    }
    
#if 0
    curl -d '{"mac":"00:1f:64:01:01:01","mid":1,"psn":2,"guid":"82b33d9c1acdd9626fe150273a800000"}' -k --cert ./client.crt --key ./client.key -u LSS:LTEFISecurityServer2012-2015 -v https://its.raytight.com:9999/service2
#endif
    
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
                " -k --cert %s --key %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_AUTH,
            get_version(),
                get_oem(),
                get_mac(),
                get_mid(),
                get_psn(),
                get_rt(),
                get_na(),
                __otp_string(private),
            cert, key,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    if (err) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }
    
    obj = jobj(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = jcheck(obj);
    if (code) {
        debug_error("check json failed");
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

    ok("auth");
    set_na(0);
error:
    jobj_put(obj);

    if (err) {
        failed("auth");
    }

    if (key[0]) {
        unlink(key);
    }

    if (cert[0]) {
        unlink(cert);
    }
    
    return err;
}

static int
do_check(void)
{
    int err = 0;

    err = __otp_call(__otp_custom_read, custom);
    if (err) {
        return err;
    }
    
    err = __otp_call(__otp_private_read, private);
    if (err) {
        return err;
    }
    
    if (__otp_eq(custom, OTP_ZERO)) {
        /*
        * custom  is ZERO
        */
        __otp_call(__otp_custom_write, OTP_CUSTOM);

        return -ENOAUTH;
    }
    else if (false==__otp_eq(custom, __OTP_CUSTOM)
          && false==__otp_eq(custom, OTP_CUSTOM)) {
        /*
        * custom is HACKED
        *
        * report it
        */
        debug_trace("custom is HACKED");
        
        do_report(HACKED_CUSTOM);

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
        
        do_register();

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
        
        return do_auth();
    }
}

static int
do_cmd(void)
{
    char line[1+OS_LINE_LEN] = {0};
    char cert[1+FCOOKIE_FILE_LEN] = {0};
    char key[1+FCOOKIE_FILE_LEN] = {0};
    jobj_t obj = NULL;
    int err = 0, code = 0;

    if (NULL==__fcookie_file(FCOOKIE_LSS_CERT, cert)) {
        debug_error("no cert %d", FCOOKIE_LSS_CERT);
        err = -ENOEXIST; goto error;
    }

    if (NULL==__fcookie_file(FCOOKIE_LSS_KEY, key)) {
        debug_error("no cert %d", FCOOKIE_LSS_KEY);
        err = -ENOEXIST; goto error;
    }
    
    err = os_v_pgets(line, OS_LINE_LEN, 
            "curl"
                " -d '{\"mac\":\"%s\",\"mid\":%d,\"psn\":%d,\"guid\":\"%s\"}'"
                " -k --cert %s --key %s"
                " -u %s:%s"
                " -s https://%s:%s/" SERVICE_COMMAND,
            get_mac(), get_mid(), get_psn(), __otp_string(private),
            cert, key,
            oem_lss_user, oem_lss_password,
            oem_lss_server, oem_lss_port);
    unlink(cert);
    unlink(key);
    if (err) {
        debug_error("curl err(%d)", err);
        err = -ECURLFAIL; goto error;
    }
    
    obj = jobj(line);
    if (NULL==obj) {
        debug_error("bad json=%s", line);
        err = -EFAKESEVER; goto error;
    }

    code = jcheck(obj);
    if (code) {
        debug_error("check json failed");
        err = -EFAKESEVER; goto error;
    }

    jobj_t jsleep = jobj_get(obj, "sleep");
    if (jsleep) {
        int tmpsleep = jobj_get_int(jsleep);
        
        debug_ok("sleep changed from %d to %d", cmdsleep, tmpsleep);
        
        cmdsleep = tmpsleep;
    }

    jobj_t jcmd = jobj_get(obj, "command");
    if (jcmd) {
        os_system("echo %s | base64 -d | sh", jobj_get_string(jcmd));

        ok("command");
    }

error:
    jobj_put(obj);

    return err;
}

static int
__fini(void)
{
    os_fini();

    return 0;
}

static void
__exit(int sig)
{
    __fini();
    
    exit(sig);
}

static int
__init(void)
{
    int err;
    
    err = os_init();
    if (err) {
        return err;
    }

    return 0;
}

static int
__main(int argc, char *argv[])
{
    prepare();
    
    while(do_check()) {
        sleep(CHKSLEEP);

        set_na(get_na() + CHKSLEEP);
    }

    while(1) {
        do_cmd();

        sleep(cmdsleep);
    }
    
    return 0;
}

#ifndef __BUSYBOX__
#define guard_main  main
#endif

/*
* cmd have enabled when boot
*/
int guard_main(int argc, char *argv[])
{
    setup_signal_exit(__exit);
    setup_signal_callstack(NULL);
    
    return os_call(__init, __fini, __main, argc, argv);
}
/******************************************************************************/
