/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      testdll
#endif

#define OS_EXTEND

#include "utils.h"
#include <dlfcn.h>

OS_INITER;

#if 0
#if 0
// void obstack_1grow (struct obstack *obstack-ptr, char c)
// void obstack_1grow_fast (struct obstack *obstack-ptr, char c)
// char * nl_langinfo (nl item item)
// nl_catd catopen (const char *cat_name, int flag)
// char * catgets (nl catd catalog_desc, int set, int message, const char *string)
// int catclose (nl catd catalog_desc)
// ENTRY * hsearch (ENTRY item, ACTION action)
// int hsearch_r (ENTRY item, ACTION action, ENTRY **retval, struct hsearch data *htab)
// void FD_ZERO (fd set *set)
// void FD_SET (int filedes, fd set *set)
// void FD_CLR (int filedes, fd set *set)
// int FD_ISSET (int filedes, const fd set *set)
#endif

#if 0 || 1
// void mtrace (void)
// void muntrace (void)
// void hdestroy (void)
// void _flushlbf (void)
// void sync (void)
typedef void f0_0_0(void);

// int munlockall (void)
// struct lconv * localeconv (void)
// int fcloseall (void)
// int getchar (void)
// int getchar_unlocked (void)
typedef uint32_t f0_4_0(void);
#endif

#if 1
// void perror (const char *message)
// void free (void *ptr)
// void cfree (void *ptr)
// void hdestroy_r (struct hsearch data *htab)
// void globfree (glob t *pglob)
// void globfree64 (glob64 t *pglob)
// void regfree (regex t *compiled)
// void flockfile (FILE *stream)
// void funlockfile (FILE *stream)
// void clearerr (FILE *stream)
// void clearerr_unlocked (FILE *stream)
// void rewind (FILE *stream)
// void __fpurge (FILE *stream)
// void setlinebuf (FILE *stream)
// void aio_init (const struct aioinit *init)
typedef void f1_0_4(uint32_t);

// void warn (const char *format, . . . )
// void warnx (const char *format, . . . )
typedef void f1_0_4x(void *, ...);

// int mcheck (void (*abortfn) (enum mcheck status status))
// enum mcheck_status mprobe (void *pointer)
// int obstack_init (struct obstack *obstack-ptr)
// int obstack_object_size (struct obstack *obstack-ptr)
// int obstack_room (struct obstack *obstack-ptr)
// int obstack_object_size (struct obstack *obstack-ptr)
// int obstack_alignment_mask (struct obstack *obstack-ptr)
// int obstack_chunk_size (struct obstack *obstack-ptr)
// int brk (void *addr)
// char * strerror (int errnum)
// void * malloc (size_t size)
// void * valloc (size t size)
// void * alloca (size t size)
// void *sbrk (ptrdiff t delta)
// void * obstack_finish (struct obstack *obstack-ptr)
// void * obstack_base (struct obstack *obstack-ptr)
// void * obstack_next_free (struct obstack *obstack-ptr)
// int mlockall (int flags)
// int islower (int c)
// int isupper (int c)
// int isalpha (int c)
// int isdigit (int c)
// int isalnum (int c)
// int isxdigit (int c)
// int ispunct (int c)
// int isspace (int c)
// int isblank (int c)
// int isgraph (int c)
// int isprint (int c)
// int iscntrl (int c)
// int isascii (int c)
// int tolower (int c)
// int toupper (int c)
// int toascii (int c)
// int _tolower (int c)
// int _toupper (int c)
// size_t strlen (const char *s)
// char * strdup (const char *s)
// char * strdupa (const char *s)
// char * basename (const char *filename)
// char * dirname (char *path)
// char * strfry (char *string)
// int mbsinit (const mbstate t *ps)
// int iconv_close (iconv t cd)
// int rpmatch (const char *response)
// char * gettext (const char *msgid)
// char * textdomain (const char *domainname)
// int hcreate (size t nel)
// int __freadable (FILE *stream)
// int __fwritable (FILE *stream)
// int __freading (FILE *stream)
// int __fwriting (FILE *stream)
// int fclose (FILE *stream)
// int ftrylockfile (FILE *stream)
// int putchar (int c)
// int putchar_unlocked (int c)
// int puts (const char *s)
// int fgetc (FILE *stream)
// int fgetc_unlocked (FILE *stream)
// int getc (FILE *stream)
// int getc_unlocked (FILE *stream)
// int getw (FILE *stream)
// char * gets (char *s)
// int feof (FILE *stream)
// int feof_unlocked (FILE *stream)
// int ferror (FILE *stream)
// int ferror_unlocked (FILE *stream)
// long int ftell (FILE *stream)
// off_t ftello (FILE *stream)
// int fflush (FILE *stream)
// int fflush_unlocked (FILE *stream)
// int __flbf (FILE *stream)
// size_t __fbufsize (FILE *stream)
// size_t __fpending (FILE *stream)
// int close (int filedes)
// int fileno (FILE *stream)
// int fileno_unlocked (FILE *stream)
// int shm_unlink (const char *name)
// int fsync (int fildes)
// int fdatasync (int fildes)
// int aio_read (struct aiocb *aiocbp)
// int aio_read64 (struct aiocb64 *aiocbp)
// int aio_write (struct aiocb *aiocbp)
// int aio_write64 (struct aiocb64 *aiocbp)
// int aio_error (const struct aiocb *aiocbp)
// int aio_error64 (const struct aiocb64 *aiocbp)
// ssize_t aio_return (struct aiocb *aiocbp)
// ssize_t aio_return64 (struct aiocb64 *aiocbp)
// int dup (int old)
typedef uint32_t f1_4_4(uint32_t);

