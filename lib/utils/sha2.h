#ifndef __SHA2_H_f482c6dfe05341e7b2ddf4db926acb7d__
#define __SHA2_H_f482c6dfe05341e7b2ddf4db926acb7d__
/******************************************************************************/
// https://github.com/ogay/sha2

#define SHA224_DIGEST_SIZE  (224/8) // 28
#define SHA256_DIGEST_SIZE  (256/8) // 32
#define SHA384_DIGEST_SIZE  (384/8) // 48
#define SHA512_DIGEST_SIZE  (512/8) // 64

#define SHA224_BLOCK_SIZE  SHA256_BLOCK_SIZE    // 64
#define SHA256_BLOCK_SIZE  (512/8)              // 64
#define SHA384_BLOCK_SIZE  SHA512_BLOCK_SIZE    // 128
#define SHA512_BLOCK_SIZE  (1024/8)             // 128

#define SHA224_H_INITER { \
    0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, \
    0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4, \
}
#define SHA256_H_INITER { \
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, \
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, \
}
#define SHA384_H_INITER { \
    0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, \
    0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL, \
    0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL, \
    0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL, \
}
#define SHA512_H_INITER { \
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, \
    0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL, \
    0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, \
    0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL, \
}

#define SHA224_K_INITER SHA256_K_INITER
#define SHA256_K_INITER { \
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, \
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, \
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, \
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, \
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, \
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, \
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, \
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, \
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, \
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, \
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, \
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, \
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, \
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, \
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, \
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2, \
}
#define SHA384_K_INITER SHA512_K_INITER
#define SHA512_K_INITER { \
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, \
    0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, \
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, \
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, \
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, \
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, \
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, \
    0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL, \
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, \
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, \
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, \
    0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, \
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, \
    0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, \
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, \
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, \
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, \
    0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL, \
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, \
    0x81c2c92e47edaee6ULL, 0x92722c851482353bULL, \
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, \
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, \
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, \
    0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, \
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, \
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, \
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, \
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, \
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, \
    0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL, \
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, \
    0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, \
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, \
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, \
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, \
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL, \
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, \
    0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL, \
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, \
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL, \
}

typedef uint32 sha224_uint_t;
typedef uint32 sha256_uint_t;
typedef uint64 sha384_uint_t;
typedef uint64 sha512_uint_t;

#define SHA224_T_SHIFT  6
#define SHA256_T_SHIFT  6
#define SHA384_T_SHIFT  7
#define SHA512_T_SHIFT  7

#define SHA224_K_COUNT  64
#define SHA256_K_COUNT  64
#define SHA384_K_COUNT  80
#define SHA512_K_COUNT  80

#define SHA224_U_SHIFT  6
#define SHA256_U_SHIFT  6
#define SHA384_U_SHIFT  7
#define SHA512_U_SHIFT  7

#define SHA224_F_COUNT  7
#define SHA256_F_COUNT  8
#define SHA384_F_COUNT  6
#define SHA512_F_COUNT  8

#define SHA224_F_SHIFT  6
#define SHA256_F_SHIFT  6
#define SHA384_F_SHIFT  7
#define SHA512_F_SHIFT  7

#define SHA224_F_OFFSET 9
#define SHA256_F_OFFSET 9
#define SHA384_F_OFFSET 17
#define SHA512_F_OFFSET 17

#define SHA_SHIFT(x, n)     ((x) >> (n))
#define SHA_ROTR(x, n)      (((x) >> (n)) | ((x) << ((sizeof(x) << 3) - (n))))
#define SHA_ROTL(x, n)      (((x) << (n)) | ((x) >> ((sizeof(x) << 3) - (n))))
#define SHA_CH(x, y, z)     (((x) & (y)) ^ (~(x) & (z)))
#define SHA_MAJ(x, y, z)    (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define SHA224_F1(x)    SHA256_F1(x)
#define SHA224_F2(x)    SHA256_F2(x)
#define SHA224_F3(x)    SHA256_F3(x)
#define SHA224_F4(x)    SHA256_F4(x)

