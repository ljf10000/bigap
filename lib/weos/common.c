/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
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

/******************************************************************************/