// int printf (const char *template, . . . )
// int scanf (const char *template, . . . )
typedef uint32_t f1_4_4x(uint32_t, ...);

// char * l64a (long int n)
typedef uint32_t f1_4_8(uint64_t);

// long int a64l (const char *string)
// off64_t ftello64 (FILE *stream)
typedef uint64_t f1_8_4(uint32_t);
#endif

#if 2
// void vwarn (const char *format, va_list ap)
// void vwarnx (const char *format, va list ap)
// void obstack_free (struct obstack *obstack-ptr, void *object)
// void obstack_ptr_grow (struct obstack *obstack-ptr, void *data)
// void obstack_blank (struct obstack *obstack-ptr, int size);
// void obstack_int_grow (struct obstack *obstack-ptr, int data)
// void obstack_int_grow_fast (struct obstack *obstack-ptr, int data)
// void obstack_blank_fast (struct obstack *obstack-ptr, int size)
// void obstack_ptr_grow_fast (struct obstack *obstack-ptr, void *data)
// void bzero (void *block, size t size)
// void envz_strip (char **envz, size t *envz_len)
// void tdestroy (void *vroot, free fn t freefct)
// void twalk (const void *root, action fn t action)
// void setbuf (FILE *stream, char *buf)
typedef void f2_0_44(uint32_t, uint32_t);

// void err (int status, const char *format, . . . );
// void errx (int status, const char *format, . . . )
typedef void f2_0_44x(uint32_t, uint32_t, ...);

