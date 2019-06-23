#include <json.h>
#include <bits.h>

#include "distribution.h"
#include "des.h"
#include "handler.h"
#include "msg.h"

//无cfg配置请求，根据type来区分
static elink_feature_t elink_features[] =
    {
        {"keyngack", recv_keyngack}, //回应指定秘钥生成方式
        {"dh", recv_dh},             //dh交互
        {"dev_reg", NULL},           //设备注册
        {"keepalive", NULL},         //链路保活
        {"ack", recv_ack},           //应答
        {"get_status", NULL},        //查询信息
        {"status", NULL},            //状态返回信息
        {"dev_report", NULL},        //下挂设备状态信息
        {"wan_report", NULL},        //wan状态信息
        {"deassociation", NULL},     //下挂终端去关联
        {"getrssiinfo", NULL},       //无线信号检测
        {"rssiinfo", NULL},          //无线信号信息返回
        {"NULL", NULL}};

//get_status请求，type都是get_status，需要特征码进行区分
static elink_feature_t elink_features_get_status[] =
    {
        {"wifi", recv_wifi},   //查询wifi信息
        {"wifiswitch", recv_wifiswitch},   //查询wifi的状态，on/off
        {"ledswitch", recv_ledswitch},       //查询led的状态
        {"wifitimer", recv_wifitimer},       //查询wifi定时开关
        {"bandsupport", recv_bandsupport}, //查询终端设备支持的频段
        {"cpurate", recv_cpurate},         //查询cpu的速率
        {"memoryuserate", recv_memoryuserate},   //查询内存使用率
        {"uploadspeed", recv_uploadspeed},     //查询上传速率
        {"downloadspeed", recv_downloadspeed},   //查询下载速率
        {"wlanstats", recv_wlanstats},       //查询无线的状态
        {"channel", recv_channel},         //查询无线的信道
        {"onlineTime", recv_onlineTime},      //查询设备的在线时长
        {"terminalNum", recv_terminalNum},     //查询下挂终端数
        {"load", recv_load},            //查询无线信道负载
        {"real_devinfo", recv_real_devinfo},    //下挂终端去关联
        {"elinkstat", recv_elinkstat},       //无线信号检测
        {"neighborinfo", recv_neighborinfo},    //查询ap wlan邻居信息
        {"networktype", recv_networktype},     //查询接入方式
        {"workmode", recv_workmode},        //查询工作模式
        {"NULL", NULL}};

//cfg配置请求，type都是cfg，需要按照特征码进行区分
static elink_feature_t elink_features_cfg[] =
    {
        //配置type为cfg  这些请求的type是固定的
        {"radio", NULL},          //配置与同步信息
        {"wifiswitch", NULL},     //配置与同步信息
        {"wpsswitch", NULL},      //wps开关消息
        {"upgrade", NULL},        //设备升级消息
        {"upgrade_client", NULL}, //设备云客户端消息(可选)
        {"reboot", recv_reboot},  //设备重启
        {"reset", recv_reset},    //设备重置
        {"save", recv_save},      ///tmp/ctc_elinkap.json文件和固件参数一同保存
        {"ctrlcommand", NULL},    //设备操作信息
        {"roaming_set", NULL},    //漫游配置
        {"roaming_report", NULL}, //漫游终端rssi上报
        {"NULL", NULL}};

