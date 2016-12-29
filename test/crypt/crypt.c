/*******************************************************************************
Copyright (c) 2016-2018, Supper Walle Technology. All rights reserved.
*******************************************************************************/
#ifndef __THIS_APP
#define __THIS_APP      crypt
#endif

#include "utils.h"

OS_INITER;

#define KEYSIZE 8
#define KEYBITS (32*KEYSIZE)

int main(int argc, char *argv[])
{
    byte key[32]        = "0123456789abcdef0123456789ABCDEF";
    byte plaintext[16]  = "0123456789abcdef";
    byte ciphertext[16];
    uint32 key32[KEYSIZE];

    aes_key_setup(key, key32, KEYBITS);

    aes_encrypt(plaintext, ciphertext, key32, KEYBITS);
    os_println("plaintext:");
    os_dump_buffer(plaintext, sizeof(plaintext));
    os_println("ciphertext:");
    os_dump_buffer(ciphertext, sizeof(ciphertext));

    aes_decrypt(ciphertext, plaintext, key32, KEYBITS);
    os_println("plaintext:");
    os_dump_buffer(plaintext, sizeof(plaintext));
    os_println("ciphertext:");
    os_dump_buffer(ciphertext, sizeof(ciphertext));
#if 0
    byte hmac[32];
    byte message[32] = "00112233445566778899aabbccddeeff";

    hmac_sha256(key, sizeof(key), message, sizeof(message), hmac);
    os_println("message:");
    os_dump_buffer(message, sizeof(message));
    os_println("hmac:");
    os_dump_buffer(hmac, sizeof(hmac));
#endif

    return 0;
}
/******************************************************************************/