// void * realloc (void *ptr, size t newsize)
// void * obstack_alloc (struct obstack *obstack-ptr, int size)
// int mallopt (int param, int value)
// void * calloc (size t count, size t eltsize)
// void * aligned_alloc (size t alignment, size t size)
// void * memalign (size t boundary, size t size)
// int mlock (const void *addr, size t len)
// int munlock (const void *addr, size t len)
// size_t strnlen (const char *s, size t maxlen)
// char * strcpy (char *restrict to, const char *restrict from)
// char * strndup (const char *s, size t size)
// char * stpcpy (char *restrict to, const char *restrict from)
// char * strndupa (const char *s, size t size)
// char * strcat (char *restrict to, const char *restrict from)
// int strcmp (const char *s1, const char *s2)
// int strcasecmp (const char *s1, const char *s2)
// int strverscmp (const char *s1, const char *s2)
// int bcmp (const void *a1, const void *a2, size t size)
// int strcoll (const char *s1, const char *s2)
// void * rawmemchr (const void *block, int c)
// char * strchr (const char *string, int c)
// char * strchrnul (const char *string, int c)
// char * strrchr (const char *string, int c)
// char * strstr (const char *haystack, const char *needle)
// char * strcasestr (const char *haystack, const char *needle)
// size_t strspn (const char *string, const char *skipset)
// size_t strcspn (const char *string, const char *stopset)
// char * strpbrk (const char *string, const char *stopset)
// char * index (const char *string, int c)
// char * rindex (const char *string, int c)
// char * strtok (char *restrict newstring, const char *restrict delimiters)
// char * strsep (char **string_ptr, const char *delimiter)
// void * memfrob (void *mem, size t length)
// size_t argz_count (const char *argz, size t arg_len)
// int mblen (const char *string, size t size)
// iconv_t iconv_open (const char *tocode, const char *fromcode)
// char * setlocale (int category, const char *locale)
// char * dgettext (const char *domainname, const char *msgid)
// char * bindtextdomain (const char *domainname, const char *dirname)
// char * bind_textdomain_codeset (const char *domainname, const char *codeset)
// int hcreate_r (size t nel, struct hsearch data *htab)
// FILE * fopen (const char *filename, const char *opentype)
// FILE * fopen64 (const char *filename, const char *opentype)
// int __fsetlocking (FILE *stream, int type)
// int fwide (FILE *stream, int mode)
// int fputc (int c, FILE *stream)
// int fputc_unlocked (int c, FILE *stream)
// int putc (int c, FILE *stream)
// int putc_unlocked (int c, FILE *stream)
// int fputs (const char *s, FILE *stream)
// int fputs_unlocked (const char *s, FILE *stream)
// int putw (int w, FILE *stream)
// int ungetc (int c, FILE *stream)
// int vprintf (const char *template, va list ap)
// int vscanf (const char *template, va list ap)
// int fgetpos (FILE *stream, fpos t *position)
// int fsetpos (FILE *stream, const fpos t *position)
// int fgetpos64 (FILE *stream, fpos64 t *position)
// int fsetpos64 (FILE *stream, const fpos64 t *position)
// FILE * open_memstream (char **ptr, size t *sizeloc)
// int addseverity (int severity, const char *string)
// int creat (const char *filename, mode t mode)
// int creat64 (const char *filename, mode t mode)
// FILE * fdopen (int filedes, const char *opentype)
// int munmap (void *addr, size t length)
// int aio_fsync (int op, struct aiocb *aiocbp)
// int aio_fsync64 (int op, struct aiocb64 *aiocbp)
// int aio_cancel (int fildes, struct aiocb *aiocbp)
// int aio_cancel64 (int fildes, struct aiocb64 *aiocbp)
// int dup2 (int old, int new)
typedef uint32_t f2_4_44(uint32_t, uint32_t);

// int fprintf (FILE *stream, const char *template, . . . )
// int sprintf (char *s, const char *template, . . . )
// int asprintf (char **ptr, const char *template, . . . )
// int obstack_printf (struct obstack *obstack, const char *template, . . . )
// int fscanf (FILE *stream, const char *template, . . . )
// int sscanf (const char *s, const char *template, . . . )
// int open (const char *filename, int flags[, mode t mode])
// int open64 (const char *filename, int flags[, mode t mode])
// int fcntl (int filedes, int command, . . . )
// int ioctl (int filedes, int command, . . . )
typedef uint32_t f2_4_44x(uint32_t, uint32_t, ...);

typedef uint32_t f2_4_48(uint32_t, uint64_t);
#endif

#if 3
// void verr (int status, const char *format, va list ap)
// void verrx (int status, const char *format, va list ap)
// void obstack_grow (struct obstack *obstack-ptr, void *data, int size)
// void obstack_grow0 (struct obstack *obstack-ptr, void *data, int size)
// void bcopy (const void *from, void *to, size t size)
// void argz_extract (const char *argz, size t argz_len, char **argv)
// void argz_stringify (char *argz, size t len, int sep)
// void argz_delete (char **argz, size t *argz_len, char *entry)
// void envz_remove (char **envz, size t *envz_len, const char *name)
// void setbuffer (FILE *stream, char *buf, size t size)
typedef void f3_0_444(uint32_t, uint32_t, uint32_t);

