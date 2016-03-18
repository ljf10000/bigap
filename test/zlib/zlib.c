/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      zlib
#endif

#include "utils.h"
#include "zlib.h"

OS_INITER;

static void
output(char *tag, int level, byte *buf, int len)
{
    int i;

    os_printf("%s[level=%d]:%d:", tag, level, len);
    for (i=0; i<len; i++) {
        os_printf("%d,", buf[i]);
    }
    os_println(__empty);
}

int __main(int argc, char *argv[])
{
    char input_compress[] = "hello, world\n";
    byte input_uncompress[10] = {
        {120,1,1,14,0,241,255,104,101,108,108,111,44,32,119,111,114,108,100,10,0,38,122,4,14},
        {120,1,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,156,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
        {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147},
    };
    char output_compress[1+OS_LINE_LEN] = {0};
    char output_uncompress[1+OS_LINE_LEN] = {0};
    int err = 0;
    int output_compress_len = OS_LINE_LEN;
    int output_uncompress_len = OS_LINE_LEN;
    int i;

    for (i=Z_NO_COMPRESSION; i<=Z_BEST_COMPRESSION; i++) {
        os_println("before compress[level=%d]:%s", i, input_compress);
        err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), i);
        output("after compress", i, output_compress, output_compress_len);
        
        output("before uncompress", i, input_uncompress[i], os_count_of(input_uncompress[i]));
        err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress[i], os_count_of(input_uncompress[i]));
        os_println("after uncompress[level=%d]:%s", i, output_uncompress, output_uncompress_len);
    }
        
    return 0;
}


int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
