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

    os_printf("%s:", tag);
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
    char buf_compress[1+OS_LINE_LEN] = {0};
    char buf_uncompress[1+OS_LINE_LEN] = {0};
    int err = 0;
    int len_compress = OS_LINE_LEN;
    int len_uncompress = OS_LINE_LEN;
    
    err = compress(buf_compress, &len_compress, input_compress, os_strlen(input_compress));
    output("after compress", buf_compress, len_compress);

    err = uncompress(buf_uncompress, &len_uncompress, input_uncompress, os_count_of(input_uncompress));
    output("after uncompress", buf_compress, len_compress);
    
    return 0;    
}


int main(int argc, char *argv[])
{
    setup_signal_exit(NULL);
    setup_signal_callstack(NULL);
    
    return os_main(__main, argc, argv);
}
/******************************************************************************/
