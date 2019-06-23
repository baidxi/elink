#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>

#include <sys/types.h>
#include <wait.h>

#include <elog.h>

#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/evp.h>

//log相关宏定义
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "elinkclient"
#endif

//连接服务器间隔s
#define TCP_INIT_DELAY_S 5
#define REMOTE_TCP_PORT 32768

#define ELINK_UNIX_SOCKET "/tmp/ctc_elinkap.sock"

//接收数组最大长度
#define MAX_BUF_LEN 4096
#define WAITE_TIME_S 2

#define DH_128_KEY_LEN 16
#define AES_128_BLOCK_SIZE 16

//心跳间隔
#define LOCAL_HEART_S 15
#define REMOTE_HEART_S 15

//头部长度
#define ELINK_MAGIC 0x3f721fb5
#define ELINK_MAGIC_LEN 4
#define ELINK_CONTENT_LEN 4
#define ELINK_HEADER_LEN (ELINK_MAGIC_LEN + ELINK_CONTENT_LEN)

//加解密调试开关
#define ENABLE_AES_DEBUG    1

#define FREE(x)        \
    do                 \
    {                  \
        if (x != NULL) \
        {              \
            free(x);   \
            x = NULL;  \
        }              \
    } while (0);

typedef enum
{
    init_e = 0,  //初始化状态
    connect_e,   //连接状态
    key_req_e,   //秘钥请求
    key_exc_e,   //秘钥交换
    dev_reg_e,   //设备注册
    keep_alive_e //链路保活
} link_state_t;

typedef enum
{
    CONNECT_ERROR_E = -10000
} error_t;

int32_t mk_popen(const int8_t *path, int8_t *const argv[], int32_t *code, int8_t *output, const size_t size);
int32_t check_remote_msg_header(const int8_t *buf);
int32_t check_msg_header(const int8_t *buf);

#endif