#ifndef __DES_H
#define __DES_H

#include "common.h"

typedef enum
{
    decrypt_e = 0,
    encrypt_e
} des_mode_t;

int32_t do_b64_cmd(uint8_t *src, int32_t srclen, uint8_t **dst, des_mode_t des_mode);
int32_t do_aes_cbc_crypt(uint8_t *in, int32_t inlen, uint8_t **out, int32_t *outlen, uint8_t *key, int32_t do_encrypt);

#endif