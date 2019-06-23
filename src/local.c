#include <pthread.h>
#include "distribution.h"
#include "local.h"

static void create_local_recv_thread(p_client_node_t p_client_node);
static void create_local_heart_thread(p_client_node_t p_client_node);

static void *local_client_recv(void *arg)
{
    //获取fd
    p_client_node_t p_client_node = (p_client_node_t)arg;
    int32_t len = 0;
    int32_t msg_len = 0;
    while (1)
    {
        bzero(p_client_node->recv_buf, MAX_BUF_LEN);
        //循环获取从服务器发来的数据,先读取数据长度，根据长度来判断发送数据的大小，以便读取
        len = recv(p_client_node->fd, p_client_node->recv_buf, ELINK_HEADER_LEN, 0);
        if (len > 0)
        {
            //获取数据的长度
            msg_len = check_msg_header(p_client_node->recv_buf);
            if (msg_len < 0)
            {
                continue;
            }
            else
            {
                p_client_node->msg_len = 0;
                //获取mag_len长度的数据
                while(p_client_node->msg_len < msg_len)
                {
                    p_client_node->msg_len += recv(p_client_node->fd, p_client_node->recv_buf + p_client_node->msg_len, msg_len, 0);
                }
            }
            data_distribution(p_client_node);
        }
    }
}

static void *local_client_heart(void *arg)
{
    //获取fd
    p_client_node_t p_client_node = (p_client_node_t)arg;
    int8_t buf[MAX_BUF_LEN] = {0};
    int32_t len = 0;
    client_type_t type = local_client_e;

    while (1)
    {
        if (p_client_node->link_state == dev_reg_e)
        {
            log_d("send local heart!");
            //发送心跳包
            len = send_heart_msg(p_client_node);
            if (len == CONNECT_ERROR_E)
            {
                //tcp服务器已经关闭，尝试再次连接
                log_e("tcp server is closed");

                //等待client重新建立
                while (restart_authentication(p_client_node, local_client_e) != 0)
                {
                    sleep(TCP_INIT_DELAY_S);
                }
            }
        }
        sleep(LOCAL_HEART_S);
    }
}

static void *local_client(void *arg)
{
    client_type_t type = local_client_e;
    p_client_node_t p_client_node = (p_client_node_t)arg;

    //忽略管道破裂的信号
    //TODO:当本地服务器死掉的时候会发出SIGPIPE(管道破裂的信号),这个信号默认会终止进程
    //所以这里忽略此信号
    signal(SIGPIPE, SIG_IGN);

    client_init(p_client_node, type);
    p_client_node->link_state = connect_e;

    //创建接收线程
    create_local_recv_thread(p_client_node);

    //创建心跳
    create_local_heart_thread(p_client_node);

    //等待协议认证成功
    while (start_authentication(p_client_node) != 0)
    {
        sleep(TCP_INIT_DELAY_S);
    }

    //死循环
    while (1)
    {
        sleep(5);
    }
}

//数据接收线程
static void create_local_recv_thread(p_client_node_t p_client_node)
{
    //创建接收线程和心跳线程
    pthread_t local_client_recv_thread;

    if (pthread_create(&local_client_recv_thread, NULL, local_client_recv, (void *)p_client_node) == -1)
    {
        log_e("pthread_create local_client_recv_thread error\n");
    }
}

static void create_local_heart_thread(p_client_node_t p_client_node)
{
    pthread_t local_client_heart_thread;

    if (pthread_create(&local_client_heart_thread, NULL, local_client_heart, (void *)p_client_node) == -1)
    {
        log_e("pthread_create local_client_heart_thread error");
    }

    return 0;
}

void create_local_thread(p_client_node_t p_client_node)
{
    pthread_t local_client_thread;

    if (pthread_create(&local_client_thread, NULL, local_client, (void *)p_client_node) == -1)
    {
        log_e("pthread_create local_client_heart_thread error\n");
    }

    return 0;
}