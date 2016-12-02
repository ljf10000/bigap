#ifndef __BASE64_H_4a45a623e251441595a8422b336fcf17__
#define __BASE64_H_4a45a623e251441595a8422b336fcf17__
/******************************************************************************/
// https://github.com/littlstar/b64.c

STATIC INLINE char
b64_table(int v);

/**
 * Encode `byte *' source with `size_t' size.
 * Returns a `char *' base64 encoded string.
 */
STATIC INLINE char *
b64_encode(byte *src, size_t len);

/**
 * Dencode `char *' source with `size_t' size.
 * Returns a `unsigned char *' base64 decoded string + size of decoded string.
 */
STATIC INLINE char *
b64_decode_ex(byte *src, size_t len, size_t *decsize);

/**
 * Dencode `char *' source with `size_t' size.
 * Returns a `unsigned char *' base64 decoded string.
 */
static inline char *
b64_decode(byte *src, size_t len)
{
	return b64_decode_ex(src, len, NULL);
}

#ifdef __BOOT__
#include "weos/boot/base64.c"
#endif
/******************************************************************************/
#endif				/* __BASE64_H_4a45a623e251441595a8422b336fcf17__ */
