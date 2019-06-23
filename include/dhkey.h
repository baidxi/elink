#ifndef __DHKEY_H
#define __DHKEY_H

#include "common.h"
#include "des.h"

typedef struct
{
    //dh加密用的p,g公钥和共享秘钥
    uint8_t *p_key;
    uint8_t *g_key;
    uint8_t *pub_key;
    uint8_t *p_shared_key;
    //dh结构
    DH *p_dh;
} dhkey_t, *p_dhkey_t;

int32_t dhkey_init(p_dhkey_t p_dhkey);
int32_t gen_dh_shared_key(uint8_t **p_shared_key, BIGNUM *clipub, DH *dh);
void bin_print(uint8_t *value, uint32_t len);
void dhkey_exit(DH *p_dh);

#endif