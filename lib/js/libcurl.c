/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      libjs
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     libcurl
#endif

#define __RUNAS_UNKNOW__

#include "utils.h"
#include "js.h"

#if js_LIBCURL
#include <curl/curl.h>
#include "libcurl.h"
/******************************************************************************/
#define __LIB_VERSION(A, B, C)  ((A)<<16 | (B)<<8 | (C))
#define __LIB_VERSION_CURRENT   __LIB_VERSION(LIBCURL_VERSION_MAJOR, LIBCURL_VERSION_MINOR, LIBCURL_VERSION_PATCH)
#define LIB_VERSION(A, B, C)    (__LIB_VERSION_CURRENT >= __LIB_VERSION(A, B, C))

int
libcurl_get_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_forms *p = (struct curl_forms *)obj; os_objzero(p);
    
    p->option   = js_get_obj_int(ctx, idx, "option");
    p->value    = js_get_obj_string(ctx, idx, "value", NULL);

    return 0;
}

int
libcurl_set_curl_forms(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_forms *p = (struct curl_forms *)obj;
    
    js_set_obj_int(ctx, idx, "option", p->option);
    js_set_obj_string(ctx, idx, "value", (char *)p->value);

    return 0;
}

struct curl_httppost_obj {
    struct curl_httppost *post;
    struct curl_httppost *last;
};

int
libcurl_get_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_httppost_obj *p = (struct curl_httppost_obj *)obj; os_objzero(p);
    
    p->post = (struct curl_httppost *)js_get_obj_pointer(ctx, idx, "post");
    p->last = (struct curl_httppost *)js_get_obj_pointer(ctx, idx, "last");

    return 0;
}

int
libcurl_set_curl_httppost_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_httppost_obj *p = (struct curl_httppost_obj *)obj;
    
    js_set_obj_pointer(ctx, idx, "post", p->post);
    js_set_obj_pointer(ctx, idx, "last", p->last);

    return 0;
}

struct curl_slist_obj {
    struct curl_slist *list;
};

int
libcurl_get_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_slist_obj *p = (struct curl_slist_obj *)obj; os_objzero(p);
    
    p->list = (struct curl_slist *)js_get_obj_pointer(ctx, idx, "list");

    return 0;
}

int
libcurl_set_curl_slist_obj(duk_context *ctx, duk_idx_t idx, duk_object_t obj)
{
    struct curl_slist_obj *p = (struct curl_slist_obj *)obj;
    
    js_set_obj_pointer(ctx, idx, "list", p->list);

    return 0;
}
/******************************************************************************/
JS_PARAM(curl_easy_init, 0);
static duk_ret_t
duke_curl_easy_init(duk_context *ctx)
{
    CURL *p = curl_easy_init();

    return js_push_pointer(ctx, p), 1;
}

JS_PARAM(curl_easy_cleanup, 1);
static duk_ret_t
duke_curl_easy_cleanup(duk_context *ctx)
{
    CURL *p = (CURL *)duk_require_pointer(ctx, 0);

    return curl_easy_cleanup(p), 0;
}