static int32_t handler_event(p_client_node_t p_client_node, json_object *json)
{
    int8_t type[64] = {0};
    int8_t buf[2048] = {0};
    int32_t ret = 0;

    json_object *tmp_jso = json_object_object_get(json, "type");

    //获取操作类型type
    strcpy(type, json_object_get_string(tmp_jso));

    //根据操作类型的type，调用不同的处理函数
    uint8_t i = 0;

    //将json转换成字符串
    int8_t *msg = json_object_to_json_string(json);

    log_d("=========== %s", msg);

    if (strcmp(type, "get_status") == 0)
    {
        i = 0;
        int32_t indx = 0;
        int8_t *p = NULL;
        int8_t request_str[64] = {0};

        log_d("handler type is get_status");

        //构造json数据
        json_object *json_obj = json_object_new_object();
        json_object_object_add(json_obj, "type", json_object_new_string("status"));
        json_object_object_add(json_obj, "sequence", json_object_new_int64(p_client_node->sequence_num));
        json_object_object_add(json_obj, "mac", json_object_new_string("40b0345078f9"));

        //根据{"type":"get_status","get":[{"name":"wifiswitch"}]}获取name字段

        json_object *array_obj = json_object_object_get(json, "get");

        log_d("array_obj is %s", json_object_to_json_string(array_obj));

        //遍历get数组，根据每一个name字段对应的请求调用对应的处理函数
        int32_t length = json_object_array_length(array_obj);
        log_d("array_obj length is %d", length);
        for (indx = 0; indx < length; indx++)
        {
            json_object *node = json_object_array_get_idx(array_obj, indx);
            json_object *request_obj = json_object_object_get(node, "name");
            strcpy(request_str, json_object_get_string(request_obj));
            log_d("request_str name is %s", request_str);
            
            #if 1
            i = 0;
            //从get_status数组中查找匹配的request_str项
            while (strcmp(elink_features_get_status[i].type, "NULL") != 0)
            {
                if (strcmp(request_str, elink_features_get_status[i].type) ==0)
                {
                    log_d("===============");
                    //查找到匹配的请求，执行处理函数
                    if ((p = elink_features_get_status[i].handler(p_client_node, json)) == NULL)
                    {
                        log_e("%s error", elink_features_get_status[i].type);
                        break;
                    }
                    else
                    {
                        log_d("===============");
                        //将数据追加到buf中
                        //TODO:p指向的字符串是函数内部json生成的字符串
                        strcat(buf, p);
                    }
                }
                i++;
            }
            #endif
        }
        
        #if 1
        log_d("==========");

        //生成最终的json结构
        json_object_object_add(json_obj, "status", json_object_new_string(buf));

        log_d("===============");

        //将json转换成字符串
        int8_t *msg = json_object_to_json_string(json_obj);

        log_d("===============");

        //发送字符串
        ret = msg_sent(p_client_node, msg, strlen(msg));

        log_d("===============");

        json_object_put(json_obj);
        log_d("===============");
        #endif
    }
    else if (strcmp(type, "cfg") == 0)
    {
        //TODO:get_status的请求可能以组合的方式发送到底层，所以要考虑组合形式
        i = 0;
        log_d("handler type is cfg");
        //cfg配置请求，按照特征码区分
        while (strcmp(elink_features_cfg[i].type, "NULL") != 0)
        {
            if (strstr(msg, elink_features_cfg[i].type) != NULL)
            {
                elink_features_cfg[i].handler(p_client_node, json);
                break;
            }
            i++;
        }
    }
    else
    {
        i = 0;
        log_d("handler type is not cfg");
        //无cfg配置请求，按照type进行区分
        while (strcmp(elink_features[i].type, "NULL") != 0)
        {
            if (strcmp(elink_features[i].type, type) == 0)
            {
                log_d("elink_features[i].type is %s", elink_features[i].type);
                if (elink_features[i].handler != NULL)
                {
                    elink_features[i].handler(p_client_node, json);
                }
                break;
            }
            i++;
        }
    }

    return ret;
}

//数据分发函数
int32_t data_distribution(p_client_node_t p_client_node)
{
    int32_t ret = -1;
    int32_t data_len = 0;

    if (p_client_node == NULL)
    {
        log_d("data Distribution data is empty");
        return -1;
    }

    //创建一个json对象
    json_object *json = NULL;

    //在加密协商完成之前，采用密文传输，秘钥协商完成之后采用密文
    //判断秘钥是否协商完成
    if (p_client_node->dhkey.p_shared_key == NULL)
    {
        //共享秘钥为空,数据未加密
        //创建json  生成json对象
        log_d("Distribution data is %s", p_client_node->recv_buf);
        json = json_tokener_parse(p_client_node->recv_buf);
    }
    else
    {
        //共享秘钥不为空，数据加密
        //解密
        #if 1
        uint8_t *outbuf = NULL;
        int32_t outlen = 0;
        if (do_aes_cbc_crypt(p_client_node->recv_buf, p_client_node->msg_len, &outbuf, &outlen, p_client_node->dhkey.p_shared_key, 0) > 0)
        {
            log_d("Distribution data is %s", outbuf);
            if (strstr((int8_t *)outbuf, "keepalive") == NULL)
            {
                log_d("this is keepalive!");
            }
            //创建json  生成json对象
            json = json_tokener_parse(outbuf);
            FREE(outbuf);
        }
        #endif
    }

    if (is_error(json))
    {
        log_e("json is NULL");
        return -1;
    }
    
    //事件操作
    ret = handler_event(p_client_node, json);

    return 0;
}