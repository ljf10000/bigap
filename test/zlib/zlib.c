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
    byte input_uncompress0[] = {120,1,1,14,0,241,255,104,101,108,108,111,44,32,119,111,114,108,100,10,0,38,122,4,14};
    byte input_uncompress1[] = {120,1,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress2[] = {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress3[] = {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress4[] = {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress5[] = {120,94,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress6[] = {120,156,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress7[] = {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress8[] = {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};
    byte input_uncompress9[] = {120,218,203,72,205,201,201,215,81,40,207,47,202,73,225,98,0,0,38,122,4,147};

    char output_compress[1+OS_LINE_LEN] = {0};
    char output_uncompress[1+OS_LINE_LEN] = {0};
    int err = 0;
    int output_compress_len = OS_LINE_LEN;
    int output_uncompress_len = OS_LINE_LEN;

    os_println("before compress  [level=%d]:%s", 0, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 0);
    output("after    compress", 0, output_compress, output_compress_len);
    
    output("before uncompress", 0, input_uncompress0, os_count_of(input_uncompress0));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress0, os_count_of(input_uncompress0));
    os_println("after  uncompress[level=%d]:%s", 0, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 1, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 1);
    output("after    compress", 1, output_compress, output_compress_len);
    
    output("before uncompress", 1, input_uncompress1, os_count_of(input_uncompress1));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress1, os_count_of(input_uncompress1));
    os_println("after  uncompress[level=%d]:%s", 1, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 2, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 2);
    output("after    compress", 2, output_compress, output_compress_len);
    
    output("before uncompress", 2, input_uncompress2, os_count_of(input_uncompress2));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress2, os_count_of(input_uncompress2));
    os_println("after  uncompress[level=%d]:%s", 2, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 3, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 3);
    output("after    compress", 3, output_compress, output_compress_len);
    
    output("before uncompress", 3, input_uncompress3, os_count_of(input_uncompress3));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress3, os_count_of(input_uncompress3));
    os_println("after  uncompress[level=%d]:%s", 3, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 4, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 4);
    output("after    compress", 4, output_compress, output_compress_len);
    
    output("before uncompress", 4, input_uncompress4, os_count_of(input_uncompress4));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress4, os_count_of(input_uncompress4));
    os_println("after  uncompress[level=%d]:%s", 4, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 5, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 5);
    output("after    compress", 5, output_compress, output_compress_len);
    
    output("before uncompress", 5, input_uncompress5, os_count_of(input_uncompress5));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress5, os_count_of(input_uncompress5));
    os_println("after  uncompress[level=%d]:%s", 5, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 6, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 6);
    output("after    compress", 6, output_compress, output_compress_len);
    
    output("before uncompress", 6, input_uncompress6, os_count_of(input_uncompress6));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress6, os_count_of(input_uncompress6));
    os_println("after  uncompress[level=%d]:%s", 6, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 7, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 7);
    output("after    compress", 7, output_compress, output_compress_len);
    
    output("before uncompress", 7, input_uncompress7, os_count_of(input_uncompress7));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress7, os_count_of(input_uncompress7));
    os_println("after  uncompress[level=%d]:%s", 7, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 8, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 8);
    output("after    compress", 8, output_compress, output_compress_len);
    
    output("before uncompress", 8, input_uncompress8, os_count_of(input_uncompress8));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress8, os_count_of(input_uncompress8));
    os_println("after  uncompress[level=%d]:%s", 8, output_uncompress, output_uncompress_len);
    os_println("-------------------------");

    os_println("before compress  [level=%d]:%s", 9, input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_count_of(input_compress), 9);
    output("after    compress", 9, output_compress, output_compress_len);
    
    output("before uncompress", 9, input_uncompress9, os_count_of(input_uncompress9));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress9, os_count_of(input_uncompress9));
    os_println("after  uncompress[level=%d]:%s", 9, output_uncompress, output_uncompress_len);
    os_println("-------------------------");
        
        
    return 0;
}


int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
