/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
int
os_chex2int(int ch)
{
    switch(ch) {
        case '0' ... '9':
            return ch - '0';
        case 'a' ... 'f':
            return ch - 'a' + 10;
        case 'A' ... 'F':
            return ch - 'A' + 10;
        default:
            return os_assertV(0);
    }
}

int
os_hex2bin(char *hex, byte *buf, int size)
{
    int i;
    int len = strlen(hex);

    if (len%2) {
        return -EBADHEX;
    }
    else if ((size + size) < len) {
        return -ENOSPACE;
    }

    int hexlen = len/2;
    for (i=0; i<hexlen; i++) {
        buf[i] = 16 * os_chex2int(hex[2*i]) + os_chex2int(hex[2*i+1]);
    }

    return hexlen;
}

int
os_bin2hex(char *hex, int space, byte *buf, int size)
{
    int i, len = size+size;
    
    if (len < space) {
        return -ENOSPACE;
    }

    for (i=0; i<size; i++) {
        os_sprintf(hex + 2*i, "%.2X", buf[i]);
    }
    hex[len] = 0;
    
    return len;
}
/******************************************************************************/