// void error (int status, int errnum, const char *format, ...)
typedef void f3_0_444x(uint32_t, uint32_t, uint32_t, ...);

// int posix_memalign (void **memptr, size t alignment, size t size)
// char * strerror_r (int errnum, char *buf, size_t n)
// void * obstack_copy (struct obstack *obstack-ptr, void *address, int size)
// void * obstack_copy0 (struct obstack *obstack-ptr, void *address, int size)
// void * memcpy (void *restrict to, const void *restrict from, size t size)
// void * mempcpy (void *restrict to, const void *restrict from, size t size)
// void * memmove (void *to, const void *from, size t size)
// void * memset (void *block, int c, size t size)
// char * strncpy (char *restrict to, const char *restrict from, size t size)
// char * stpncpy (char *restrict to, const char *restrict from, size t size)
// char * strncat (char *restrict to, const char *restrict from, size t size)
// int memcmp (const void *a1, const void *a2, size t size)
// int strncmp (const char *s1, const char *s2, size t size)
// int strncasecmp (const char *s1, const char *s2, size t n)
// size_t strxfrm (char *restrict to, const char *restrict from, size t size)
// void * memchr (const void *block, int c, size t size)
// void * memrchr (const void *block, int c, size t size)
// char * strtok_r (char *newstring, const char *delimiters, char **save_ptr)
// error_t argz_create (char *const argv[], char **argz, size t *argz_len)
// error_t argz_add (char **argz, size t *argz_len, const char *str)
// char * argz_next (const char *argz, size t argz_len, const char *entry)
// char * envz_entry (const char *envz, size t envz_len, const char *name)
// char * envz_get (const char *envz, size t envz_len, const char *name)
// size_t mbrlen (const char *restrict s, size t n, mbstate t *ps)
// char * dcgettext (const char *domainname, const char *msgid, int category)
// void * tsearch (const void *key, void **rootp, comparison fn t compar)
// void * tfind (const void *key, void *const *rootp, comparison fn t compar)
// void * tdelete (const void *key, void **rootp, comparison fn t compar)
// int fnmatch (const char *pattern, const char *string, int flags)
// int regcomp (regex t *restrict compiled, const char *restrict pattern, int cflags)
// FILE * freopen (const char *filename, const char *opentype, FILE *stream)
// FILE * freopen64 (const char *filename, const char *opentype, FILE *stream)
// ssize_t getline (char **lineptr, size t *n, FILE *stream)
// char * fgets (char *s, int count, FILE *stream)
// char * fgets_unlocked (char *s, int count, FILE *stream)
// int vfprintf (FILE *stream, const char *template, va list ap)
// int vsprintf (char *s, const char *template, va list ap)
// int vasprintf (char **ptr, const char *template, va list ap)
// size_t parse_printf_format (const char *template, size t n, int *argtypes)
// int register_printf_function (int spec, printf function handler-function, printf arginfo function arginfo-function)
// int printf_size (FILE *fp, const struct printf info *info, const void *const *args)
// int printf_size_info (const struct printf info *info, size t n, int *argtypes)
// int vfscanf (FILE *stream, const char *template, va list ap)
// int vsscanf (const char *s, const char *template, va list ap)
// int fseek (FILE *stream, long int offset, int whence)
// int fseeko (FILE *stream, off t offset, int whence)
// FILE * fmemopen (void *buf, size t size, const char *opentype)
// FILE * fopencookie (void *cookie, const char *opentype, cookie io functions t io-functions)
// ssize_t read (int filedes, void *buffer, size t size)
// ssize_t write (int filedes, const void *buffer, size t size)
// char * ngettext (const char *msgid1, const char *msgid2, unsigned long int n)
// off_t lseek (int filedes, off t offset, int whence)
// ssize_t readv (int filedes, const struct iovec *vector, int count)
// ssize_t writev (int filedes, const struct iovec *vector, int count)
// int msync (void *address, size t length, int flags)
// int madvise (void *addr, size t length, int advice)
// int shm_open (const char *name, int oflag, mode t mode)
// int aio_suspend (const struct aiocb *const list[], int nent, const struct timespec *timeout)
// int aio_suspend64 (const struct aiocb64 *const list[], int nent, const struct timespec *timeout)
typedef uint32_t f3_4_444(uint32_t, uint32_t, uint32_t);