JS_PARAM(curl_easy_perform, 1);
static duk_ret_t
duke_curl_easy_perform(duk_context *ctx)
{
    CURL *p = (CURL *)duk_require_pointer(ctx, 0);

    int err = curl_easy_perform(p);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(curl_easy_setopt, 3);
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
#if LIB_VERSION(7, 10, 0)
        case CURLOPT_NOSIGNAL:
#endif
#if LIB_VERSION(7, 21, 0)
        case CURLOPT_WILDCARDMATCH:
#endif
        // ERROR OPTIONS
        case CURLOPT_FAILONERROR:
        // NETWORK OPTIONS
#if LIB_VERSION(7, 42, 0)
        case CURLOPT_PATH_AS_IS:
#endif
#if 0 /* Subject for removal in the future. Do not use! */
        case CURLOPT_DNS_USE_GLOBAL_CACHE:
#endif
        // NAMES and PASSWORDS OPTIONS (Authentication)
#if LIB_VERSION(7, 31, 0)
        case CURLOPT_SASL_IR:
#endif
        // HTTP OPTIONS
        case CURLOPT_AUTOREFERER:
#if LIB_VERSION(7, 21, 6)
        case CURLOPT_TRANSFER_ENCODING:
#endif
        case CURLOPT_FOLLOWLOCATION:
        case CURLOPT_UNRESTRICTED_AUTH:
#if 0 /* Deprecated since 7.12.1. Do not use. */
        case CURLOPT_PUT:
#endif
        case CURLOPT_POST:
        case CURLOPT_COOKIESESSION:
        case CURLOPT_HTTPGET:
#if LIB_VERSION(7, 14, 1)
        case CURLOPT_IGNORE_CONTENT_LENGTH:
#endif
#if LIB_VERSION(7, 16, 2)
        case CURLOPT_HTTP_CONTENT_DECODING:
        case CURLOPT_HTTP_TRANSFER_DECODING:
#endif
        // FTP OPTIONS
#if LIB_VERSION(7, 16, 4)
        case CURLOPT_APPEND:
#endif
#if LIB_VERSION(7, 10, 5)
        case CURLOPT_FTP_USE_EPRT:
#endif
        case CURLOPT_FTP_USE_EPSV:
#if LIB_VERSION(7, 20, 0)
        case CURLOPT_FTP_USE_PRET:
#endif
#if LIB_VERSION(7, 14, 2)
        case CURLOPT_FTP_SKIP_PASV_IP:
#endif
        // PROTOCOL OPTIONS
        case CURLOPT_TRANSFERTEXT:
#if LIB_VERSION(7, 18, 0)
        case CURLOPT_PROXY_TRANSFER_MODE:
#endif
#if LIB_VERSION(7, 40, 0)
        case CURLOPT_CRLF:
#endif
        case CURLOPT_FILETIME:
#if LIB_VERSION(7, 16, 4)
        case CURLOPT_DIRLISTONLY:
#endif
        case CURLOPT_NOBODY:
        case CURLOPT_UPLOAD:
        // CONNECTION OPTIONS
        case CURLOPT_FRESH_CONNECT:
        case CURLOPT_FORBID_REUSE:
#if LIB_VERSION(7, 15, 2)
        case CURLOPT_CONNECT_ONLY:
#endif
        // SSL and SECURITY OPTIONS
#if LIB_VERSION(7, 36, 0)
        case CURLOPT_SSL_ENABLE_ALPN:
        case CURLOPT_SSL_ENABLE_NPN:
#endif
        case CURLOPT_SSLENGINE_DEFAULT:
#if LIB_VERSION(7, 42, 0)
        case CURLOPT_SSL_FALSESTART:
#endif
        case CURLOPT_SSL_VERIFYPEER:
#if LIB_VERSION(7, 41, 0)
        case CURLOPT_SSL_VERIFYSTATUS:
#endif
        case CURLOPT_CERTINFO:
#if LIB_VERSION(7, 16, 0)
        case CURLOPT_SSL_SESSIONID_CACHE:
#endif
        {
            bool val = duk_require_bool(ctx, 2);
            
            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
        case CURLOPT_PROXYPORT:
        case CURLOPT_PROXYTYPE:
        case CURLOPT_HTTPPROXYTUNNEL:
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_SOCKS5_GSSAPI_NEC:
#endif
#if LIB_VERSION(7, 15, 2)
        case CURLOPT_LOCALPORT:
        case CURLOPT_LOCALPORTRANGE:
#endif
        case CURLOPT_DNS_CACHE_TIMEOUT:
#if LIB_VERSION(7, 10, 0)
        case CURLOPT_BUFFERSIZE:
#endif
        case CURLOPT_PORT:
        case CURLOPT_TCP_NODELAY:
#if LIB_VERSION(7, 19, 0)
        case CURLOPT_ADDRESS_SCOPE:
#endif
#if LIB_VERSION(7, 25, 0)
        case CURLOPT_TCP_KEEPALIVE:
        case CURLOPT_TCP_KEEPIDLE:
#endif
        case CURLOPT_TCP_KEEPINTVL:
        // NAMES and PASSWORDS OPTIONS (Authentication)
        case CURLOPT_NETRC:
#if LIB_VERSION(7, 10, 7)
        case CURLOPT_PROXYAUTH:
#endif
        // HTTP OPTIONS
        case CURLOPT_MAXREDIRS:
        case CURLOPT_POSTFIELDSIZE:
        case CURLOPT_POSTFIELDSIZE_LARGE:
        case CURLOPT_HTTP_VERSION:
#if LIB_VERSION(7, 37, 0)
        case CURLOPT_HEADEROPT:
#endif
#if LIB_VERSION(7, 36, 0)
        case CURLOPT_EXPECT_100_TIMEOUT_MS:
#endif
#if LIB_VERSION(7, 43, 0)
        case CURLOPT_PIPEWAIT:
#endif
#if LIB_VERSION(7, 46, 0)
        case CURLOPT_STREAM_WEIGHT:
#endif
        // TFTP OPTIONS
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_TFTP_BLKSIZE:
#endif
#if LIB_VERSION(7, 48, 0)
        case CURLOPT_TFTP_NO_OPTIONS:
#endif
        // FTP OPTIONS
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_FTP_CREATE_MISSING_DIRS:
#endif
#if LIB_VERSION(7, 10, 8)
        case CURLOPT_FTP_RESPONSE_TIMEOUT:
#endif
#if LIB_VERSION(7, 12, 2)
        case CURLOPT_FTPSSLAUTH:
#endif
#if LIB_VERSION(7, 16, 1)
        case CURLOPT_FTP_SSL_CCC:
#endif
#if LIB_VERSION(7, 15, 1)
        case CURLOPT_FTP_FILEMETHOD:
#endif
        // RTSP OPTIONS
#if LIB_VERSION(7, 20, 0)
        case CURLOPT_RTSP_REQUEST:
        case CURLOPT_RTSP_CLIENT_CSEQ:
        case CURLOPT_RTSP_SERVER_CSEQ:
#endif
        // PROTOCOL OPTIONS
        case CURLOPT_RESUME_FROM:
#if LIB_VERSION(7, 11, 0)
        case CURLOPT_RESUME_FROM_LARGE:
#endif
#if LIB_VERSION(7, 23, 0)
        case CURLOPT_INFILESIZE:
        case CURLOPT_INFILESIZE_LARGE:
#endif
        case CURLOPT_MAXFILESIZE:
#if LIB_VERSION(7, 11, 0)
        case CURLOPT_MAXFILESIZE_LARGE:
#endif
        case CURLOPT_TIMECONDITION:
        case CURLOPT_TIMEVALUE:
        // CONNECTION OPTIONS
        case CURLOPT_TIMEOUT:
        case CURLOPT_TIMEOUT_MS:
        case CURLOPT_LOW_SPEED_LIMIT:
        case CURLOPT_LOW_SPEED_TIME:
#if LIB_VERSION(7, 15, 5)
        case CURLOPT_MAX_SEND_SPEED_LARGE:
        case CURLOPT_MAX_RECV_SPEED_LARGE:
#endif
        case CURLOPT_MAXCONNECTS:
        case CURLOPT_CONNECTTIMEOUT:
        case CURLOPT_CONNECTTIMEOUT_MS:
        case CURLOPT_IPRESOLVE:
#if LIB_VERSION(7, 11, 0)
        case CURLOPT_USE_SSL:
#endif
#if LIB_VERSION(7, 24, 0)
        case CURLOPT_ACCEPTTIMEOUT_MS:
#endif
        // SSL and SECURITY OPTIONS
#if LIB_VERSION(7, 18, 1)
        case CURLOPT_SSLVERSION:
#endif
        case CURLOPT_SSL_VERIFYHOST: {
            int val = duk_require_int(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_PROTOCOLS:
        case CURLOPT_REDIR_PROTOCOLS:
#endif
        // NAMES and PASSWORDS OPTIONS (Authentication)
#if LIB_VERSION(7, 10, 6)
        case CURLOPT_HTTPAUTH:
#endif
        // HTTP OPTIONS
#if LIB_VERSION(7, 17, 1)
        case CURLOPT_POSTREDIR:
#endif
        // SSL and SECURITY OPTIONS
#if LIB_VERSION(7, 25, 0)
        case CURLOPT_SSL_OPTIONS:
#endif
#if LIB_VERSION(7, 22, 0)
        case CURLOPT_GSSAPI_DELEGATION:
#endif
        // SSH OPTIONS
#if LIB_VERSION(7, 28, 0)
        case CURLOPT_SSH_AUTH_TYPES:
#endif
        // OTHER OPTIONS
#if LIB_VERSION(7, 16, 4)
        case CURLOPT_NEW_FILE_PERMS:
        case CURLOPT_NEW_DIRECTORY_PERMS:
#endif
        {
            duk_uint_t val = duk_require_uint(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // ERROR OPTIONS
        case CURLOPT_STDERR: {
            STREAM val = (STREAM)duk_require_pointer(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // ERROR OPTIONS
        case CURLOPT_ERRORBUFFER: {
            duk_buffer_t val = duk_require_buffer_data(ctx, 2, NULL);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // NETWORK OPTIONS
        case CURLOPT_URL:
#if LIB_VERSION(7, 45, 0)
        case CURLOPT_DEFAULT_PROTOCOL:
#endif
#if LIB_VERSION(7, 21, 7)
        case CURLOPT_PROXY:
#endif
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_NOPROXY:
#endif
#if LIB_VERSION(7, 19, 4)
        case CURLOPT_SOCKS5_GSSAPI_SERVICE:
#endif
#if LIB_VERSION(7, 43, 0)
        case CURLOPT_PROXY_SERVICE_NAME:
        case CURLOPT_SERVICE_NAME:
#endif
        case CURLOPT_INTERFACE:
#if LIB_VERSION(7, 40, 0)
        case CURLOPT_UNIX_SOCKET_PATH:
#endif
        // NAMES and PASSWORDS OPTIONS (Authentication)
#if LIB_VERSION(7, 10, 9)
        case CURLOPT_NETRC_FILE:
#endif
        case CURLOPT_USERPWD:
        case CURLOPT_PROXYUSERPWD:
#if LIB_VERSION(7, 19, 1)
        case CURLOPT_USERNAME:
        case CURLOPT_PASSWORD:
#endif
#if LIB_VERSION(7, 34, 0)
        case CURLOPT_LOGIN_OPTIONS:
#endif
#if LIB_VERSION(7, 19, 1)
        case CURLOPT_PROXYUSERNAME:
        case CURLOPT_PROXYPASSWORD:
#endif
#if LIB_VERSION(7, 21, 4)
        case CURLOPT_TLSAUTH_USERNAME:
        case CURLOPT_TLSAUTH_PASSWORD:
        case CURLOPT_TLSAUTH_TYPE:
#endif
#if LIB_VERSION(7, 33, 0)
        case CURLOPT_XOAUTH2_BEARER:
#endif
        // HTTP OPTIONS
#if LIB_VERSION(7, 21, 6)
        case CURLOPT_ACCEPT_ENCODING:
#endif
        case CURLOPT_POSTFIELDS:
        case CURLOPT_COPYPOSTFIELDS:
        case CURLOPT_REFERER: 
        case CURLOPT_USERAGENT:
        case CURLOPT_COOKIE:
        case CURLOPT_COOKIEFILE:
        case CURLOPT_COOKIEJAR:
#if LIB_VERSION(7, 39, 0)
        case CURLOPT_COOKIELIST:
#endif
        // SMTP OPTIONS
#if LIB_VERSION(7, 20, 0)
        case CURLOPT_MAIL_FROM:
#endif
#if LIB_VERSION(7, 25, 0)
        case CURLOPT_MAIL_AUTH:
#endif
        // FTP OPTIONS
#if LIB_VERSION(7, 19, 5)
        case CURLOPT_FTPPORT:
#endif
        case CURLOPT_PREQUOTE:
#if LIB_VERSION(7, 15, 5)
        case CURLOPT_FTP_ALTERNATIVE_TO_USER:
#endif
#if LIB_VERSION(7, 13, 0)
        case CURLOPT_FTP_ACCOUNT:
#endif
        // RTSP OPTIONS
#if LIB_VERSION(7, 20, 0)
        case CURLOPT_RTSP_SESSION_ID:
        case CURLOPT_RTSP_STREAM_URI:
        case CURLOPT_RTSP_TRANSPORT:
#endif
        // PROTOCOL OPTIONS
#if LIB_VERSION(7, 18, 0)
        case CURLOPT_RANGE:
#endif
#if LIB_VERSION(7, 34, 0)
        case CURLOPT_CUSTOMREQUEST:
#endif
        // CONNECTION OPTIONS
#if LIB_VERSION(7, 33, 0)
        case CURLOPT_DNS_INTERFACE:
        case CURLOPT_DNS_LOCAL_IP4:
        case CURLOPT_DNS_LOCAL_IP6:
#endif
#if LIB_VERSION(7, 24, 0)
        case CURLOPT_DNS_SERVERS:
#endif
        // SSL and SECURITY OPTIONS
        case CURLOPT_SSLCERT:
#if LIB_VERSION(7, 9, 3)
        case CURLOPT_SSLCERTTYPE:
#endif
        case CURLOPT_SSLKEY:
        case CURLOPT_SSLKEYTYPE:
#if LIB_VERSION(7, 16, 4)
        case CURLOPT_KEYPASSWD:
#endif
        case CURLOPT_SSLENGINE:
        case CURLOPT_CAINFO:
        case CURLOPT_ISSUERCERT:
        case CURLOPT_CAPATH:
#if LIB_VERSION(7, 19, 0)
        case CURLOPT_CRLFILE:
#endif
#if LIB_VERSION(7, 39, 0)
        case CURLOPT_PINNEDPUBLICKEY:
#endif
        case CURLOPT_RANDOM_FILE:
        case CURLOPT_EGDSOCKET:
        case CURLOPT_SSL_CIPHER_LIST:
#if LIB_VERSION(7, 16, 3)
        case CURLOPT_KRBLEVEL:
#endif
        // SSH OPTIONS
#if LIB_VERSION(7, 17, 1)
        case CURLOPT_SSH_HOST_PUBLIC_KEY_MD5:
#endif
#if LIB_VERSION(7, 26, 0)
        case CURLOPT_SSH_PUBLIC_KEYFILE:
#endif
#if LIB_VERSION(7, 16, 1)
        case CURLOPT_SSH_PRIVATE_KEYFILE:
#endif
#if LIB_VERSION(7, 19, 6)
        case CURLOPT_SSH_KNOWNHOSTS:
#endif
        {
            char *val = (char *)duk_require_string(ctx, 2);

            err = curl_easy_setopt(p, opt, val);
        }   break;
        // HTTP OPTIONS
        case CURLOPT_HTTPPOST: {
            struct curl_httppost_obj obj;

            libcurl_get_curl_httppost_obj(ctx, 2, &obj);

            err = curl_easy_setopt(p, opt, obj.post);
        }   break;
        // HTTP OPTIONS
        case CURLOPT_HTTPHEADER:
#if LIB_VERSION(7, 37, 0)
        case CURLOPT_PROXYHEADER:
#endif
#if LIB_VERSION(7, 10, 3)
        case CURLOPT_HTTP200ALIASES:
#endif
        // SMTP OPTIONS
#if LIB_VERSION(7, 20, 0)
        case CURLOPT_MAIL_RCPT:
#endif
        // FTP OPTIONS
#if LIB_VERSION(7, 24, 0)
        case CURLOPT_QUOTE:
#endif
        case CURLOPT_POSTQUOTE:
        // CONNECTION OPTIONS
#if LIB_VERSION(7, 21, 3)
        case CURLOPT_RESOLVE:
#endif
        // TELNET OPTIONS
        case CURLOPT_TELNETOPTIONS: {
            struct curl_slist_obj obj;
            
            libcurl_get_curl_slist_obj(ctx, 2, &obj);
            
            err = curl_easy_setopt(p, opt, obj.list);
        }   break;
        // HTTP OPTIONS
#if LIB_VERSION(7, 46, 0)
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
#if LIB_VERSION(7, 19, 6)
        case CURLOPT_SSH_KEYFUNCTION:
        case CURLOPT_SSH_KEYDATA:
#endif
        // OTHER OPTIONS
#if LIB_VERSION(7, 10, 3)
        case CURLOPT_PRIVATE:
#endif
        case CURLOPT_SHARE:
        default:
            err = -ENOSUPPORT; 
            break;
    }

error:
    return duk_push_int(ctx, err), 1;
}
    
JS_PARAM(curl_formadd, DUK_VARARGS);
static duk_ret_t
duke_curl_formadd(duk_context *ctx)
{
    struct curl_httppost_obj obj;
    int i, err;
    
    libcurl_get_curl_httppost_obj(ctx, 0, &obj);
    
    int argc = duk_get_argc(ctx);
    if (argc < (1+2*1) || argc > (1+2*32) || 1!=(argc%2)) {
        err = __js_seterrno(ctx, -EFORMAT); 

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
                    err = __js_seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = (void *)duk_get_string(ctx, idx);
                break;
            // buffer
            case CURLFORM_PTRCONTENTS:
            case CURLFORM_BUFFER:
            case CURLFORM_BUFFERPTR:
                if (false==duk_is_buffer(ctx, idx)) {
                    err = __js_seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = duk_get_buffer_data(ctx, idx, NULL);
                break;
            // int
            // case CURLFORM_CONTENTLEN:
            case CURLFORM_CONTENTSLENGTH:
            case CURLFORM_BUFFERLENGTH:
                if (false==duk_is_int(ctx, idx)) {
                    err = __js_seterrno(ctx, -EFORMAT); 

                    return duk_push_int(ctx, err), 1;
                }

                param[i].value = (void *)duk_get_int(ctx, idx);
                break;
            // no support
            case CURLFORM_ARRAY:
            case CURLFORM_CONTENTHEADER:
            default:
                err = __js_seterrno(ctx, -ENOSUPPORT); 

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
        default:err = __js_seterrno(ctx, -EFORMAT); break;
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

    libcurl_set_curl_httppost_obj(ctx, 0, &obj);

    return duk_push_int(ctx, err), 1;
}

JS_PARAM(curl_formfree, 1);
static duk_ret_t
duke_curl_formfree(duk_context *ctx)
{
    struct curl_httppost_obj obj;
    libcurl_get_curl_httppost_obj(ctx, 0, &obj);
    curl_formfree(obj.post);
    os_objzero(&obj);
    libcurl_set_curl_httppost_obj(ctx, 0, &obj);
    
    return 0;
}

JS_PARAM(curl_slist_append, 1);
static duk_ret_t
duke_curl_slist_append(duk_context *ctx)
{
    struct curl_slist_obj obj;
    libcurl_get_curl_slist_obj(ctx, 0, &obj);
    char *string = (char *)duk_require_string(ctx, 1);
    
    struct curl_slist *list = curl_slist_append(obj.list, string);
    if (NULL==list) {
        return duk_push_false(ctx), 1;
    }

    obj.list = list;
    libcurl_set_curl_slist_obj(ctx, 0, &obj);
    
    return duk_push_true(ctx), 1;
}

JS_PARAM(curl_slist_free_all, 1);
static duk_ret_t
duke_curl_slist_free_all(duk_context *ctx)
{
    struct curl_slist_obj obj;
    libcurl_get_curl_slist_obj(ctx, 0, &obj);
    curl_slist_free_all(obj.list);
    os_objzero(&obj);
    libcurl_set_curl_slist_obj(ctx, 0, &obj);
    
    return 0;
}

#include "libcurl/libcurlf.c"
#include "libcurl/libcurln.c"

int js_libcurl_register(duk_context *ctx)
{
    duk_push_global_object(ctx);
        duk_push_object(ctx);
            libcurlf_register(ctx, -1);
            libcurln_register(ctx, -1);
        duk_put_prop_string(ctx, -2, js_MOD_LIBCURL);
    duk_pop(ctx);

    debug_ok("register libcurl ok.");

    return 0;
}
#endif /* js_LIBCURL */

