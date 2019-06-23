#ifndef __CLIENT_H
#define __CLIENT_H

#include "common.h"
#include "dhkey.h"

typedef enum
{
    remote_client_e = 0,
    local_client_e
} client_type_t;

//客户端节点
typedef struct
{
    int32_t fd;
    //发送计数
    uint64_t sequence_num;
    //接收数组
    int8_t recv_buf[MAX_BUF_LEN];

    //数据长度
    uint32_t msg_len;

    //连接状态
    link_state_t link_state;
    
    //dh加密结构
    dhkey_t dhkey;
} client_node_t, *p_client_node_t;

typedef struct
{
    client_node_t local_node;
    client_node_t remote_node;
} client_t, *p_client_t;

int32_t start_authentication(p_client_node_t p_client_node);
int32_t restart_authentication(p_client_node_t p_client_node, client_type_t type);
void client_init(p_client_node_t p_client_node, client_type_t type);
void client_exit(p_client_node_t p_client_node);
int32_t client_send_buf(int32_t fd, int8_t *buf, int32_t len);

#endif