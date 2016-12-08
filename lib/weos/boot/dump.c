/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
DECLARE void
os_dump_line(int line, byte *raw, int len, os_dump_line_f *dump_line)
{
    int i, offset = 0;
    char buf[1 + __DUMP_LINE_MAX] = {0};
    
    /*
    * line as 
    *
    * "xxxxH :"
    */
    offset += os_soprintf(buf, offset, "%.4XH :", __DUMP_LINE_BYTES*line);

    /*
    * Hexadecimal Content as
    *
    * " xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx ; "
    */
    for (i=0; i<len; i++) {
        if (0 == (i%__DUMP_LINE_BLOCK_BYTES)) {
            offset += os_soprintf(buf, offset, __space);
        }
        
        offset += os_soprintf(buf, offset, "%.2X", raw[i]);
    }
    
    for (; i<__DUMP_LINE_BYTES; i++) {
        if (0 == (i%__DUMP_LINE_BLOCK_BYTES)) {
            offset += os_soprintf(buf, offset, __space);
        }
        
        offset += os_soprintf(buf, offset, __space __space);
    }
    offset += os_soprintf(buf, offset, " ; ");
    
    /*
    * Raw Content as
    *
    * "cccccccccccccccc"
    */
    for (i=0; i<len; i++) {
        int c = (int)raw[i];
        
        offset += os_soprintf(buf, offset, "%c", os_isprint(c)?c:'.');
    }
    offset += os_soprintf(buf, offset, __crlf);

    if (dump_line) {
        (*dump_line)(buf);
    } else {
        os_dump_printf("%s", buf);
    }
}

DECLARE void
__os_dump_buffer(void *buffer, int len, os_dump_line_f *dump_line)
{
    int i, line, tail;
    byte *raw = (byte *)buffer;

    if (len<0) {
        return;
    }
    
    /*
    * 行数，向上取整 
    */
    line = __OS_ALIGN(len, __DUMP_LINE_BYTES)/__DUMP_LINE_BYTES;
    /*
    * 尾行字节数
    *
    * 需要考虑特殊情况: 
    *   尾行字节数为0, 即 len 正好是 __DUMP_LINE_BYTES 整数倍
    *   则尾行字节数重置为 __DUMP_LINE_BYTES
    */
    tail = len%__DUMP_LINE_BYTES;
    tail = tail?tail:__DUMP_LINE_BYTES;
    
    /*
    * header
    */
    if (dump_line) {
        (*dump_line)(__DUMP_LINE_HEADER);
    } else {
        os_dump_printf(__DUMP_LINE_HEADER);
    }
    
    /*
    * body
    */
    for (i=0; i<(line-1); i++) {
        os_dump_line(i, raw + i * __DUMP_LINE_BYTES, __DUMP_LINE_BYTES, dump_line);
    }
    os_dump_line(line, raw + i * __DUMP_LINE_BYTES, tail, dump_line);
}
/******************************************************************************/
