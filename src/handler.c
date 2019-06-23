#include <json.h>
#include <ulog.h>

#include "des.h"
#include "client.h"
#include "handler.h"

//接收到回应秘钥生成方式  更新状态
int8_t *recv_keyngack(p_client_node_t p_client_node, json_object *jso)
{
    log_d("recv_keyngack");
    p_client_node->link_state = key_req_e; //秘钥请求完成

    return 0;
}

//秘钥交换完成，生成共享秘钥  更新状态
int8_t *recv_dh(p_client_node_t p_client_node, json_object *json_obj)
{
    uint32_t klen = 0;
    int32_t ret = 0;
    uint8_t *cdhkey = NULL;
    BIGNUM *clipub = NULL;

    int8_t dhkey[MAX_BUF_LEN] = {0};

    log_d("recv_dh");

    BIO *out = NULL;
    out = BIO_new(BIO_s_file());
    BIO_set_fp(out, stdout, BIO_NOCLOSE);

    //json格式数据解析
    json_object *object_obj = json_object_object_get(json_obj, "data");
    json_object *key_obj = json_object_object_get(object_obj, "dh_key");

    strcpy(dhkey, json_object_get_string(key_obj));

    //解编码
    klen = do_b64_cmd((uint8_t *)dhkey, strlen(dhkey), &cdhkey, decrypt_e);

    if (cdhkey)
        clipub = BN_bin2bn(cdhkey, klen, NULL);

    if (clipub != NULL)
    {
        log_d("server public key is: ");
        BN_print(out, clipub);
        log_d(" (%u)\n", klen);
    }

    //生成共享秘钥,保存到shared_key中
    if ( ( ret = gen_dh_shared_key(&(p_client_node->dhkey.p_shared_key), clipub, p_client_node->dhkey.p_dh) ) < 0)
    {
        log_e("failed to generate shared key");
    }

    BN_free(clipub);
    FREE(cdhkey);
    BIO_free(out);

    p_client_node->link_state = key_exc_e; //秘钥交换完成

    return ret;
}

int8_t *recv_ack(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("recv_dh");
    if (p_client_node->link_state == key_exc_e)
    {
        log_d("p_client_node->link_state = dev_reg_e");
        //设备注册完成
        p_client_node->link_state = dev_reg_e;
    }
}

/*==============================cfg配置请求===================================*/
/*
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
*/
int8_t *recv_reboot(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("====recv_reboot");
    //TODO:具体生效调用这个函数，底层会延时制定的delay_s毫秒重启.
    //ERROR_CODE_E configurate_request(source_e type, request_e handle, const int8_t *request_json_str, int8_t response_json_str[MAX_TEXT - 4], int32_t delay_s)
    //system("reboot");

    return NULL;
}

int8_t *recv_reset(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("====recv_reset");
    //TODO:具体生效调用这个函数，底层会延时制定的delay_s毫秒重启.
    //ERROR_CODE_E configurate_request(source_e type, request_e handle, const int8_t *request_json_str, int8_t response_json_str[MAX_TEXT - 4], int32_t delay_s)
    //system("reboot");

    return NULL;
}

int8_t *recv_save(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("====recv_save");
    //TODO:具体生效调用这个函数，底层会延时制定的delay_s毫秒重启.
    //ERROR_CODE_E configurate_request(source_e type, request_e handle, const int8_t *request_json_str, int8_t response_json_str[MAX_TEXT - 4], int32_t delay_s)
    //system("reboot");

    return NULL;
}


/*==============================get_status配置请求===================================*/
/*
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
*/
int8_t *recv_wifi(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_wifiswitch(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return "nihao";
}

int8_t *recv_ledswitch(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_wifitimer(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_bandsupport(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_cpurate(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_memoryuserate(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_uploadspeed(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_downloadspeed(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_wlanstats(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_channel(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_onlineTime(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_terminalNum(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_load(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_real_devinfo(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_elinkstat(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_neighborinfo(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_networktype(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}

int8_t *recv_workmode(p_client_node_t p_client_node, json_object *json_obj)
{
    log_d("=======");
    return NULL;
}
