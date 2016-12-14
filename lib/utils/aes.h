#ifndef __AES_H_c70ef85be0db44c9ba68886d5a275db9__
#define __AES_H_c70ef85be0db44c9ba68886d5a275db9__
/******************************************************************************/
// https://github.com/B-Con/crypto-algorithms

/****************************** MACROS ******************************/
#define AES_BLOCK_SIZE 16               // AES operates on 16 bytes at a time

/*********************** FUNCTION DECLARATIONS **********************/
///////////////////
// AES
///////////////////
// Key setup must be done before any AES en/de-cryption functions can be used.
EXTERN void aes_key_setup(const byte key[],     // The key, must be 128, 192, or 256 bits
                   uint32 w[],                  // Output key schedule to be used later
                   int keysize);                // Bit length of the key, 128, 192, or 256

EXTERN void aes_encrypt(const byte in[],        // 16 bytes of plaintext
                 byte out[],                    // 16 bytes of ciphertext
                 const uint32 key[],            // From the key setup
                 int keysize);                  // Bit length of the key, 128, 192, or 256

EXTERN void aes_decrypt(const byte in[],        // 16 bytes of ciphertext
                 byte out[],                    // 16 bytes of plaintext
                 const uint32 key[],            // From the key setup
                 int keysize);                  // Bit length of the key, 128, 192, or 256

///////////////////
// AES - CBC
///////////////////
EXTERN int aes_encrypt_cbc(const byte in[],          // Plaintext
                    size_t in_len,            // Must be a multiple of AES_BLOCK_SIZE
                    byte out[],               // Ciphertext, same length as plaintext
                    const uint32 key[],         // From the key setup
                    int keysize,              // Bit length of the key, 128, 192, or 256
                    const byte iv[]);         // IV, must be AES_BLOCK_SIZE bytes long

// Only output the CBC-MAC of the input.
EXTERN int aes_encrypt_cbc_mac(const byte in[],      // plaintext
                        size_t in_len,        // Must be a multiple of AES_BLOCK_SIZE
                        byte out[],           // Output MAC
                        const uint32 key[],     // From the key setup
                        int keysize,          // Bit length of the key, 128, 192, or 256
                        const byte iv[]);     // IV, must be AES_BLOCK_SIZE bytes long

///////////////////
// AES - CTR
///////////////////
EXTERN void aes_increment_iv(byte iv[],                  // Must be a multiple of AES_BLOCK_SIZE
                  int counter_size);          // Bytes of the IV used for counting (low end)

EXTERN void aes_encrypt_ctr(const byte in[],         // Plaintext
                     size_t in_len,           // Any byte length
                     byte out[],              // Ciphertext, same length as plaintext
                     const uint32 key[],        // From the key setup
                     int keysize,             // Bit length of the key, 128, 192, or 256
                     const byte iv[]);        // IV, must be AES_BLOCK_SIZE bytes long

EXTERN void aes_decrypt_ctr(const byte in[],         // Ciphertext
                     size_t in_len,           // Any byte length
                     byte out[],              // Plaintext, same length as ciphertext
                     const uint32 key[],        // From the key setup
                     int keysize,             // Bit length of the key, 128, 192, or 256
                     const byte iv[]);        // IV, must be AES_BLOCK_SIZE bytes long

///////////////////
// AES - CCM
///////////////////
// Returns True if the input parameters do not violate any constraint.
EXTERN int aes_encrypt_ccm(const byte plaintext[],              // IN  - Plaintext.
                    uint32 plaintext_len,                  // IN  - Plaintext length.
                    const byte associated_data[],        // IN  - Associated Data included in authentication, but not encryption.
                    unsigned short associated_data_len,  // IN  - Associated Data length in bytes.
                    const byte nonce[],                  // IN  - The Nonce to be used for encryption.
                    unsigned short nonce_len,            // IN  - Nonce length in bytes.
                    byte ciphertext[],                   // OUT - Ciphertext, a concatination of the plaintext and the MAC.
                    uint32 *ciphertext_len,                // OUT - The length of the ciphertext, always plaintext_len + mac_len.
                    uint32 mac_len,                        // IN  - The desired length of the MAC, must be 4, 6, 8, 10, 12, 14, or 16.
                    const byte key[],                    // IN  - The AES key for encryption.
                    int keysize);                        // IN  - The length of the key in bits. Valid values are 128, 192, 256.

// Returns True if the input parameters do not violate any constraint.
// Use mac_auth to ensure decryption/validation was preformed correctly.
// If authentication does not succeed, the plaintext is zeroed out. To overwride
// this, call with mac_auth = NULL. The proper proceedure is to decrypt with
// authentication enabled (mac_auth != NULL) and make a second call to that
// ignores authentication explicitly if the first call failes.
EXTERN int aes_decrypt_ccm(const byte ciphertext[],             // IN  - Ciphertext, the concatination of encrypted plaintext and MAC.
                    uint32 ciphertext_len,                 // IN  - Ciphertext length in bytes.
                    const byte assoc[],                  // IN  - The Associated Data, required for authentication.
                    unsigned short assoc_len,            // IN  - Associated Data length in bytes.
                    const byte nonce[],                  // IN  - The Nonce to use for decryption, same one as for encryption.
                    unsigned short nonce_len,            // IN  - Nonce length in bytes.
                    byte plaintext[],                    // OUT - The plaintext that was decrypted. Will need to be large enough to hold ciphertext_len - mac_len.
                    uint32 *plaintext_len,                 // OUT - Length in bytes of the output plaintext, always ciphertext_len - mac_len .
                    uint32 mac_len,                        // IN  - The length of the MAC that was calculated.
                    int *mac_auth,                       // OUT - TRUE if authentication succeeded, FALSE if it did not. NULL pointer will ignore the authentication.
                    const byte key[],                    // IN  - The AES key for decryption.
                    int keysize);                        // IN  - The length of the key in BITS. Valid values are 128, 192, 256.

#ifdef __BOOT__
#include "weos/boot/aes.c"
#endif
/******************************************************************************/
#endif /* __AES_H_c70ef85be0db44c9ba68886d5a275db9__ */
