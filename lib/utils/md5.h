#ifndef __MD5_H_46af656ba80c4a0c9dc59106b11551a0__
#define __MD5_H_46af656ba80c4a0c9dc59106b11551a0__
/******************************************************************************/
// https://github.com/leimingshan/MD5
enum { OS_MD5_SIZE = 16 };
enum { OS_MD5_STRING_LEN = 2*OS_MD5_SIZE };

/* MD5 ctx. */
typedef struct {
	uint32 state[4];	/* state (ABCD) */
	uint32 count[2];	/* number of bits, modulo 2^64 (lsb first) */
	byte buffer[64];	/* input buffer */
} md5_content_t;

/* Constants for __md5_transfrom routine.
 */
#define MD5_S11 7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21 5
#define MD5_S22 9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31 4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41 6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

/* MD5_F, MD5_G, MD5_H and MD5_I are basic MD5 functions.
 */
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

/* MD5_ROTATE_LEFT rotates x left n bits.
 */
#define MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* MD5_FF, MD5_GG, MD5_HH, and MD5_II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define MD5_FF(a, b, c, d, x, s, ac) { \
 (a) += MD5_F ((b), (c), (d)) + (x) + (uint32)(ac); \
 (a) = MD5_ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define MD5_GG(a, b, c, d, x, s, ac) { \
 (a) += MD5_G ((b), (c), (d)) + (x) + (uint32)(ac); \
 (a) = MD5_ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define MD5_HH(a, b, c, d, x, s, ac) { \
 (a) += MD5_H ((b), (c), (d)) + (x) + (uint32)(ac); \
 (a) = MD5_ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}
#define MD5_II(a, b, c, d, x, s, ac) { \
 (a) += MD5_I ((b), (c), (d)) + (x) + (uint32)(ac); \
 (a) = MD5_ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
}


/* MD5 initialization. Begins an MD5 operation, writing a new ctx.
 */
STATIC INLINE void
md5_init(md5_content_t *ctx);

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  ctx.
 */
STATIC INLINE void
md5_update(md5_content_t *ctx, byte *input, uint32 inputLen);

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the ctx.
 */
STATIC INLINE void
md5_fini(md5_content_t *ctx, byte md5[16]);

STATIC INLINE void
md5_encode(byte md5[OS_MD5_SIZE], void *buf, uint32 len);

static inline bool
md5_eq(byte a[OS_MD5_SIZE], byte b[OS_MD5_SIZE])
{
    return os_memeq(a, b, OS_MD5_SIZE);
}

STATIC INLINE int
md5_file(char *filename, byte md5[OS_MD5_SIZE]);

static inline void
md5_hex2bin(char *hex, byte md5[OS_MD5_SIZE])
{
    os_hex2bin(hex, md5, OS_MD5_SIZE);
}

static inline void
md5_bin2hex(char *hex, byte md5[OS_MD5_SIZE])
{
    os_bin2hex(hex, OS_MD5_STRING_LEN, md5, OS_MD5_SIZE);
}

#define MD5_SZERO   SZERO32
/******************************************************************************/
#endif /* __MD5_H_46af656ba80c4a0c9dc59106b11551a0__ */
