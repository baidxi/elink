#ifndef __MSG_H
#define __MSG_H

#include <json.h>

#include "common.h"
#include "client.h"

//发送函数
int32_t send_secret_request(p_client_node_t p_client_nodee);
int32_t send_dh_info(p_client_node_t p_client_node);
int32_t send_dev_reg(p_client_node_t p_client_node);
int32_t send_heart_msg(p_client_node_t p_client_node);
int32_t msg_sent(p_client_node_t p_client_node, int8_t *msg, uint32_t msg_len);

#endif