// int snprintf (char *s, size t size, const char *template, . . . )
typedef uint32_t f3_4_444x(uint32_t, uint32_t, uint32_t, ...);

typedef uint32_t f3_4_448(uint32_t, uint32_t, uint64_t);

// int fseeko64 (FILE *stream, off64 t offset, int whence)
typedef uint32_t f3_4_484(uint32_t, uint64_t, uint32_t);

// ssize_t strfmon (char *s, size t maxsize, const char *format, . . . )
typedef uint32_t f3_4_444x(uint32_t, uint32_t, uint32_t, ...);

// off64_t lseek64 (int filedes, off64 t offset, int whence)
typedef uint64_t f3_8_484(uint32_t, uint64_t, uint32_t);
#endif

#if 4
// void qsort (void *array, size t count, size t size, comparison fn t compare)
typedef void f4_0_4444(uint32_t, uint32_t, uint32_t, uint32_t);

// void * memccpy (void *restrict to, const void *restrict from, int c, size t size)
// void * memmem (const void *haystack, size t haystack-len, const void *needle, size t needle-len)
// error_t argz_create_sep (const char *string, int sep, char **argz, size t *argz_len)
// error_t argz_add_sep (char **argz, size t *argz_len, const char *str, int delim)
// error_t argz_append (char **argz, size t *argz_len, const char *buf, size t buf_len)
// error_t argz_insert (char **argz, size t *argz_len, char *before, const char *entry)
// error_t envz_add (char **envz, size t *envz_len, const char *name, const char *value)
// size_t iconv (iconv t cd, char **inbuf, size t *inbytesleft, char **outbuf, size t *outbytesleft)
// int glob (const char *pattern, int flags, int (*errfunc) (const char *filename, int error-code), glob t *vector-ptr)
// int glob64 (const char *pattern, int flags, int (*errfunc) (const char *filename, int error-code), glob64 t *vector-ptr)
// size_t regerror (int errcode, const regex t *restrict compiled, char *restrict buffer, size t length)
// ssize_t getdelim (char **lineptr, size t *n, int delimiter, FILE *stream)
// size_t fread (void *data, size t size, size t count, FILE *stream)
// size_t fread_unlocked (void *data, size t size, size t count, FILE *stream)
// size_t fwrite (const void *data, size t size, size t count, FILE *stream)
// size_t fwrite_unlocked (const void *data, size t size, size t count, FILE *stream)
// int vsnprintf (char *s, size t size, const char *template, va list ap)
// int obstack_vprintf (struct obstack *obstack, const char *template, va list ap)
// int setvbuf (FILE *stream, char *buf, int mode, size t size)
// ssize_t pread (int filedes, void *buffer, size t size, off t offset)
// ssize_t pwrite (int filedes, const void *buffer, size t size, off t offset)
// char * dngettext (const char *domain, const char *msgid1, const char *msgid2, unsigned long int n)
// void * mremap (void *address, size t length, size t new_length, int flag)
// int lio_listio (int mode, struct aiocb *const list[], int nent, struct sigevent *sig)
// int lio_listio64 (int mode, struct aiocb64 *const list[], int nent, struct sigevent *sig)
typedef uint32_t f4_4_4444(uint32_t, uint32_t, uint32_t, uint32_t);

// ssize_t pread64 (int filedes, void *buffer, size t size, off64 t offset)
// ssize_t pwrite64 (int filedes, const void *buffer, size t size, off64 t offset)
typedef uint32_t f4_4_4448(uint32_t, uint32_t, uint32_t, uint64_t);
#endif

