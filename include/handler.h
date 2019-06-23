#ifndef __HANDLER_H
#define __HANDLER_H

#include "common.h"
#include "client.h"
#include <json.h>

//接收函数
int8_t *recv_keyngack(p_client_node_t p_client_node, json_object *jso);
int8_t *recv_dh(p_client_node_t p_client_node, json_object *jso);
int8_t *recv_ack(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_reboot(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_reset(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_save(p_client_node_t p_client_node, json_object *json_obj);

//get_status请求
int8_t *recv_wifi(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_wifiswitch(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_ledswitch(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_wifitimer(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_bandsupport(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_cpurate(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_memoryuserate(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_uploadspeed(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_downloadspeed(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_wlanstats(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_channel(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_onlineTime(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_terminalNum(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_load(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_real_devinfo(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_elinkstat(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_neighborinfo(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_networktype(p_client_node_t p_client_node, json_object *json_obj);
int8_t *recv_workmode(p_client_node_t p_client_node, json_object *json_obj);

#endif