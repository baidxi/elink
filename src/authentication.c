#include "authentication.h"

static int32_t _init(p_client_node_t p_client_node)
{
    int32_t ret = -1;

    dhkey_exit(p_client_node->dhkey.p_dh);
    FREE(p_client_node->dhkey.p_key);
    FREE(p_client_node->dhkey.g_key);
    FREE(p_client_node->dhkey.pub_key);
    FREE(p_client_node->dhkey.p_shared_key);

    memset(p_client_node->recv_buf, 0, MAX_BUF_LEN);

    p_client_node->dhkey.p_shared_key = NULL;
    p_client_node->dhkey.g_key = NULL;
    p_client_node->dhkey.p_key = NULL;
    p_client_node->dhkey.pub_key = NULL;
    p_client_node->dhkey.p_dh = NULL;

    //状态设置为初始化状态
    p_client_node->sequence_num = 0;
    p_client_node->link_state = init_e;
    
    //dh秘钥初始化
    ret = dhkey_init(&p_client_node->dhkey);

    return ret;
}

//重新认证
int32_t restart_authentication(p_client_node_t p_client_node, client_type_t type)
{
    int32_t ret = 0;
    close(p_client_node->fd);
    p_client_node->fd = 0;

    client_init(p_client_node, type);
    p_client_node->link_state = connect_e;

    ret = start_authentication(p_client_node);

    return ret;
    
}

//开始认证
int32_t start_authentication(p_client_node_t p_client_node)
{
    int32_t ret = 0;

    if (_init(p_client_node) < 0)
    {
        log_e("_init error!\n");
    }

    //等待秘钥方式协商完成
    while(p_client_node->link_state != key_req_e)
    {
        //发送秘钥协商方式
        ret = send_secret_request(p_client_node);
        if (ret > 0)
        {
            //状态修改为发送协商请求
            p_client_node->link_state = key_req_e;
        }
        else if (ret == CONNECT_ERROR_E)
        {
            return ret;
        }
        sleep(WAITE_TIME_S);
    }
    //等待秘钥协商完成
    while(p_client_node->link_state != key_exc_e)
    {
        //发送秘钥
        ret = send_dh_info(p_client_node);
        if (ret == CONNECT_ERROR_E)
        {
            return ret;
        }

        sleep(WAITE_TIME_S);
    }

    //等待设备注册请求完成
    while(p_client_node->link_state != dev_reg_e)
    {
        //发送设备注册请求
        ret = send_dev_reg(p_client_node);
        if (ret == CONNECT_ERROR_E)
        {
            return ret;
        }
        sleep(WAITE_TIME_S);
    }
    
    return 0;
}