#define SHA256_F1(x)    (SHA_ROTR(x,  2) ^ SHA_ROTR(x, 13) ^ SHA_ROTR( x, 22))
#define SHA256_F2(x)    (SHA_ROTR(x,  6) ^ SHA_ROTR(x, 11) ^ SHA_ROTR( x, 25))
#define SHA256_F3(x)    (SHA_ROTR(x,  7) ^ SHA_ROTR(x, 18) ^ SHA_SHIFT(x,  3))
#define SHA256_F4(x)    (SHA_ROTR(x, 17) ^ SHA_ROTR(x, 19) ^ SHA_SHIFT(x, 10))

#define SHA384_F1(x)    SHA512_F1(x)
#define SHA384_F2(x)    SHA512_F2(x)
#define SHA384_F3(x)    SHA512_F3(x)
#define SHA384_F4(x)    SHA512_F4(x)

#define SHA512_F1(x)    (SHA_ROTR(x, 28) ^ SHA_ROTR(x, 34) ^ SHA_ROTR( x, 39))
#define SHA512_F2(x)    (SHA_ROTR(x, 14) ^ SHA_ROTR(x, 18) ^ SHA_ROTR( x, 41))
#define SHA512_F3(x)    (SHA_ROTR(x,  1) ^ SHA_ROTR(x,  8) ^ SHA_SHIFT(x, 7))
#define SHA512_F4(x)    (SHA_ROTR(x, 19) ^ SHA_ROTR(x, 61) ^ SHA_SHIFT(x, 6))

#define SHA_UNPACK32(x, str) do {               \
    *((str) + 3) = (uint8) ((x)      );         \
    *((str) + 2) = (uint8) ((x) >>  8);         \
    *((str) + 1) = (uint8) ((x) >> 16);         \
    *((str) + 0) = (uint8) ((x) >> 24);         \
}while(0)

#define SHA_PACK32(str, x)  do{                 \
    *(x) =   ((uint32) *((str) + 3)      )      \
           | ((uint32) *((str) + 2) <<  8)      \
           | ((uint32) *((str) + 1) << 16)      \
           | ((uint32) *((str) + 0) << 24);     \
}while(0)

#define SHA_UNPACK64(x, str) do {               \
    *((str) + 7) = (uint8) ((x)      );         \
    *((str) + 6) = (uint8) ((x) >>  8);         \
    *((str) + 5) = (uint8) ((x) >> 16);         \
    *((str) + 4) = (uint8) ((x) >> 24);         \
    *((str) + 3) = (uint8) ((x) >> 32);         \
    *((str) + 2) = (uint8) ((x) >> 40);         \
    *((str) + 1) = (uint8) ((x) >> 48);         \
    *((str) + 0) = (uint8) ((x) >> 56);         \
}while(0)

#define SHA_PACK64(str, x)  do{                 \
    *(x) =   ((uint64) *((str) + 7)      )      \
           | ((uint64) *((str) + 6) <<  8)      \
           | ((uint64) *((str) + 5) << 16)      \
           | ((uint64) *((str) + 4) << 24)      \
           | ((uint64) *((str) + 3) << 32)      \
           | ((uint64) *((str) + 2) << 40)      \
           | ((uint64) *((str) + 1) << 48)      \
           | ((uint64) *((str) + 0) << 56);     \
}while(0)

#define SHA_UNPACK(x, str)      SHA_UNPACK32(x, str)
#define SHA224_UNPACK(x, str)   SHA_UNPACK32(x, str)
#define SHA256_UNPACK(x, str)   SHA_UNPACK32(x, str)
#define SHA384_UNPACK(x, str)   SHA_UNPACK64(x, str)
#define SHA512_UNPACK(x, str)   SHA_UNPACK64(x, str)

#define SHA224_PACK(str, x)     SHA_PACK32(str, x)
#define SHA256_PACK(str, x)     SHA_PACK32(str, x)
#define SHA384_PACK(str, x)     SHA_PACK64(str, x)
#define SHA512_PACK(str, x)     SHA_PACK64(str, x)

#define SHA224_SCR(w, i)        SHA256_SCR(w, i)
#define SHA256_SCR(w, i)    do{                 \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]    \
          + SHA256_F3(w[i - 15]) + w[i - 16];   \
}while(0)