#if 5
// error_t envz_merge (char **envz, size t *envz_len, const char *envz2, size t envz2_len, int override)
typedef void f5_0_44444(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

// void error_at_line (int status, int errnum, const char *fname, unsigned int lineno, const char *format, ...)
typedef void f5_0_44444x(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);

// error_t argz_replace (char **argz, size t *argz_len, const char *str, const char *with, unsigned *replace_count)
// void * lfind (const void *key, const void *base, size t *nmemb, size t size, comparison fn t compar)
// void * lsearch (const void *key, void *base, size t *nmemb, size t size, comparison fn t compar)
// void * bsearch (const void *key, const void *array, size t count, size t size, comparison fn t compare)
// int regexec (const regex t *restrict compiled, const char *restrict string, size t nmatch, regmatch t matchptr[restrict], int eflags)
// int select (int nfds, fd set *read-fds, fd set *write-fds, fd set *except-fds, struct timeval *timeout)
typedef uint32_t f5_4_44444(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ...);

// char * dcngettext (const char *domain, const char *msgid1, const char *msgid2, unsigned long int n, int category)
typedef uint32_t f5_4_44484(uint32_t, uint32_t, uint32_t, uint64_t, uint32_t);
#endif

#if 6
// int fmtmsg (long int classification, const char *label, int severity, const char *text, const char *action, const char *tag)
// void * mmap (void *address, size t length, int protect, int flags, int filedes, off t offset)
typedef uint32_t f6_4_444444(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

// void * mmap64 (void *address, size t length, int protect, int flags, int filedes, off64 t offset)
typedef uint32_t f6_4_444448(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t);
#endif
#endif


enum {
    LIB_T_POINTER,
    LIB_T_V32,
    LIB_T_V64,

    LIB_T_END
};

typedef struct {
    int size;

    union {
        uint64_t b8;
        uint32_t b4;
        void *p;
    } u;
} libval_t;

#define LIBVAL0_INITER()      {.size = 0}
#define LIBVAL4_INITER(_val)  { \
    .size = sizeof(uint32_t),   \
    .u = {                      \
        .b4 = (uint32_t)_val,   \
    },                          \
} /* end */
#define LIBVAL8_INITER(_val)  { \
    .size = sizeof(uint64_t),   \
    .u = {                      \
        .b8 = (uint64_t)(_val), \
    },                          \
} /* end */
#define LIBVALp_INITER(_val)  { \
    .size = sizeof(void *),     \
    .u = {                      \
        .p = (void *)(_val),    \
    },                          \
} /* end */

#define LIBRESULT_INITER(_type) {.size = sizeof(_type)}

typedef struct {
    int count;
    libval_t *param;
    
    libval_t result;
} libproto_t;

#define LIBPROTO_INITER(_type, _params)  {  \
    .count = os_count_of(_params),          \
    .param = _params,                       \
    .result = LIBRESULT_INITER(_type),      \
}

typedef void func_v_v(void);
typedef void func_v_4x(uint32_t, ...);
typedef void func_v_8x(uint64_t, ...);

typedef uint32_t func_4_v(void);
typedef uint32_t func_4_4x(uint32_t, ...);
typedef uint32_t func_4_8x(uint64_t, ...);

typedef uint64_t func_8_v(void);
typedef uint64_t func_8_4x(uint32_t, ...);
typedef uint64_t func_8_8x(uint64_t, ...);

#define LIBVAL(_val)                (4==(_val)->size?(_val)->u.b4:(_val)->u.b8)
#define LIBFUN(_f, _type)           ((_type)(_f))
#define LIBPARAM(_proto, _idx)      LIBVAL(&(_proto)->param[_idx])

#define LIBPARAM0(_proto)           /* nothing */
#define LIBPARAM1(_proto)           LIBPARAM(_proto, 0)
#define LIBPARAM2(_proto)           LIBPARAM1(_proto), LIBPARAM(_proto, 1)
#define LIBPARAM3(_proto)           LIBPARAM2(_proto), LIBPARAM(_proto, 2)
#define LIBPARAM4(_proto)           LIBPARAM3(_proto), LIBPARAM(_proto, 3)
#define LIBPARAM5(_proto)           LIBPARAM4(_proto), LIBPARAM(_proto, 4)
#define LIBPARAM6(_proto)           LIBPARAM5(_proto), LIBPARAM(_proto, 5)
#define LIBPARAM7(_proto)           LIBPARAM6(_proto), LIBPARAM(_proto, 6)
#define LIBPARAM8(_proto)           LIBPARAM7(_proto), LIBPARAM(_proto, 7)
#define LIBPARAM9(_proto)           LIBPARAM8(_proto), LIBPARAM(_proto, 8)
#define LIBPARAMx(_proto, _count)   LIBPARAM##_count(_proto)

