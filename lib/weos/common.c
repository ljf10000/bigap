/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifdef __APP__

int
envs_count(char *env[])
{
    int count;

    if (NULL==env) {
        return 0;
    }
    
    for (count=0; env[count]; count++) {
        os_do_nothing();
    }

    return count;
}

void
envs_append(char *dst[], char *src[])
{
    int i, count = envs_count(dst);

    for (i=0; src[i]; i++) {
        dst[i+count] = src[i];
    }
}

char *
env_gets(char *envname, char *deft) 
{
    if (envname) {
        char *env = getenv(envname);
        
        if (is_good_env(env)) {
            env_println("get env:%s=%s", envname, env);
            
            return env;
        } else {
            env_println("no-found env:%s, use default:%s", envname, deft);

            return deft;
        }
    } else {
        env_println("empty env, use default:%s", deft);

        return deft;
    }
}

int
__env_copy(char *envname, char *deft, char s[], int size) 
{
    char *env = env_gets(envname, deft);
    if (NULL==env) {
        return 0;
    }
    else if (os_strlen(env) > size - 1) {
        return os_assertV(-ETOOBIG);
    }
    
    os_strlcpy(s, env, size);
    
    return 0;
}

int
env_geti(char *envname, int deft) 
{
    if (NULL==envname) {
        return os_assertV(deft);
    }
    
    char *env = getenv(envname);
    if (false==is_good_env(env)) {
        env_println("no-found env:%s, use default:%d", envname, deft);
        
        return deft;
    } else {
        int value = os_atoi(env);

        env_println("get env:%s=%d", envname, value);

        return value;
    }
}

char *
os_getmacby(char *script)
{
    static char line[1+OS_LINE_LEN];

    if (0==line[0]) {
        int err = os_pgets(line, OS_LINE_LEN, script);
        if (err) {
            return NULL;
        }
        else if (false==is_good_macstring(line)) {
            return NULL;
        }
    }

    return line;
}
#elif defined(__BOOT__)
#ifndef BOOT_SNPRINTF_BUFSIZE
#define BOOT_SNPRINTF_BUFSIZE   4096
#endif

int
os_vsnprintf(char *buf, int size, const char *fmt, va_list args)
{
    int bsize = size?size-1:0;
    int len = 0;
    
    char *p = (char *)os_zalloc(BOOT_SNPRINTF_BUFSIZE);
    if (NULL==p) {
        return -ENOMEM;
    }

    len = os_vsprintf(p, fmt, args);

    if (len <= bsize) {
        strcpy(buf, p);
    } else {
        len = bsize;
        os_memcpy(buf, p, len);
    }

error:
    os_free(p);

    return len;
}

int
os_snprintf(char *buf, int size, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vsnprintf(buf, size, fmt, args);
    va_end(args);

    return len;
}

int
os_vasprintf(char **buf, const char *fmt, va_list args)
{
    int len = 0;
    
    char *p = (char *)os_zalloc(BOOT_SNPRINTF_BUFSIZE);
    if (NULL==p) {
        return -ENOMEM;
    }

    len = os_vsprintf(p, fmt, args);
    *buf = p;

    return len;
}

int
os_asprintf(char **buf, const char *fmt, ...)
{
    va_list args;

    va_start(args, (char *)fmt);
    int len = os_vasprintf(buf, fmt, args);
    va_end(args);

    return len;
}
#endif

uint32 
__crc32(uint32 crc_in, const void *buf, uint32 size) 
{
    static uint32 table[256] = {
        0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,
        0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,
        0xE7B82D07,0x90BF1D91,0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,
        0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,
        0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,0x3B6E20C8,0x4C69105E,0xD56041E4,
        0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,
        0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,0x26D930AC,
        0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
        0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,
        0xB6662D3D,0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,
        0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,
        0x086D3D2D,0x91646C97,0xE6635C01,0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,
        0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,
        0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,0x4DB26158,0x3AB551CE,
        0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,
        0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
        0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,
        0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,
        0xB7BD5C3B,0xC0BA6CAD,0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,
        0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,
        0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,0xF00F9344,0x8708A3D2,0x1E01F268,
        0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,
        0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,0xD6D6A3E8,
        0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
        0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,
        0x4669BE79,0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,
        0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,
        0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,
        0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,
        0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,0x86D3D2D4,0xF1D4E242,
        0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,
        0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
        0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,
        0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,
        0x47B2CF7F,0x30B5FFE9,0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,
        0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,
        0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
    };
    byte *B = (byte *) buf;
    uint32 crc = crc_in ^ 0xFFFFFFFF;
    uint32 i;
    
    /** accumulate crc32 for buffer **/
    for (i = 0; i < size; i++) {
        crc = (crc >> 8) ^ table[(crc ^ B[i]) & 0xFF];
    }
    
    return crc ^ 0xFFFFFFFF;
}