#define SHA384_SCR(w, i)        SHA512_SCR(w, i)
#define SHA512_SCR(w, i)    do{                 \
    w[i] =  SHA512_F4(w[i -  2]) + w[i -  7]    \
          + SHA512_F3(w[i - 15]) + w[i - 16];   \
}while(0)

typedef struct {
    uint32 tot_len;
    uint32 len;
    byte block[2 * SHA224_BLOCK_SIZE];
    sha224_uint_t h[8];
} sha224_ctx_t;

typedef struct {
    uint32 tot_len;
    uint32 len;
    byte block[2 * SHA256_BLOCK_SIZE];
    sha256_uint_t h[8];
} sha256_ctx_t;

typedef struct {
    uint32 tot_len;
    uint32 len;
    byte block[2 * SHA384_BLOCK_SIZE];
    sha384_uint_t h[8];
} sha384_ctx_t;

typedef struct {
    uint32 tot_len;
    uint32 len;
    byte block[2 * SHA512_BLOCK_SIZE];
    sha512_uint_t h[8];
} sha512_ctx_t;

#define SHA_INITER(_h0_initer)  {   \
    .tot_len= 0,                    \
    .len    = 0,                    \
    .h      = _h0_initer,           \
}   /* end */
#define SHA224_INITER       SHA_INITER(SHA224_H_INITER)
#define SHA256_INITER       SHA_INITER(SHA256_H_INITER)
#define SHA384_INITER       SHA_INITER(SHA384_H_INITER)
#define SHA512_INITER       SHA_INITER(SHA512_H_INITER)

#define SHA_INIT(_ctx, _h0_initer)          do{ \
    typeof((_ctx)->h[0]) __h0[8] = _h0_initer;  \
                                                \
    os_memcpy((_ctx)->h, __h0, sizeof(__h0));   \
                                                \
    (_ctx)->len        = 0;                     \
    (_ctx)->tot_len    = 0;                     \
}while(0)
#define SHA224_INIT(_ctx)   SHA_INIT(_ctx, SHA224_H_INITER)
#define SHA256_INIT(_ctx)   SHA_INIT(_ctx, SHA256_H_INITER)
#define SHA384_INIT(_ctx)   SHA_INIT(_ctx, SHA384_H_INITER)
#define SHA512_INIT(_ctx)   SHA_INIT(_ctx, SHA512_H_INITER)