#define LIBCALL(_f, _proto, _count)   do{   \
    if (0==(_proto)->result.size) {         \
        if (0==(_count)) {                  \
            LIBFUN(_f, func_v_v)(LIBPARAMx(_proto, _count)); \
        } else if (4==LIBPARAM1(_proto)->size) { \
            LIBFUN(_f, func_v_4x)(LIBPARAMx(_proto, _count)); \
        } else {                            \
            LIBFUN(_f, func_v_8x)(LIBPARAMx(_proto, _count));  \
        }                                   \
    } else if (4==(_proto)->result.size) {  \
        if (0==(_count)) {                  \
            (_proto)->result.b4 = LIBFUN(_f, func_4_v)(LIBPARAMx(_proto, _count)); \
        } else if (4==LIBPARAM1(_proto)->size) { \
            (_proto)->result.b4 = LIBFUN(_f, func_4_4x)(LIBPARAMx(_proto, _count)); \
        } else {                            \
            (_proto)->result.b4 = LIBFUN(_f, func_4_8x)(LIBPARAMx(_proto, _count));  \
        }                                   \
    } else {                                \
        if (0==(_count)) {                  \
            (_proto)->result.b8 = LIBFUN(_f, func_8_v)(LIBPARAMx(_proto, _count)); \
        } else if (4==LIBPARAM1(_proto)->size) { \
            (_proto)->result.b8 = LIBFUN(_f, func_8_4x)(LIBPARAMx(_proto, _count)); \
        } else {                            \
            (_proto)->result.b8 = LIBFUN(_f, func_8_8x)(LIBPARAMx(_proto, _count));  \
        }                                   \
    }                                       \
}while(0)

static void
__libcall(void *f, libproto_t *proto)
{
    switch(proto->count) {
        default:
        case 0:
            LIBCALL(f, proto, 0);
            break;
        case 1:
            LIBCALL(f, proto, 1);
            break;
        case 2:
            LIBCALL(f, proto, 2);
            break;
        case 3:
            LIBCALL(f, proto, 3);
            break;
        case 4:
            LIBCALL(f, proto, 4);
            break;
        case 5:
            LIBCALL(f, proto, 5);
            break;
        case 6:
            LIBCALL(f, proto, 6);
            break;
        case 7:
            LIBCALL(f, proto, 7);
            break;
        case 8:
            LIBCALL(f, proto, 8);
            break;
        case 9:
            LIBCALL(f, proto, 9);
            break;
    }
}

static int
libcall(const char *lib, const char *sym, libproto_t *proto)
{
    void *h = dlopen(lib, RTLD_LAZY);
    if (NULL==h) {
        return -ELOADDLL;
    }

    dlerror();

    char *errstring;
    void *f = dlsym(h, sym);
    if (NULL==f) {
        return -ELOADSYM;
    }
    else if (NULL!=(errstring=dlerror())) {
        debug_error("load %s:%s error:%s", lib, sym, errstring);

        return -ELOADSYM;
    }

    __libcall(f, proto);
    
    if (proto->result.u.b4) {
        return (int)0==proto->result.u.b4;
    } else {
        return 0;
    }
}

static int
__main(int argc, char *argv[])
{
    int err = 0;

    libval_t params[] = {
        [0] = LIBVALp_INITER("libc %s number %d"),
        [1] = LIBVALp_INITER("printf"),
        [2] = LIBVAL4_INITER(100),
    };
    libproto_t proto = LIBPROTO_INITER(int, params);
    
    libcall("libc.so", "printf", &proto);
    
    return err;
}

int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_call(__init, __fini, __main, argc, argv);
}

/******************************************************************************/
