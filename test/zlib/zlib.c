/******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      zlib
#endif

#include "utils.h"
#include "zlib.h"

OS_INITER;

static void
output(char *tag, byte *buf, int len)
{
    int i;

    os_printf("%s:%d:", tag, len);
    for (i=0; i<len; i++) {
        os_printf("%d ", buf[i]);
    }
    os_println(__empty);
}

int __main(int argc, char *argv[])
{
    char input_compress[] = "hello, world\n";
    byte input_uncompress[] = {
        120, 156, 202, 72, 205, 201, 201, 215, 81, 40, 207,
		47, 202, 73, 225, 2, 4, 0, 0, 255, 255, 33, 231, 4, 147};
    char output_compress[1+OS_LINE_LEN] = {0};
    char output_uncompress[1+OS_LINE_LEN] = {0};
    int err = 0;
    int output_compress_len = OS_LINE_LEN;
    int output_uncompress_len = OS_LINE_LEN;

    os_println("before compress:%s", input_compress);
    err = compress2(output_compress, &output_compress_len, input_compress, os_strlen(input_compress), Z_BEST_SPEED);
    output("after compress", output_compress, output_compress_len);

    output("before uncompress", input_uncompress, os_count_of(input_uncompress));
    err = uncompress(output_uncompress, &output_uncompress_len, input_uncompress, os_count_of(input_uncompress));
    os_println("after uncompress:%s", output_uncompress, output_uncompress_len);
    
    return 0;
}


int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