#define DECLARE_SHA_TABLE(_number) \
DECLARE void \
sha##_number##_transf(sha##_number##_ctx_t *ctx, const byte *message, uint32 block_nb) \
{                                                           \
    static sha##_number##_uint_t k[SHA##_number##_K_COUNT] = SHA##_number##_K_INITER; \
                                                            \
    sha##_number##_uint_t w[SHA##_number##_K_COUNT];        \
    sha##_number##_uint_t wv[8];                            \
    sha##_number##_uint_t t1, t2;                           \
    const byte *sub_block;                                  \
    int i, j;                                               \
                                                            \
    for (i = 0; i < (int) block_nb; i++) {                  \
        sub_block = message + (i << SHA##_number##_T_SHIFT);\
                                                            \
        for (j = 0; j < 16; j++) {                          \
            SHA##_number##_PACK(&sub_block[j << (SHA##_number##_T_SHIFT-4)], &w[j]); \
        }                                                   \
                                                            \
        for (j = 16; j < SHA##_number##_K_COUNT; j++) {     \
            SHA##_number##_SCR(w, j);                       \
        }                                                   \
                                                            \
        for (j = 0; j < 8; j++) {                           \
            wv[j] = ctx->h[j];                              \
        }                                                   \
                                                            \
        for (j = 0; j < SHA##_number##_K_COUNT; j++) {      \
            t1 = wv[7] + SHA##_number##_F2(wv[4]) + SHA_CH(wv[4], wv[5], wv[6]) \
                + k[j] + w[j];                              \
            t2 = SHA##_number##_F1(wv[0]) + SHA_MAJ(wv[0], wv[1], wv[2]); \
            wv[7] = wv[6];                                  \
            wv[6] = wv[5];                                  \
            wv[5] = wv[4];                                  \
            wv[4] = wv[3] + t1;                             \
            wv[3] = wv[2];                                  \
            wv[2] = wv[1];                                  \
            wv[1] = wv[0];                                  \
            wv[0] = t1 + t2;                                \
        }                                                   \
                                                            \
        for (j = 0; j < 8; j++) {                           \
            ctx->h[j] += wv[j];                             \
        }                                                   \
    }                                                       \
}                                                           \
DECLARE void                                                \
sha##_number##_init(sha##_number##_ctx_t *ctx)              \
{                                                           \
    SHA##_number##_INIT(ctx);                               \
}                                                           \
DECLARE void                                                \
sha##_number##_update(sha##_number##_ctx_t *ctx, const byte *message, uint32 len) \
{                                                           \
    uint32 block_nb;                                        \
    uint32 new_len, rem_len, tmp_len;                       \
    const byte *shifted_message;                            \
                                                            \
    tmp_len = SHA##_number##_BLOCK_SIZE - ctx->len;         \
    rem_len = len < tmp_len ? len : tmp_len;                \
                                                            \
    memcpy(&ctx->block[ctx->len], message, rem_len);        \
                                                            \
    if (ctx->len + len < SHA##_number##_BLOCK_SIZE) {       \
        ctx->len += len;                                    \
        return;                                             \
    }                                                       \
                                                            \
    new_len = len - rem_len;                                \
    block_nb = new_len / SHA##_number##_BLOCK_SIZE;         \
                                                            \
    shifted_message = message + rem_len;                    \
                                                            \
    sha##_number##_transf(ctx, ctx->block, 1);              \
    sha##_number##_transf(ctx, shifted_message, block_nb);  \
                                                            \
    rem_len = new_len % SHA##_number##_BLOCK_SIZE;          \
                                                            \
    memcpy(ctx->block, &shifted_message[block_nb << SHA##_number##_U_SHIFT], rem_len); \
                                                            \
    ctx->len = rem_len;                                     \
    ctx->tot_len += (block_nb + 1) << SHA##_number##_U_SHIFT; \
}                                                           \
DECLARE void                                                \
sha##_number##_final(sha##_number##_ctx_t *ctx, byte digest[]) \
{                                                           \
    uint32 block_nb;                                        \
    uint32 pm_len;                                          \
    uint32 len_b;                                           \
    int i;                                                  \
                                                            \
    block_nb = (1 + ((SHA##_number##_BLOCK_SIZE - SHA##_number##_F_OFFSET) \
                     < (ctx->len % SHA##_number##_BLOCK_SIZE))); \
                                                            \
    len_b = (ctx->tot_len + ctx->len) << 3;                 \
    pm_len = block_nb << SHA##_number##_F_SHIFT;            \
                                                            \
    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);    \
    ctx->block[ctx->len] = 0x80;                            \
    SHA_UNPACK(len_b, ctx->block + pm_len - 4);             \
                                                            \
    sha##_number##_transf(ctx, ctx->block, block_nb);       \
                                                            \
    for (i = 0 ; i < SHA##_number##_F_COUNT; i++) {         \
        SHA##_number##_UNPACK(ctx->h[i], &digest[i << (SHA##_number##_F_SHIFT-4)]); \
    }                                                       \
}                                                           \
DECLARE void                                                \
sha##_number(const byte *message, uint32 len, byte digest[]) \
{                                                           \
    sha##_number##_ctx_t ctx = SHA##_number##_INITER;       \
                                                            \
    sha##_number##_update(&ctx, message, len);              \
    sha##_number##_final(&ctx, digest);                     \
}                                                           \
os_fake_declare                                             \
/* end */

#if USE_MOD_SHA224
EXTERN void
sha224_init(sha224_ctx_t *ctx);
EXTERN void
sha224_update(sha224_ctx_t *ctx, const byte *message, uint32 len);
EXTERN void
sha224_final(sha224_ctx_t *ctx, byte digest[SHA224_DIGEST_SIZE]);
EXTERN void
sha224(const byte *message, uint32 len, byte digest[SHA224_DIGEST_SIZE]);
#endif

#if USE_MOD_SHA256
EXTERN void
sha256_init(sha256_ctx_t *ctx);
EXTERN void
sha256_update(sha256_ctx_t *ctx, const byte *message, uint32 len);
EXTERN void
sha256_final(sha256_ctx_t *ctx, byte digest[SHA256_DIGEST_SIZE]);
EXTERN void
sha256(const byte *message, uint32 len, byte digest[SHA256_DIGEST_SIZE]);
#endif

#if USE_MOD_SHA384
EXTERN void
sha384_init(sha384_ctx_t *ctx);
EXTERN void
sha384_update(sha384_ctx_t *ctx, const byte *message, uint32 len);
EXTERN void
sha384_final(sha384_ctx_t *ctx, byte digest[SHA384_DIGEST_SIZE]);
EXTERN void
sha384(const byte *message, uint32 len, byte digest[SHA384_DIGEST_SIZE]);
#endif

#if USE_MOD_SHA512
EXTERN void
sha512_init(sha512_ctx_t *ctx);
EXTERN void
sha512_update(sha512_ctx_t *ctx, const byte *message, uint32 len);
EXTERN void
sha512_final(sha512_ctx_t *ctx, byte digest[SHA512_DIGEST_SIZE]);
EXTERN void
sha512(const byte *message, uint32 len, byte digest[SHA512_DIGEST_SIZE]);
#endif

#if 1
#define SHA2_ENUM_MAPPER(_)     \
    _(SHA_224,  0, "sha224"),   \
    _(SHA_256,  1, "sha256"),   \
    _(SHA_384,  2, "sha384"),   \
    _(SHA_512,  3, "sha512"),   \
    /* end */
DECLARE_ENUM(sha2, SHA2_ENUM_MAPPER, SHA_END);

static inline enum_ops_t *sha2_ops(void);
static inline bool is_good_sha2(int id);
static inline char *sha2_getnamebyid(int id);
static inline int sha2_getidbyname(const char *name);

#define SHA_224     SHA_224
#define SHA_256     SHA_256
#define SHA_384     SHA_384
#define SHA_512     SHA_512
#define SHA_END     SHA_END
#endif

#define SHA_TYPE(_digest_size)  (   \
    SHA224_DIGEST_SIZE==(_digest_size)?SHA_224:( \
    SHA256_DIGEST_SIZE==(_digest_size)?SHA_256:( \
    SHA384_DIGEST_SIZE==(_digest_size)?SHA_384:( \
    SHA512_DIGEST_SIZE==(_digest_size)?SHA_512:( \
    SHA_256))))) /* end */

#define SHA_DIGEST_SIZE(_type)  (   \
    SHA_224==(_type)?SHA224_DIGEST_SIZE:(    \
    SHA_256==(_type)?SHA256_DIGEST_SIZE:(    \
    SHA_384==(_type)?SHA384_DIGEST_SIZE:(    \
    SHA_512==(_type)?SHA512_DIGEST_SIZE:(    \
    SHA256_DIGEST_SIZE))))) /* end */

#define SHA_BLOCK_SIZE(_type)  (    \
    SHA_224==(_type)?SHA224_BLOCK_SIZE:(     \
    SHA_256==(_type)?SHA256_BLOCK_SIZE:(     \
    SHA_384==(_type)?SHA384_BLOCK_SIZE:(     \
    SHA_512==(_type)?SHA512_BLOCK_SIZE:(     \
    SHA256_BLOCK_SIZE))))) /* end */

typedef struct {
    int type;   // SHA_END

    union {
        sha224_ctx_t ctx224;
        sha256_ctx_t ctx256;
        sha384_ctx_t ctx384;
        sha512_ctx_t ctx512;
    } u;
} sha2_ctx_t;

EXTERN void
sha2_init(sha2_ctx_t *ctx);

EXTERN void
sha2_update(sha2_ctx_t *ctx, const byte *message, uint32 len);

EXTERN void
sha2_final(sha2_ctx_t *ctx, byte digest[]);

EXTERN void
sha2(int type, const byte *message, uint32 len, byte digest[]);

#ifdef __BOOT__
#include "weos/boot/sha2.c"
#endif
/******************************************************************************/
#endif /* __SHA2_H_f482c6dfe05341e7b2ddf4db926acb7d__ */
