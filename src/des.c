#include <utils.h>
#include <ulog.h>

#include "des.h"
#include "dhkey.h"

int32_t do_b64_cmd(uint8_t *src, int32_t srclen, uint8_t **dst, des_mode_t des_mode)
{
	int32_t dstlen = 0;

	if (des_mode == encrypt_e)
		dstlen = B64_ENCODE_LEN(srclen);
	else
		dstlen = B64_DECODE_LEN(srclen);

	*dst = malloc(dstlen);
	if (!*dst) {
		log_e("malloc failed");
		return -1;
	}

	if (des_mode == encrypt_e)
		dstlen = b64_encode(src, srclen, *dst, dstlen);
	else
		dstlen = b64_decode(src, *dst, dstlen);

	return dstlen;
}

//采用对称解密

/*
in：        输出的字符串
inlen：     输入字符串长度
out:        输出字符串
outlen:     输出字符串长度
key:        生成的key
do_encrypt: 加密/解密
*/

int32_t do_aes_cbc_crypt(uint8_t *in, int32_t inlen, uint8_t **out, int32_t *outlen, uint8_t *key, int32_t do_encrypt)
{
	int32_t tmplen = 0;
	uint8_t *buf = NULL;
	uint8_t iv[EVP_MAX_IV_LENGTH] = {0};
	EVP_CIPHER_CTX ctx;

#if ENABLE_AES_DEBUG
	log_d("do_aes_cbc_crypt %s from:", do_encrypt ? "encrypt" : "decrypt");
	bin_print(in, inlen);
#endif

	/* make sure key size == 16 */

	EVP_CIPHER_CTX_init(&ctx);

	EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv, do_encrypt);

	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	buf = calloc(1, inlen + 1);

	if (!buf) {
		ULOG_ERR("%s: %s malloc %d failed\n", __func__, do_encrypt ? "encrypt" : "decrypt", inlen + 1);
		return -1;
	}

	if (EVP_CipherUpdate(&ctx, buf, outlen, in, inlen) == 0) {
		ULOG_ERR("aes_128_cbc %s failed: EVP_CipherUpdate\n", do_encrypt ? "encrypt" : "decrypt");
		EVP_CIPHER_CTX_cleanup(&ctx);
		FREE(buf);
		return -1;
	}

	if (EVP_CipherFinal_ex(&ctx, buf + *outlen, &tmplen) == 0 && (tmplen > 0)) {
		ULOG_ERR("aes_128_cbc %s failed: EVP_CipherFinal_ex, tmplen %d\n", do_encrypt ? "encrypt" : "decrypt", tmplen);
		EVP_CIPHER_CTX_cleanup(&ctx);
		FREE(buf);
		return -1;
	}

	*out = buf;

	*outlen += tmplen;

#if ENABLE_AES_DEBUG
	log_d("================do_aes_cbc_crypt to:");
	bin_print(*out, *outlen);
#endif

	EVP_CIPHER_CTX_cleanup(&ctx);
	return *outlen;
}