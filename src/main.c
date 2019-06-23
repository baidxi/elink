#include "client.h"
#include "dhkey.h"
#include "remote.h"
#include "local.h"

client_t g_client;

/*
组网终端客户端需要连接两个tcp服务器，一个网关的服务器(ip地址通过dhcp拿到)。
一个本地的服务器，用于服务转换(ip地址是本地ip)，两个服务器监听的端口都是32768端口

*/
static void _easyloger_init();

int32_t main()
{
    //日志系统初始化
    _easyloger_init();

    create_remote_thread(&g_client.remote_node);
    create_local_thread(&g_client.local_node);

    while (1)
    {
        sleep(5);
    }
    return 0;
}

static void _easyloger_init()
{
    /* close printf buffer */
    setbuf(stdout, NULL);

    /* initialize EasyLogger */
    elog_init();

    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL);

#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();
}
