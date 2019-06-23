#include <json.h>
#include <ulog.h>

#include "common.h"
#include "des.h"
#include "client.h"
#include "msg.h"
#include "dhkey.h"

//需要发送的msg计算
static uint8_t *_add_elink_hdr_local(uint8_t *buf, uint32_t len)
{
    uint32_t buf_len = 0;
    uint32_t magic = htonl(ELINK_MAGIC);
    uint8_t *newbuf = NULL;

    if (!buf)
    {
        log_e("invalid buf");
        return NULL;
    }

    buf_len = htonl(len);
    newbuf = malloc(ELINK_HEADER_LEN + len);

    if (newbuf)
    {
        memcpy(newbuf, &magic, ELINK_MAGIC_LEN);
        memcpy(newbuf + ELINK_MAGIC_LEN, &buf_len, ELINK_CONTENT_LEN);
        memcpy(newbuf + ELINK_HEADER_LEN, buf, len);
    }

    return newbuf;
}

//数据发送函数，根据是否生成共享秘钥来判断数据是否需要加密
int32_t msg_sent(p_client_node_t p_client_node, int8_t *msg, uint32_t msg_len)
{
    uint8_t *outbuf = NULL;
    uint8_t *outbuf_tmp = NULL;
    uint8_t *pad_msg = NULL;
    int32_t outlen = 0, pad_len = 0;
    int32_t outlen_tmp = 0;
    int8_t buf[MAX_BUF_LEN] = {0};
    uint8_t *hdrbuf = NULL;
    int32_t len = 0;

    if (p_client_node->dhkey.p_shared_key != NULL)
    {
        log_d("msg_sent msg is %s", msg);

        //加密
        if (msg_len % AES_128_BLOCK_SIZE)
        {
            pad_len = msg_len + AES_128_BLOCK_SIZE - (msg_len % AES_128_BLOCK_SIZE);
            pad_msg = calloc(1, pad_len); //memwatch realloc will not work

            if (pad_msg == NULL)
            {
                return 0;
            }
            else
            {
                memcpy(pad_msg, msg, msg_len);
            }
            do_aes_cbc_crypt(pad_msg, pad_len, &outbuf, &outlen, p_client_node->dhkey.p_shared_key, 1);
        }
        else
        {
            do_aes_cbc_crypt(msg, msg_len, &outbuf, &outlen, p_client_node->dhkey.p_shared_key, 1);
        }

        hdrbuf = _add_elink_hdr_local(outbuf, outlen);

        FREE(pad_msg);
        FREE(outbuf);
    }
    else
    {
        log_d("msg_sent msg is %s", msg);
        outlen = strlen(msg);
        hdrbuf = _add_elink_hdr_local(msg, outlen);
    }

    int32_t ret = client_send_buf(p_client_node->fd, hdrbuf, outlen + ELINK_HEADER_LEN);
    if (ret <= 0)
    {
        log_e("client_send_buf error");
    }
    FREE(hdrbuf)

    return ret;
}

//发送协商秘钥请求，客户端应该把自己的p,g和公钥发送给服务器
int32_t send_secret_request(p_client_node_t p_client_node)
{
    //构造json数据
    json_object *json = json_object_new_object();
    json_object_object_add(json, "type", json_object_new_string("keyngreq"));
    json_object_object_add(json, "sequence", json_object_new_int64(p_client_node->sequence_num));

    //获取路由器wan口mac
    json_object_object_add(json, "mac", json_object_new_string("40b0345078f9"));
    json_object_object_add(json, "version", json_object_new_string("V2017.1.0"));

    json_object *array = json_object_new_array();
    json_object *tmp = json_object_new_object();

    json_object_object_add(tmp, "keymode", json_object_new_string("dh"));
    json_object_array_add(array, tmp);

    json_object_object_add(json, "keymodelist", array);

    //将json对象转换成json字符串
    int8_t *msg = json_object_to_json_string(json);

    int32_t ret = msg_sent(p_client_node, msg, strlen(msg));

    //释放
    json_object_put(json);

    return ret;
}

//发送dh key和p,g
int32_t send_dh_info(p_client_node_t p_client_node)
{
    p_client_node->sequence_num++;

    //构造json数据
    json_object *json = json_object_new_object();
    json_object_object_add(json, "type", json_object_new_string("dh"));
    json_object_object_add(json, "sequence", json_object_new_int64(p_client_node->sequence_num));

    //获取路由器wan口mac
    json_object_object_add(json, "mac", json_object_new_string("40b0345078f9"));

    //创建一个json集合
    json_object *json_list = json_object_new_object();
    json_object_object_add(json_list, "dh_key", json_object_new_string(p_client_node->dhkey.pub_key));
    json_object_object_add(json_list, "dh_p", json_object_new_string(p_client_node->dhkey.p_key));
    json_object_object_add(json_list, "dh_g", json_object_new_string(p_client_node->dhkey.g_key));
    json_object_object_add(json, "data", json_list);

    //将json对象转换成json字符串
    int8_t *msg = json_object_to_json_string(json);

    int32_t ret = msg_sent(p_client_node, msg, strlen(msg));

    //释放
    json_object_put(json);

    return ret;
}

//发送设备注册消息
int32_t send_dev_reg(p_client_node_t p_client_node)
{
    p_client_node->sequence_num++;

    //构造json数据
    json_object *json = json_object_new_object();
    json_object_object_add(json, "type", json_object_new_string("dev_reg"));
    json_object_object_add(json, "sequence", json_object_new_int64(p_client_node->sequence_num));

    //TODO:获取路由器wan口mac
    json_object_object_add(json, "mac", json_object_new_string("40b0345078f9"));

    //创建一个json集合
    json_object *json_list = json_object_new_object();
    json_object_object_add(json_list, "vendor", json_object_new_string("360"));
    json_object_object_add(json_list, "model", json_object_new_string("P1"));
    json_object_object_add(json_list, "swversion", json_object_new_string("V1.0.0"));
    json_object_object_add(json_list, "hdversion", json_object_new_string("V1.0.0"));

    //TODO:需要填上平台分配的sn号
    json_object_object_add(json_list, "sn", json_object_new_string("0x11223344556677"));

    //TODO:需要获取wan口的ip地址
    json_object_object_add(json_list, "ipaddr", json_object_new_string("V1.0.0"));

    //TODO:厂商产品介绍的url
    json_object_object_add(json_list, "url", json_object_new_string("www.baidu.com"));

    //TODO:产品是有线还是无线  我们这里是无线
    json_object_object_add(json_list, "wireless", json_object_new_string("no"));

    json_object_object_add(json, "data", json_list);

    //将json对象转换成json字符串
    int8_t *msg = json_object_to_json_string(json);

    int32_t ret = msg_sent(p_client_node, msg, strlen(msg));

    //释放
    json_object_put(json);

    return ret;
}

//发送心跳包
int32_t send_heart_msg(p_client_node_t p_client_node)
{
    p_client_node->sequence_num++;

    //构造json数据
    json_object *json = json_object_new_object();
    json_object_object_add(json, "type", json_object_new_string("keepalive"));
    json_object_object_add(json, "sequence", json_object_new_int64(p_client_node->sequence_num));

    //TODO:获取路由器wan口mac
    json_object_object_add(json, "mac", json_object_new_string("40b0345078f9"));

    //将json对象转换成json字符串
    int8_t *msg = json_object_to_json_string(json);

    int32_t ret = msg_sent(p_client_node, msg, strlen(msg));

    //释放
    json_object_put(json);

    return ret;
}
