#ifndef __DUMP_H_96bec2f065cb4415a8327409eae6d67c__
#define __DUMP_H_96bec2f065cb4415a8327409eae6d67c__
/******************************************************************************/
/*
*   raw format(like UltraEdit)

      :                                     ;
 Line :       Hexadecimal Content           ; Raw Content
      : 0 1 2 3  4 5 6 7  8 9 A B  C D E F  ;
      :                                     ;
xxxxH : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; cccccccccccccccc
xxxxH : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; cccccccccccccccc
xxxxH : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; cccccccccccccccc
xxxxH : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; cccccccccccccccc
xxxxH : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; cccccccccccccccc
xxxxH : xxxxxxxx xxxxxxxx xxxxxx            ; ccccccccccc
*/

#ifndef __DUMP_LINE_BLOCK
#define __DUMP_LINE_BLOCK           4
#endif

#ifndef __DUMP_LINE_BLOCK_BYTES
#define __DUMP_LINE_BLOCK_BYTES     4
#endif

#ifndef __DUMP_LINE_LIMIT
#define __DUMP_LINE_LIMIT           80
#endif

enum {
    __DUMP_LINE_BYTES   = __DUMP_LINE_BLOCK_BYTES * __DUMP_LINE_BLOCK,
    __DUMP_LINE_MAX     =  (0
                            + 8 /* "xxxxH : " */
                            + (2 * __DUMP_LINE_BLOCK_BYTES + 1) * __DUMP_LINE_BLOCK \
                            + 2 /* "; " */
                            + __DUMP_LINE_BYTES
                            + 1 /* "\n" */
                            ),
};

#if __DUMP_LINE_MAX > __DUMP_LINE_LIMIT
#error "must __DUMP_LINE_MAX < __DUMP_LINE_LIMIT"
#endif

#ifndef __DUMP_LINE_SEPARATOR
#define __DUMP_LINE_SEPARATOR \
"=============================================================="
#endif

#ifndef __DUMP_LINE_SEPARATOR_SUB
#define __DUMP_LINE_SEPARATOR_SUB \
"--------------------------------------------------------------"
#endif

#ifndef __DUMP_LINE_HEADER
#define __DUMP_LINE_HEADER \
"      :                                     ;"                 __crlf \
" Line :       Hexadecimal Content           ; Raw Content"     __crlf \
"      : 0 1 2 3  4 5 6 7  8 9 A B  C D E F  ;"                 __crlf \
"      :                                     ;"                 __crlf
#endif

typedef void os_dump_line_f(char *line);

#ifdef OS_DUMP_PRINTF
#define os_dump_printf(_fmt, _args...)  OS_DUMP_PRINTF(_fmt, ##_args)
#else
#define os_dump_printf(_fmt, _args...)  os_printf(_fmt, ##_args)
#endif

EXTERN void
os_dump_line(int line, byte *raw, int len, os_dump_line_f *dump_line);

EXTERN void
__os_dump_buffer(void *buffer, int len, os_dump_line_f *dump_line);

#define os_dump_buffer(_is_dump, _buffer, _len, _dump_line) do{ \
    if (_is_dump) {                                             \
        __os_dump_buffer(_buffer, _len, _dump_line);            \
    }                                                           \
}while(0)
/******************************************************************************/
#endif /* __DUMP_H_96bec2f065cb4415a8327409eae6d67c__ */
