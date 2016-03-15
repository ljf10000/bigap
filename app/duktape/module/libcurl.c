#ifndef __THIS_APP
#define __THIS_APP      js
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "libcurl"
#endif

#include "utils.h"
#include "dukc.h"

#if duk_LIBCURL
#include <curl/curl.h>
#include "libcurl.h"

LIB_PARAM(curl_easy_init, 0);
static duk_ret_t
duke_curl_easy_init(duk_context *ctx)
{
    CURL *p = curl_easy_init();

    return __push_pointer(ctx, p), 1;
}

LIB_PARAM(curl_easy_cleanup, 1);
static duk_ret_t
duke_curl_easy_cleanup(duk_context *ctx)
{
    CURL *p = (CURL *)duk_require_pointer(ctx, 0);

    return curl_easy_cleanup(p), 0;
}

LIB_PARAM(curl_easy_perform, 1);
static duk_ret_t
duke_curl_easy_perform(duk_context *ctx)
{
    CURL *p = (CURL *)duk_require_pointer(ctx, 0);

    int err = curl_easy_perform(p);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(curl_easy_setopt, 3);
static duk_ret_t
duke_curl_easy_setopt(duk_context *ctx)
{
    CURL *p = (CURL *)duk_require_pointer(ctx, 0);
    int opt = duk_require_int(ctx, 1);
    int err;
    
    switch(opt) {
        // BEHAVIOR OPTIONS
        case CURLOPT_VERBOSE:
        case CURLOPT_HEADER:
        case CURLOPT_NOPROGRESS:
        case CURLOPT_NOSIGNAL:
        case CURLOPT_WILDCARDMATCH:
        // ERROR OPTIONS
        case CURLOPT_FAILONERROR:
        // NETWORK OPTIONS
        // case CURLOPT_PATH_AS_IS:
        case CURLOPT_DNS_USE_GLOBAL_CACHE:
        // NAMES and PASSWORDS OPTIONS (Authentication)
        case CURLOPT_SASL_IR:
        // HTTP OPTIONS
        case CURLOPT_AUTOREFERER:
        case CURLOPT_TRANSFER_ENCODING: 
        case CURLOPT_FOLLOWLOCATION:
        case CURLOPT_UNRESTRICTED_AUTH:
        case CURLOPT_PUT: 
        case CURLOPT_POST:
        case CURLOPT_COOKIESESSION:
        case CURLOPT_HTTPGET:
        case CURLOPT_IGNORE_CONTENT_LENGTH: 
        case CURLOPT_HTTP_CONTENT_DECODING: 
        case CURLOPT_HTTP_TRANSFER_DECODING:
        // FTP OPTIONS
        case CURLOPT_APPEND: 
        case CURLOPT_FTP_USE_EPRT: 
        case CURLOPT_FTP_USE_EPSV: 
        case CURLOPT_FTP_USE_PRET:
        case CURLOPT_FTP_SKIP_PASV_IP:
        // PROTOCOL OPTIONS
        case CURLOPT_TRANSFERTEXT:
        case CURLOPT_PROXY_TRANSFER_MODE: 
        case CURLOPT_CRLF:
        case CURLOPT_FILETIME: 
        case CURLOPT_DIRLISTONLY: 
        case CURLOPT_NOBODY: 
        case CURLOPT_UPLOAD:
        // CONNECTION OPTIONS
        case CURLOPT_FRESH_CONNECT: 
        case CURLOPT_FORBID_REUSE: 
        case CURLOPT_CONNECT_ONLY: 
        // SSL and SECURITY OPTIONS
        case CURLOPT_SSL_ENABLE_ALPN:
        case CURLOPT_SSL_ENABLE_NPN: 
        case CURLOPT_SSLENGINE_DEFAULT: 
        //case CURLOPT_SSL_FALSESTART:
        case CURLOPT_SSL_VERIFYPEER: 
        //case CURLOPT_SSL_VERIFYSTATUS:
        case CURLOPT_CERTINFO: 
        case CURLOPT_SSL_SESSIONID_CACHE: {
            bool val = duk_require_bool(ctx, 2);
            
            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
        case CURLOPT_PROXYPORT:
        case CURLOPT_PROXYTYPE:
        case CURLOPT_HTTPPROXYTUNNEL:
        case CURLOPT_SOCKS5_GSSAPI_NEC:
        case CURLOPT_LOCALPORT: 
        case CURLOPT_LOCALPORTRANGE: 
        case CURLOPT_DNS_CACHE_TIMEOUT:
        case CURLOPT_BUFFERSIZE: 
        case CURLOPT_PORT: 
        case CURLOPT_TCP_NODELAY: 
        case CURLOPT_ADDRESS_SCOPE: 
        case CURLOPT_TCP_KEEPALIVE: 
        case CURLOPT_TCP_KEEPIDLE: 
        case CURLOPT_TCP_KEEPINTVL:
        // NAMES and PASSWORDS OPTIONS (Authentication)
        case CURLOPT_NETRC:
        case CURLOPT_PROXYAUTH:
        // HTTP OPTIONS
        case CURLOPT_MAXREDIRS:
        case CURLOPT_POSTFIELDSIZE: 
        case CURLOPT_POSTFIELDSIZE_LARGE:
        case CURLOPT_HTTP_VERSION:
        case CURLOPT_EXPECT_100_TIMEOUT_MS: 
        //case CURLOPT_PIPEWAIT:
        //case CURLOPT_STREAM_WEIGHT:
        // TFTP OPTIONS
        case CURLOPT_TFTP_BLKSIZE:
        // FTP OPTIONS
        case CURLOPT_FTP_CREATE_MISSING_DIRS:
        case CURLOPT_FTP_RESPONSE_TIMEOUT:
        case CURLOPT_FTPSSLAUTH: 
        case CURLOPT_FTP_SSL_CCC:
        case CURLOPT_FTP_FILEMETHOD:
        // RTSP OPTIONS
        case CURLOPT_RTSP_REQUEST:
        case CURLOPT_RTSP_CLIENT_CSEQ: 
        case CURLOPT_RTSP_SERVER_CSEQ:
        // PROTOCOL OPTIONS
        case CURLOPT_RESUME_FROM: 
        case CURLOPT_RESUME_FROM_LARGE:
        case CURLOPT_INFILESIZE: 
        case CURLOPT_INFILESIZE_LARGE:
        case CURLOPT_MAXFILESIZE: 
        case CURLOPT_MAXFILESIZE_LARGE: 
        case CURLOPT_TIMECONDITION: 
        case CURLOPT_TIMEVALUE: 
        // CONNECTION OPTIONS
        case CURLOPT_TIMEOUT:
        case CURLOPT_TIMEOUT_MS: 
        case CURLOPT_LOW_SPEED_LIMIT: 
        case CURLOPT_LOW_SPEED_TIME: 
        case CURLOPT_MAX_SEND_SPEED_LARGE:
        case CURLOPT_MAX_RECV_SPEED_LARGE: 
        case CURLOPT_MAXCONNECTS:
        case CURLOPT_CONNECTTIMEOUT: 
        case CURLOPT_CONNECTTIMEOUT_MS:
        case CURLOPT_IPRESOLVE: 
        case CURLOPT_USE_SSL:
        case CURLOPT_ACCEPTTIMEOUT_MS:
        // SSL and SECURITY OPTIONS
        case CURLOPT_SSLVERSION: 
        case CURLOPT_SSL_VERIFYHOST: {
            int val = duk_require_int(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
        case CURLOPT_PROTOCOLS:
        case CURLOPT_REDIR_PROTOCOLS:
        // NAMES and PASSWORDS OPTIONS (Authentication)
        case CURLOPT_HTTPAUTH:
        // HTTP OPTIONS
        case CURLOPT_POSTREDIR: 
        // SSL and SECURITY OPTIONS
        case CURLOPT_SSL_OPTIONS: 
        case CURLOPT_GSSAPI_DELEGATION: 
        // SSH OPTIONS
        case CURLOPT_SSH_AUTH_TYPES: 
        // OTHER OPTIONS
        case CURLOPT_NEW_FILE_PERMS:
        case CURLOPT_NEW_DIRECTORY_PERMS: {
            duk_uint_t val = duk_require_uint(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // ERROR OPTIONS
        case CURLOPT_STDERR: {
            FILE *val = (FILE *)duk_require_pointer(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // ERROR OPTIONS
        case CURLOPT_ERRORBUFFER: {
            duk_buffer_t val = duk_require_buffer_data(ctx, 2, NULL);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
        case CURLOPT_URL:
        //case CURLOPT_DEFAULT_PROTOCOL: 
        case CURLOPT_PROXY:
        case CURLOPT_NOPROXY:
        case CURLOPT_SOCKS5_GSSAPI_SERVICE:
        //case CURLOPT_PROXY_SERVICE_NAME: 
        //case CURLOPT_SERVICE_NAME: 
        case CURLOPT_INTERFACE: 
        //case CURLOPT_UNIX_SOCKET_PATH:
        // NAMES and PASSWORDS OPTIONS (Authentication)
        case CURLOPT_NETRC_FILE:
        case CURLOPT_USERPWD: 
        case CURLOPT_PROXYUSERPWD: 
        case CURLOPT_USERNAME: 
        case CURLOPT_PASSWORD: 
        case CURLOPT_LOGIN_OPTIONS: 
        case CURLOPT_PROXYUSERNAME:
        case CURLOPT_PROXYPASSWORD:
        case CURLOPT_TLSAUTH_USERNAME:
        case CURLOPT_TLSAUTH_PASSWORD: 
        case CURLOPT_TLSAUTH_TYPE:
        case CURLOPT_XOAUTH2_BEARER:
        // HTTP OPTIONS
        case CURLOPT_ACCEPT_ENCODING:
        case CURLOPT_POSTFIELDS: 
        case CURLOPT_COPYPOSTFIELDS:
        case CURLOPT_REFERER: 
        case CURLOPT_USERAGENT:
        case CURLOPT_COOKIE: 
        case CURLOPT_COOKIEFILE: 
        case CURLOPT_COOKIEJAR:
        case CURLOPT_COOKIELIST:
        // SMTP OPTIONS
        case CURLOPT_MAIL_FROM:
        case CURLOPT_MAIL_AUTH:
        // FTP OPTIONS
        case CURLOPT_FTPPORT:
        case CURLOPT_PREQUOTE:
        case CURLOPT_FTP_ALTERNATIVE_TO_USER:
        case CURLOPT_FTP_ACCOUNT:
        // RTSP OPTIONS
        case CURLOPT_RTSP_SESSION_ID:
        case CURLOPT_RTSP_STREAM_URI:
        case CURLOPT_RTSP_TRANSPORT:
        // PROTOCOL OPTIONS
        case CURLOPT_RANGE: 
        case CURLOPT_CUSTOMREQUEST:
        // CONNECTION OPTIONS
        case CURLOPT_DNS_INTERFACE:
        case CURLOPT_DNS_LOCAL_IP4: 
        case CURLOPT_DNS_LOCAL_IP6: 
        case CURLOPT_DNS_SERVERS:
        // SSL and SECURITY OPTIONS
        case CURLOPT_SSLCERT:
        case CURLOPT_SSLCERTTYPE: 
        case CURLOPT_SSLKEY:
        case CURLOPT_SSLKEYTYPE: 
        case CURLOPT_KEYPASSWD:
        case CURLOPT_SSLENGINE:
        case CURLOPT_CAINFO: 
        case CURLOPT_ISSUERCERT: 
        case CURLOPT_CAPATH: 
        case CURLOPT_CRLFILE:
        //case CURLOPT_PINNEDPUBLICKEY: 
        case CURLOPT_RANDOM_FILE: 
        case CURLOPT_EGDSOCKET: 
        case CURLOPT_SSL_CIPHER_LIST: 
        case CURLOPT_KRBLEVEL: 
        // SSH OPTIONS
        case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
        case CURLOPT_SSH_PUBLIC_KEYFILE: 
        case CURLOPT_SSH_PRIVATE_KEYFILE: 
        case CURLOPT_SSH_KNOWNHOSTS: {
            char *val = (char *)duk_require_string(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // HTTP OPTIONS
        case CURLOPT_HTTPPOST: {
            struct curl_httppost_obj obj;

            __get_curl_httppost_obj(ctx, 2, &obj);

            err = curl_easy_setopt(p, opt, obj.post);
        }   break;
        // HTTP OPTIONS
        case CURLOPT_HTTPHEADER:
        case CURLOPT_PROXYHEADER: 
        case CURLOPT_HTTP200ALIASES: 
        // SMTP OPTIONS
        case CURLOPT_MAIL_RCPT:
        // FTP OPTIONS
        case CURLOPT_QUOTE:
        case CURLOPT_POSTQUOTE: 
        // CONNECTION OPTIONS
        case CURLOPT_RESOLVE:
        // TELNET OPTIONS
        case CURLOPT_TELNETOPTIONS: {
            struct curl_slist_obj obj;
            
            __get_curl_slist_obj(ctx, 2, &obj);
            
            err = curl_easy_setopt(p, opt, obj.list);
        }   break;
#if 0
        // HTTP OPTIONS
        case CURLOPT_STREAM_DEPENDS:
        case CURLOPT_STREAM_DEPENDS_E: {
            CURL *dep = (CURL *)duk_require_pointer(ctx, 2);

            err = curl_easy_setopt(p, opt, dep);
        }   break;
#endif
#if 0
        // CALLBACK OPTIONS
        case CURLOPT_WRITEFUNCTION:
        case CURLOPT_WRITEDATA:
        case CURLOPT_READFUNCTION:
        case CURLOPT_READDATA:
        case CURLOPT_IOCTLFUNCTION:
        case CURLOPT_IOCTLDATA:
        case CURLOPT_SEEKFUNCTION:
        case CURLOPT_SEEKDATA:
        case CURLOPT_SOCKOPTFUNCTION:
        case CURLOPT_SOCKOPTDATA:
        case CURLOPT_OPENSOCKETFUNCTION:
        case CURLOPT_OPENSOCKETDATA:
        case CURLOPT_CLOSESOCKETFUNCTION:
        case CURLOPT_CLOSESOCKETDATA:
        case CURLOPT_PROGRESSFUNCTION:
        case CURLOPT_PROGRESSDATA:
        case CURLOPT_XFERINFOFUNCTION:
        case CURLOPT_XFERINFODATA:
        case CURLOPT_HEADERFUNCTION:
        case CURLOPT_HEADERDATA:
        case CURLOPT_DEBUGFUNCTION:
        case CURLOPT_DEBUGDATA:
        case CURLOPT_SSL_CTX_FUNCTION:
        case CURLOPT_SSL_CTX_DATA:
        case CURLOPT_CONV_TO_NETWORK_FUNCTION:
        case CURLOPT_CONV_FROM_NETWORK_FUNCTION:
        case CURLOPT_CONV_FROM_UTF8_FUNCTION:
        case CURLOPT_INTERLEAVEFUNCTION:
        case CURLOPT_INTERLEAVEDATA:
        case CURLOPT_CHUNK_BGN_FUNCTION:
        case CURLOPT_CHUNK_END_FUNCTION:
        case CURLOPT_CHUNK_DATA:
        case CURLOPT_FNMATCH_FUNCTION:
        case CURLOPT_FNMATCH_DATA:
#endif
        // SSH OPTIONS
        case CURLOPT_SSH_KEYFUNCTION:
        case CURLOPT_SSH_KEYDATA:
        // OTHER OPTIONS
        case CURLOPT_PRIVATE:
        case CURLOPT_SHARE:
        default:
            err = -ENOSUPPORT; 
            break;
    }

error:
    return duk_push_int(ctx, err), 1;
}
    
LIB_PARAM(curl_formadd, DUK_VARARGS);
static duk_ret_t
duke_curl_formadd(duk_context *ctx)
{
    struct curl_httppost_obj obj;
    int i, err;
    
    __get_curl_httppost_obj(ctx, 0, &obj);
    
    int argc = duk_get_argc(ctx);
    if (argc < (1+2*1) || argc > (1+2*32) || 1!=(argc%2)) {
        err = __seterrno(ctx, -EFORMAT); 

        return duk_push_int(ctx, err), 1;
    }
    
    argc -= 1;
    argc /= 2;
    
    struct { 
        int opt;
        void *value;
    } param[argc];
    
    for (i=0; i<argc; i++) {
        int idx = 1+2*i;
        
        param[i].opt = duk_require_int(ctx, idx);
        idx++;
        
        switch(param[i].opt) {
            // string
            case CURLFORM_COPYNAME:
            case CURLFORM_PTRNAME:
            case CURLFORM_COPYCONTENTS:
            case CURLFORM_FILECONTENT:
            case CURLFORM_FILE:
            case CURLFORM_CONTENTTYPE:
            case CURLFORM_FILENAME:
                if (false==duk_is_string(ctx, idx)) {
                    err = __seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = (void *)duk_get_string(ctx, idx);
                break;
            // buffer
            case CURLFORM_PTRCONTENTS:
            case CURLFORM_BUFFER:
            case CURLFORM_BUFFERPTR:
                if (false==duk_is_buffer(ctx, idx)) {
                    err = __seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = duk_get_buffer_data(ctx, idx, NULL);
                break;
            // int
            // case CURLFORM_CONTENTLEN:
            case CURLFORM_CONTENTSLENGTH:
            case CURLFORM_BUFFERLENGTH:
                if (false==duk_is_int(ctx, idx)) {
                    err = __seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = (void *)duk_get_int(ctx, idx);
                break;
            // no support
            case CURLFORM_ARRAY:
            case CURLFORM_CONTENTHEADER:
            default:
                err = __seterrno(ctx, -ENOSUPPORT); 

                return duk_push_int(ctx, err), 1;
        }
    }

#define params(x)   param[x].opt, param[x].value
#define params_1    params(0)
#define params_2    params_1, params(1)
#define params_3    params_2, params(2)
#define params_4    params_3, params(3)
#define params_5    params_4, params(4)
#define params_6    params_5, params(5)
#define params_7    params_6, params(6)
#define params_8    params_7, params(7)
#define params_9    params_8, params(8)
#define params_10   params_9, params(9)
#define params_11   params_10, params(10)
#define params_12   params_11, params(11)
#define params_13   params_12, params(12)
#define params_14   params_13, params(13)
#define params_15   params_14, params(14)
#define params_16   params_15, params(15)
#define params_17   params_16, params(16)
#define params_18   params_17, params(17)
#define params_19   params_18, params(18)
#define params_20   params_19, params(19)
#define params_21   params_20, params(20)
#define params_22   params_21, params(21)
#define params_23   params_22, params(22)
#define params_24   params_23, params(23)
#define params_25   params_24, params(24)
#define params_26   params_25, params(25)
#define params_27   params_26, params(26)
#define params_28   params_27, params(27)
#define params_29   params_28, params(28)
#define params_30   params_29, params(29)
#define params_31   params_30, params(30)
#define params_32   params_31, params(31)

#define params_case(x)  case x: err = curl_formadd(&obj.post, &obj.last, params_##x, CURLFORM_END); break

    switch(argc) {
        params_case(1);
        params_case(2);
        params_case(3);
        params_case(4);
        params_case(5);
        params_case(6);
        params_case(7);
        params_case(8);
        params_case(9);
        params_case(10);
        params_case(11);
        params_case(12);
        params_case(13);
        params_case(14);
        params_case(15);
        params_case(16);
        params_case(17);
        params_case(18);
        params_case(19);
        params_case(20);
        params_case(21);
        params_case(22);
        params_case(23);
        params_case(24);
        params_case(25);
        params_case(26);
        params_case(27);
        params_case(28);
        params_case(29);
        params_case(30);
        params_case(31);
        params_case(32);
        default:err = __seterrno(ctx, -EFORMAT); break;
    }
    
#undef params
#undef params_1
#undef params_2
#undef params_3
#undef params_4
#undef params_5
#undef params_6
#undef params_7
#undef params_8
#undef params_9
#undef params_10
#undef params_11
#undef params_12
#undef params_13
#undef params_14
#undef params_15
#undef params_16
#undef params_17
#undef params_18
#undef params_19
#undef params_20
#undef params_21
#undef params_22
#undef params_23
#undef params_24
#undef params_25
#undef params_26
#undef params_27
#undef params_28
#undef params_29
#undef params_30
#undef params_31
#undef params_32
#undef params_case

    __set_curl_httppost_obj(ctx, 0, &obj);

    return duk_push_int(ctx, err), 1;
}

LIB_PARAM(curl_formfree, 1);
static duk_ret_t
duke_curl_formfree(duk_context *ctx)
{
    struct curl_httppost_obj obj;
    __get_curl_httppost_obj(ctx, 0, &obj);
    curl_formfree(obj.post);
    os_objzero(&obj);
    __set_curl_httppost_obj(ctx, 0, &obj);
    
    return 0;
}

LIB_PARAM(curl_slist_append, 1);
static duk_ret_t
duke_curl_slist_append(duk_context *ctx)
{
    struct curl_slist_obj obj;
    __get_curl_slist_obj(ctx, 0, &obj);
    char *string = (char *)duk_require_string(ctx, 1);
    
    struct curl_slist *list = curl_slist_append(obj.list, string);
    if (NULL==list) {
        return duk_push_false(ctx), 1;
    }

    obj.list = list;
    __set_curl_slist_obj(ctx, 0, &obj);
    
    return duk_push_true(ctx), 1;
}

LIB_PARAM(curl_slist_free_all, 1);
static duk_ret_t
duke_curl_slist_free_all(duk_context *ctx)
{
    struct curl_slist_obj obj;
    __get_curl_slist_obj(ctx, 0, &obj);
    curl_slist_free_all(obj.list);
    os_objzero(&obj);
    __set_curl_slist_obj(ctx, 0, &obj);
    
    return 0;
}

#include "libcurl/libcurlf.c"
#include "libcurl/libcurln.c"
#endif

int libcurl_register(duk_context *ctx)
{
#if duk_LIBCURL
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libcurlf_register(ctx, -1);
            libcurln_register(ctx, -1);
        duk_put_prop_string(ctx, -2, duk_MOD_LIBCURL);
    duk_pop(ctx);
#endif

    return 0;
}