uint32
os_crc32(const void *buf, uint32 size)
{
    return __crc32(0, buf, size);
}

char
b64_table(int v)
{
    static const char table[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };

    return table[v];
}

char *
b64_encode(byte *src, size_t len)
{
	int i = 0;
	int j = 0;
	char *enc = NULL;
	size_t size = 0;
	byte buf[4];
	byte tmp[3];

	// alloc
	enc = (char *)os_malloc(0);
	if (NULL == enc) {
		return NULL;
	}
	// parse until end of source
	while (len--) {
		// read up to 3 bytes at a time into `tmp'
		tmp[i++] = *(src++);

		// if 3 bytes read then encode into `buf'
		if (3 == i) {
			buf[0] = (tmp[0] & 0xfc) >> 2;
			buf[1] =
			    ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
			buf[2] =
			    ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
			buf[3] = tmp[2] & 0x3f;

			// allocate 4 new byts for `enc` and
			// then translate each encoded buffer
			// part by index from the base 64 index table
			// into `enc' byte array
			enc = (char *)os_realloc(enc, size + 4);
			for (i = 0; i < 4; ++i) {
				enc[size++] = b64_table(buf[i]);
			}

			// reset index
			i = 0;
		}
	}

	// remainder
	if (i > 0) {
		// fill `tmp' with `\0' at most 3 times
		for (j = i; j < 3; ++j) {
			tmp[j] = '\0';
		}

		// perform same codec as above
		buf[0] = (tmp[0] & 0xfc) >> 2;
		buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
		buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
		buf[3] = tmp[2] & 0x3f;

		// perform same write to `enc` with new allocation
		for (j = 0; (j < i + 1); ++j) {
			enc = (char *)os_realloc(enc, size + 1);
			enc[size++] = b64_table(buf[j]);
		}

		// while there is still a remainder
		// append `=' to `enc'
		while ((i++ < 3)) {
			enc = (char *)os_realloc(enc, size + 1);
			enc[size++] = '=';
		}
	}
	// Make sure we have enough space to add '\0' character at end.
	enc = (char *)os_realloc(enc, size + 1);
	enc[size] = '\0';

	return enc;
}

char *
b64_decode_ex(byte *src, size_t len, size_t *decsize)
{
	int i = 0;
	int j = 0;
	int l = 0;
	size_t size = 0;
	char *dec = NULL;
	char buf[3];
	char tmp[4];

	// alloc
	dec = (char *)os_malloc(0);
	if (NULL == dec) {
		return NULL;
	}
	// parse until end of source
	while (len--) {
		// break if char is `=' or not base64 char
		if ('=' == src[j]) {
			break;
		}
		if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) {
			break;
		}
		// read up to 4 bytes at a time into `tmp'
		tmp[i++] = src[j++];

		// if 4 bytes read then decode into `buf'
		if (4 == i) {
			// translate values in `tmp' from table
			for (i = 0; i < 4; ++i) {
				// find translation char in `b64_table'
				for (l = 0; l < 64; ++l) {
					if (tmp[i] == b64_table(l)) {
						tmp[i] = l;
						break;
					}
				}
			}

			// decode
			buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
			buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
			buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

			// write decoded buffer to `dec'
			dec = (char *)os_realloc(dec, size + 3);
			for (i = 0; i < 3; ++i) {
				dec[size++] = buf[i];
			}

			// reset
			i = 0;
		}
	}

	// remainder
	if (i > 0) {
		// fill `tmp' with `\0' at most 4 times
		for (j = i; j < 4; ++j) {
			tmp[j] = '\0';
		}

		// translate remainder
		for (j = 0; j < 4; ++j) {
			// find translation char in `b64_table'
			for (l = 0; l < 64; ++l) {
				if (tmp[j] == b64_table(l)) {
					tmp[j] = l;
					break;
				}
			}
		}

		// decode remainder
		buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
		buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
		buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

		// write remainer decoded buffer to `dec'
		dec = (char *)os_realloc(dec, size + (i - 1));
		for (j = 0; (j < i - 1); ++j) {
			dec[size++] = buf[j];
		}
	}
	// Make sure we have enough space to add '\0' character at end.
	dec = (char *)os_realloc(dec, size + 1);
	dec[size] = '\0';

	// Return back the size of decoded string if demanded.
	if (decsize != NULL)
		*decsize = size;

	return dec;
}

