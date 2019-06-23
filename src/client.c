#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client.h"
/*
TODO:客户端程序默认是和网关通信的
*/

static int32_t _get_ip_addr(int8_t *ip_addr);
static int32_t _socket_init(client_type_t type);

//获取tcp服务器地址，这里是直接获取网关地址
//用route命令获取
static int32_t _get_ip_addr(int8_t *ip_addr)
{
    int32_t ret = 0;
    int8_t buf[128] = {0};
    int8_t cmd_shell[128] = {0};

    sprintf(cmd_shell, "ip route show | grep default | awk '{print $3}'");
    
    int8_t *cmd[] = {"sh", "-c", cmd_shell, (int8_t *)NULL};

    if (mk_popen(cmd[0], cmd, &ret, buf, sizeof(buf)) < 0)
    {
        log_e("Fail to run cmd");
        return -1;
    }
    strcpy(ip_addr, buf);

    return 0;
}

static int32_t _local_socket_init()
{
    int32_t fd = 0;

    struct sockaddr_un remote;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    log_i("trying to connect to local server!");
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, ELINK_UNIX_SOCKET);
    int32_t len = strlen(remote.sun_path) + sizeof(remote.sun_family);

    if(connect(fd, (struct sockaddr *)&remote, len) == -1)
    {
        log_e("connet to local server error!");
        return -1;
    }

    return fd;
}

static int32_t _remote_socket_init()
{

    int8_t ip_addr[16] = {0};
    
    //获取网关地址
    _get_ip_addr(ip_addr);
    log_d("remote ip addr is %s!", ip_addr);

    //创建socket
    int32_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd > 0)
    {
        //设置为非阻塞状态
        //fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
        //设置端口可重用
        int32_t opt = 1;
        //setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        //初始化socket
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_aton(ip_addr, (struct sockaddr *)&addr.sin_addr.s_addr);
        addr.sin_port = htons(REMOTE_TCP_PORT);

        //连接不成功 一直尝试
        if (connect(fd, (struct sockaddr *)(&addr), sizeof(addr)) != 0)
        {
            perror("====connect");
            log_e("remote socket connect error!");
            return -1;
        }
    }
    return fd;
}

static int32_t _client_node_init(client_type_t type)
{
    int32_t fd = -1;

    //远程socket连接
    if (type == remote_client_e)
    {
        log_i("start init remote_socket!");
        while ((fd = _remote_socket_init()) < 0)
        {
            log_e("remote socket tcp client init error!");
            sleep(TCP_INIT_DELAY_S);
        }
    }

    //本地socket连接
    if (type == local_client_e)
    {
        log_i("start init local socket!");
        while ((fd = _local_socket_init()) < 0)
        {
            log_e("local socket tcp client init error!");
            sleep(TCP_INIT_DELAY_S);
        }
    }

    return fd;
}


//本地客户端和client_init远程客户端初始化
void client_init(p_client_node_t p_client_node, client_type_t type)
{
    p_client_node->fd = _client_node_init(type);

}

int32_t client_send_buf(int32_t fd, int8_t *buf, int32_t len)
{
    if (fd <= 0)
    {
        return -1;
    }
    int32_t ret = send(fd, buf, len, 0);

    if (ret == 0)
    {
        //tcp服务器已经关闭，尝试再次连接
        log_e("tcp server is closed!");
        return CONNECT_ERROR_E;
    }

    else if (ret < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        {
            //这几种错误默认是正常的可以继续收
            return 0;
        }
        else
        {
            //尝试再次连接
            log_e("tcp server is closed!");
            return CONNECT_ERROR_E;
            
        }
    }

    return ret;
}

