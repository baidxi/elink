#include "dhkey.h"

//秘钥初始化，生成p，g以及公钥和私钥
int32_t dhkey_init(p_dhkey_t p_dhkey)
{
    int32_t ret = 0;
    uint8_t str[DH_128_KEY_LEN] = {0};

    //生成公钥
    p_dhkey->p_dh = DH_new();

    //生成client 的密钥参数
    //prime_len，g大于1，常见2和5
    //TODO: 这里的128代表的是16字节(16*8).最开始调不通的原因是我们用的64位，也就是8个字节
    ret = DH_generate_parameters_ex(p_dhkey->p_dh, 128, DH_GENERATOR_2, NULL);

    if (ret != 1)
    {
        log_e("DH_generate_parameters_ex err!");
        ret = -1;
        goto done;
    }

    //检查密钥参数
    int32_t i = 0;
    ret = DH_check(p_dhkey->p_dh, &i); //i先被置0,然后通过或等于,得到出错信息类型

    if (ret != 1)
    {
        log_e("DH_check err!");

        if (i & DH_CHECK_P_NOT_PRIME)
        {
            log_e("p value is not prime");
        }
        else if (i & DH_CHECK_P_NOT_SAFE_PRIME)
        {
            log_e("p value is not a safe prime");
        }
        else if (i & DH_UNABLE_TO_CHECK_GENERATOR)
        {
            log_e("unable to check the generator value");
        }
        else if (i & DH_NOT_SUITABLE_GENERATOR)
        {
            log_e("the g value is not a generator");
        }
        goto done;
    }

    log_e("DH parameters appear to be ok.");

    //获取秘钥大小
    int32_t size = DH_size(p_dhkey->p_dh);
    log_d("DH key1 size : %d\n", size);

    //生成公钥和私钥
    DH_generate_key(p_dhkey->p_dh);

    log_d("dh's p = %s", BN_bn2hex(p_dhkey->p_dh->p));
    log_d("dh's g = %s", BN_bn2hex(p_dhkey->p_dh->g));
    log_d("dh's pub_key = %s", BN_bn2hex(p_dhkey->p_dh->pub_key));

    uint32_t plen = 0, glen = 0, klen = 0;
    uint8_t key_str[DH_128_KEY_LEN] = {0};
    uint8_t p_str[DH_128_KEY_LEN] = {0};
    uint8_t g_str[DH_128_KEY_LEN] = {0};

    //计算p.g,key采用base64加密
    klen = BN_bn2bin(p_dhkey->p_dh->pub_key, key_str);
    //编码
    do_b64_cmd(key_str, klen, &(p_dhkey->pub_key), encrypt_e);

    //计算p.g,key采用base64加密
    plen = BN_bn2bin(p_dhkey->p_dh->p, p_str);
    //编码
    do_b64_cmd(p_str, plen, &(p_dhkey->p_key), encrypt_e);

    //计算p.g,key采用base64加密
    glen = BN_bn2bin(p_dhkey->p_dh->g, g_str);
    //编码
    do_b64_cmd(g_str, glen, &(p_dhkey->g_key), encrypt_e);

done:
    return ret;
}

int32_t gen_dh_shared_key(uint8_t **p_shared_key, BIGNUM *clipub, DH *dh)
{
    int32_t ret = 0;
    int32_t len = DH_size(dh);

    *p_shared_key = (uint8_t *)malloc(len + 1);

    if (p_shared_key == NULL)
    {
        log_e("Failed to malloc shared key");
        return -1;
    }
    else
    {
        memset(*p_shared_key, 0, len + 1);
    }

    ret = DH_compute_key(*p_shared_key, clipub, dh);

    return ret;
}

void dhkey_exit(DH *p_dh)
{
    if (p_dh != NULL)
    {
        DH_free(p_dh);
    }
}

void bin_print(uint8_t *value, uint32_t len)
{
    int32_t i = 0;

    for (i = 0; i < len; i++)
    {
        if (i && (i % 64 == 0))
            printf("\n");

        printf("%02X", value[i] & 0xff);
    }

    printf(" (%u)\n", len);

    return;
}