/* Encodes input (uint32) into output (byte). Assumes len is
  a multiple of 4.
 */
STATIC void
__md5_encode(byte *output, uint32 *input, uint32 len)
{
	uint32 i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (byte)(input[i] & 0xff);
		output[j + 1] = (byte)((input[i] >> 8) & 0xff);
		output[j + 2] = (byte)((input[i] >> 16) & 0xff);
		output[j + 3] = (byte)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (byte) into output (uint32). Assumes len is
  a multiple of 4.
 */
STATIC void
__md5_decode(uint32 *output, byte *input, uint32 len)
{
	uint32 i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] =
		    ((uint32) input[j]) | (((uint32) input[j + 1]) << 8) |
		    (((uint32) input[j + 2]) << 16) | (((uint32) input[j + 3])
						       << 24);
}

/* MD5 basic transformation. Transforms state based on block.
 */
STATIC void
__md5_transfrom(uint32 state[4], byte block[64])
{
	uint32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	__md5_decode(x, block, 64);

	/* Round 1 */
	MD5_FF(a, b, c, d, x[0], MD5_S11, 0xd76aa478);	/* 1 */
	MD5_FF(d, a, b, c, x[1], MD5_S12, 0xe8c7b756);	/* 2 */
	MD5_FF(c, d, a, b, x[2], MD5_S13, 0x242070db);	/* 3 */
	MD5_FF(b, c, d, a, x[3], MD5_S14, 0xc1bdceee);	/* 4 */
	MD5_FF(a, b, c, d, x[4], MD5_S11, 0xf57c0faf);	/* 5 */
	MD5_FF(d, a, b, c, x[5], MD5_S12, 0x4787c62a);	/* 6 */
	MD5_FF(c, d, a, b, x[6], MD5_S13, 0xa8304613);	/* 7 */
	MD5_FF(b, c, d, a, x[7], MD5_S14, 0xfd469501);	/* 8 */
	MD5_FF(a, b, c, d, x[8], MD5_S11, 0x698098d8);	/* 9 */
	MD5_FF(d, a, b, c, x[9], MD5_S12, 0x8b44f7af);	/* 10 */
	MD5_FF(c, d, a, b, x[10], MD5_S13, 0xffff5bb1);	/* 11 */
	MD5_FF(b, c, d, a, x[11], MD5_S14, 0x895cd7be);	/* 12 */
	MD5_FF(a, b, c, d, x[12], MD5_S11, 0x6b901122);	/* 13 */
	MD5_FF(d, a, b, c, x[13], MD5_S12, 0xfd987193);	/* 14 */
	MD5_FF(c, d, a, b, x[14], MD5_S13, 0xa679438e);	/* 15 */
	MD5_FF(b, c, d, a, x[15], MD5_S14, 0x49b40821);	/* 16 */

	/* Round 2 */
	MD5_GG(a, b, c, d, x[1], MD5_S21, 0xf61e2562);	/* 17 */
	MD5_GG(d, a, b, c, x[6], MD5_S22, 0xc040b340);	/* 18 */
	MD5_GG(c, d, a, b, x[11], MD5_S23, 0x265e5a51);	/* 19 */
	MD5_GG(b, c, d, a, x[0], MD5_S24, 0xe9b6c7aa);	/* 20 */
	MD5_GG(a, b, c, d, x[5], MD5_S21, 0xd62f105d);	/* 21 */
	MD5_GG(d, a, b, c, x[10], MD5_S22, 0x2441453);	/* 22 */
	MD5_GG(c, d, a, b, x[15], MD5_S23, 0xd8a1e681);	/* 23 */
	MD5_GG(b, c, d, a, x[4], MD5_S24, 0xe7d3fbc8);	/* 24 */
	MD5_GG(a, b, c, d, x[9], MD5_S21, 0x21e1cde6);	/* 25 */
	MD5_GG(d, a, b, c, x[14], MD5_S22, 0xc33707d6);	/* 26 */
	MD5_GG(c, d, a, b, x[3], MD5_S23, 0xf4d50d87);	/* 27 */
	MD5_GG(b, c, d, a, x[8], MD5_S24, 0x455a14ed);	/* 28 */
	MD5_GG(a, b, c, d, x[13], MD5_S21, 0xa9e3e905);	/* 29 */
	MD5_GG(d, a, b, c, x[2], MD5_S22, 0xfcefa3f8);	/* 30 */
	MD5_GG(c, d, a, b, x[7], MD5_S23, 0x676f02d9);	/* 31 */
	MD5_GG(b, c, d, a, x[12], MD5_S24, 0x8d2a4c8a);	/* 32 */

	/* Round 3 */
	MD5_HH(a, b, c, d, x[5], MD5_S31, 0xfffa3942);	/* 33 */
	MD5_HH(d, a, b, c, x[8], MD5_S32, 0x8771f681);	/* 34 */
	MD5_HH(c, d, a, b, x[11], MD5_S33, 0x6d9d6122);	/* 35 */
	MD5_HH(b, c, d, a, x[14], MD5_S34, 0xfde5380c);	/* 36 */
	MD5_HH(a, b, c, d, x[1], MD5_S31, 0xa4beea44);	/* 37 */
	MD5_HH(d, a, b, c, x[4], MD5_S32, 0x4bdecfa9);	/* 38 */
	MD5_HH(c, d, a, b, x[7], MD5_S33, 0xf6bb4b60);	/* 39 */
	MD5_HH(b, c, d, a, x[10], MD5_S34, 0xbebfbc70);	/* 40 */
	MD5_HH(a, b, c, d, x[13], MD5_S31, 0x289b7ec6);	/* 41 */
	MD5_HH(d, a, b, c, x[0], MD5_S32, 0xeaa127fa);	/* 42 */
	MD5_HH(c, d, a, b, x[3], MD5_S33, 0xd4ef3085);	/* 43 */
	MD5_HH(b, c, d, a, x[6], MD5_S34, 0x4881d05);	/* 44 */
	MD5_HH(a, b, c, d, x[9], MD5_S31, 0xd9d4d039);	/* 45 */
	MD5_HH(d, a, b, c, x[12], MD5_S32, 0xe6db99e5);	/* 46 */
	MD5_HH(c, d, a, b, x[15], MD5_S33, 0x1fa27cf8);	/* 47 */
	MD5_HH(b, c, d, a, x[2], MD5_S34, 0xc4ac5665);	/* 48 */

	/* Round 4 */
	MD5_II(a, b, c, d, x[0], MD5_S41, 0xf4292244);	/* 49 */
	MD5_II(d, a, b, c, x[7], MD5_S42, 0x432aff97);	/* 50 */
	MD5_II(c, d, a, b, x[14], MD5_S43, 0xab9423a7);	/* 51 */
	MD5_II(b, c, d, a, x[5], MD5_S44, 0xfc93a039);	/* 52 */
	MD5_II(a, b, c, d, x[12], MD5_S41, 0x655b59c3);	/* 53 */
	MD5_II(d, a, b, c, x[3], MD5_S42, 0x8f0ccc92);	/* 54 */
	MD5_II(c, d, a, b, x[10], MD5_S43, 0xffeff47d);	/* 55 */
	MD5_II(b, c, d, a, x[1], MD5_S44, 0x85845dd1);	/* 56 */
	MD5_II(a, b, c, d, x[8], MD5_S41, 0x6fa87e4f);	/* 57 */
	MD5_II(d, a, b, c, x[15], MD5_S42, 0xfe2ce6e0);	/* 58 */
	MD5_II(c, d, a, b, x[6], MD5_S43, 0xa3014314);	/* 59 */
	MD5_II(b, c, d, a, x[13], MD5_S44, 0x4e0811a1);	/* 60 */
	MD5_II(a, b, c, d, x[4], MD5_S41, 0xf7537e82);	/* 61 */
	MD5_II(d, a, b, c, x[11], MD5_S42, 0xbd3af235);	/* 62 */
	MD5_II(c, d, a, b, x[2], MD5_S43, 0x2ad7d2bb);	/* 63 */
	MD5_II(b, c, d, a, x[9], MD5_S44, 0xeb86d391);	/* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	os_memset((byte *) x, 0, sizeof(x));
}

void
md5_init(md5_content_t *ctx)
{				/* ctx */
	ctx->count[0] = ctx->count[1] = 0;
	/* Load magic initialization constants. */
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
}

void
md5_update(md5_content_t *ctx, byte *input, uint32 inputLen)
{
	uint32 i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (uint32)((ctx->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((ctx->count[0] +=
	     ((uint32) inputLen << 3)) < ((uint32) inputLen << 3))
		ctx->count[1]++;
	ctx->count[1] += ((uint32) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		os_memcpy((byte *) & ctx->buffer[index], (byte *) input,
			   partLen);
		__md5_transfrom(ctx->state, ctx->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			__md5_transfrom(ctx->state, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	os_memcpy((byte *) & ctx->buffer[index], (byte *) & input[i],
		   inputLen - i);
}

void
md5_fini(md5_content_t *ctx, byte md5[16])
{
	byte bits[8];
	uint32 index, padLen;
    
    static byte padding[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

	/* Save number of bits */
	__md5_encode(bits, ctx->count, 8);

	/* Pad out to 56 mod 64. */
	index = (uint32)((ctx->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	md5_update(ctx, padding, padLen);

	/* Append length (before padding) */
	md5_update(ctx, bits, 8);
	/* Store state in digest */
	__md5_encode(md5, ctx->state, 16);

	/* Zeroize sensitive information. */
	os_memset((byte *) ctx, 0, sizeof(*ctx));
}

void
md5_encode(byte md5[OS_MD5_SIZE], void *buf, uint32 len)
{
    md5_content_t ctx;
    
    md5_init(&ctx);
    md5_update(&ctx, buf, len);
    md5_fini(&ctx, md5);
}

int
md5_file(char *filename, byte md5[OS_MD5_SIZE])
{
#ifdef __APP__
    void *content = NULL;
    md5_content_t ctx;
    int err, len;
    
    len = os_fsize(filename);
    if (len<0) {
        return -EBADFILE;
    }

    content = os_malloc(len);
    if (content) {
        return -ENOMEM;
    }

    err = os_fgetb(content, len, filename);
    if (err<0) {
        goto error;
    }

    md5_encode(md5, content, len);
error:
    os_free(content);

    return err;
#else
    return -ENOSUPPORT;
#endif
}

STATIC void *
__os_aa_item(autoarray_t *aa, uint32 idx)
{
    if (is_good_enum(idx, aa->count)) {
        return (void *)(aa->base + idx * aa->size);
    } else {
        return NULL;
    }
}

STATIC int
__os_aa_grow_to(autoarray_t *aa, uint32 count)
{
    int old  = aa->count;
    int grow = count - old;
    
    if (grow <= 0) {
        return 0; //needn't grow
    }
    
    if (aa->limit && count > aa->limit) {
        return -ESPIPE;
    }
    
    aa->base = (byte *)os_realloc(aa->base, aa->size * count);
    if (NULL==aa->base) {
        return -ENOMEM;
    }
    aa->count = count;
    
    if (aa->init) {
        int i;
        
        for (i=old; i<count; i++) {
            (*aa->init)(__os_aa_item(aa, i));
        }
    } else {
        os_memzero(__os_aa_item(aa, old), aa->size * grow);
    }
    
    return 0;
}

STATIC int
__os_aa_grow(autoarray_t *aa)
{
    int count;
    
    if (NULL==aa) {
        return error_assertV(-EKEYNULL, "$aa is nil");
    }

    count = aa->count + aa->grow;
    if (aa->limit) {
        if (aa->count==aa->limit) {
            return -ESPIPE;
        }

        if (count > aa->limit) {
            count = aa->limit;
        }
    }

    return __os_aa_grow_to(aa, count);
}

void
os_aa_clean(autoarray_t *aa)
{
    if (aa) {
        if (aa->clean) {
            int i;
            
            for (i=0; i<aa->count; i++) {
                (*aa->clean)(__os_aa_item(aa, i));
            }
        }

        if (aa->base) {
            os_free(aa->base);
        }
    }
}

int
os_aa_init(
    autoarray_t *aa, 
    uint32 size, 
    uint32 count,
    uint32 limit, 
    uint32 grow, 
    void (*init)(void *item),
    void (*clean)(void *item)
)
{
    if (NULL==aa) {
        return error_assertV(-EKEYNULL, "aa is nil");
    }
    else if (size<=0) {
        return error_assertV(-EINVAL1, "size(%d)<=0", size);
    }
    else if (count<=0) {
        return os_assertV(-EINVAL2);
    }
    else if (limit<0) {
        return os_assertV(-EINVAL3);
    }
    else if (limit && count > limit) {
        return os_assertV(-EINVAL4);
    }
    else if (grow<0) {
        return os_assertV(-EINVAL5);
    }
    
    aa->size    = size;
    aa->count   = count;
    aa->limit   = limit;
    aa->grow    = grow;
    aa->init    = init;
    aa->clean   = clean;
    
    aa->base = (byte *)os_calloc(aa->count, aa->size);
    if (NULL==aa->base) {
        return -ENOMEM;
    }
    
    if (aa->init) {
        int i;
        
        for (i=0; i<aa->count; i++) {
            (*aa->init)(__os_aa_item(aa, i));
        }
    }
    
    return 0;
}

void *
os_aa_get(autoarray_t *aa, int idx, bool grow)
{
    void *value;
    
    if (NULL==aa) {
        return os_assertV(NULL);
    }
    
    if (idx<0) {
        return os_assertV(NULL);
    }
    
    value = __os_aa_item(aa, idx);
    if (NULL==value /* no space */ 
        && grow     /* try grow */ 
        && aa->grow /* can grow */ 
        && 0==__os_aa_grow_to(aa, idx) /* grow ok */
        ) {
        return __os_aa_item(aa, idx);
    }
    
    return value;
}

void
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

void
__os_dump_buffer(void *buffer, int len, os_dump_line_f *dump_line)
{
    int i, line, tail;
    byte *raw = (byte *)buffer;
    
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

STATIC bool
__is_good_macstring_L(char *macstring)
{
    /*
    * long  macstring is "XX:XX:XX:XX:XX:XX" or "XX-XX-XX-XX-XX-XX"
    */
    int ifs = macstring[2];
    
    return (':'==ifs || '-'==ifs)
        && macstring[5] ==ifs
        && macstring[8] ==ifs
        && macstring[11]==ifs
        && macstring[14]==ifs
        && __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        /* macstring[2] is ifs */
        && __is_good_macchar(macstring[3])
        && __is_good_macchar(macstring[4])
        /* macstring[5] is ifs */
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        /* macstring[8] is ifs */
        && __is_good_macchar(macstring[9])
        && __is_good_macchar(macstring[10])
        /* macstring[11] is ifs */
        && __is_good_macchar(macstring[12])
        && __is_good_macchar(macstring[13])
        /* macstring[14] is ifs */
        && __is_good_macchar(macstring[15])
        && __is_good_macchar(macstring[16]);
}

STATIC bool
__is_good_macstring_M(char *macstring)
{
    /*
    * middle  macstring is "XXXX:XXXX:XXXX" or "XXXX-XXXX-XXXX"
    */
    int ifs = macstring[4];
    
    return (':'==ifs || '-'==ifs)
        && macstring[9] ==ifs
        && __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        && __is_good_macchar(macstring[2])
        && __is_good_macchar(macstring[3])
        /* macstring[4] is ifs */
        && __is_good_macchar(macstring[5])
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        && __is_good_macchar(macstring[8])
        /* macstring[9] is ifs */
        && __is_good_macchar(macstring[10])
        && __is_good_macchar(macstring[11])
        && __is_good_macchar(macstring[12])
        && __is_good_macchar(macstring[13]);
}

STATIC bool
__is_good_macstring_S(char *macstring)
{
    /*
    * short macstring is "XXXXXXXXXXXX"
    */
    return __is_good_macchar(macstring[0])
        && __is_good_macchar(macstring[1])
        && __is_good_macchar(macstring[2])
        && __is_good_macchar(macstring[3])
        && __is_good_macchar(macstring[4])
        && __is_good_macchar(macstring[5])
        && __is_good_macchar(macstring[6])
        && __is_good_macchar(macstring[7])
        && __is_good_macchar(macstring[8])
        && __is_good_macchar(macstring[9])
        && __is_good_macchar(macstring[10])
        && __is_good_macchar(macstring[11]);

}

bool
__is_good_macstring(char *macstring)
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __is_good_macstring_L(macstring);
        case MACSTRINGLEN_M: return __is_good_macstring_M(macstring);
        case MACSTRINGLEN_S: return __is_good_macstring_S(macstring);
        default: return false;
    }
}

STATIC byte *
__os_getmac_bystring_L(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 3*i, 2, 16, int);
    }

    return mac;
}

STATIC byte *
__os_getmac_bystring_M(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE/2; i++) {
        mac[i]  = os_hex2number(macstring + 5*i,     2, 16, int);
        mac[i+1]= os_hex2number(macstring + 5*i + 2, 2, 16, int);
    }

    return mac;
}

STATIC byte *
__os_getmac_bystring_S(byte mac[], char macstring[])
{
    int i;
    
    for (i=0; i<OS_MACSIZE; i++) {
        mac[i] = os_hex2number(macstring + 2*i, 2, 16, int);
    }

    return mac;
}

byte *
__os_getmac_bystring(byte mac[], char macstring[])
{
    switch(os_strlen(macstring)) {
        case MACSTRINGLEN_L: return __os_getmac_bystring_L(mac, macstring);
        case MACSTRINGLEN_M: return __os_getmac_bystring_M(mac, macstring);
        case MACSTRINGLEN_S: return __os_getmac_bystring_S(mac, macstring);
        default: return OS_2BMAC;
    }
}

byte *
os_getmac_bystring(byte mac[], char macstring[])
{
    byte tmp[6];
    
    if (__is_good_macstring(macstring) && 
        __os_getmac_bystring(tmp, macstring) &&
        is_good_mac(tmp)) {
        return os_maccpy(mac, tmp);
    } else {
        return NULL;
    }
}

STATIC int
__os_macsnprintf_L(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x"  "%c"
        "%.2x",
        mac[0], sep,
        mac[1], sep,
        mac[2], sep,
        mac[3], sep,
        mac[4], sep,
        mac[5]);
}

STATIC int
__os_macsnprintf_M(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x%.2x"  "%c"
        "%.2x%.2x"  "%c"
        "%.2x%.2x",
        mac[0], mac[1], sep,
        mac[2], mac[3], sep,
        mac[4], mac[5]);
}

STATIC int
__os_macsnprintf_S(byte mac[], char macstring[], int size, int sep)
{
    return os_snprintf(macstring, size,
        "%.2x%.2x"
        "%.2x%.2x"
        "%.2x%.2x",
        mac[0], mac[1],
        mac[2], mac[3],
        mac[4], mac[5]);
}

int
os_macsnprintf(byte mac[], char macstring[], int size, int type, int sep)
{
    switch(type) {
        case MACSTRINGLEN_L: return __os_macsnprintf_L(mac, macstring, size, sep);
        case MACSTRINGLEN_M: return __os_macsnprintf_M(mac, macstring, size, sep);
        case MACSTRINGLEN_S: default: return __os_macsnprintf_S(mac, macstring, size, sep);
    }
}

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

