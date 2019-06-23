# 只能网关和组网终端链接流程梳理
## 流程说明：
- 智能家庭网关上电，打开 TCP 32768 端口，等待外部连接；

- 组网终端上电，通过 WAN 口以 DHCP 方式获取到 IP 后，连接智能家庭网关的 TCP32768 端口；

- 组网终端发向智能家庭网关发起协商密钥生成方式请求，将组网终端侧支持的密钥生成方式
(DH)发送给智能家庭网关；

- 智能家庭网关在收到的组网终端的密钥生成方式中，选择智能家庭网关支持的最优密钥生成方
式(DH)，并回应给组网终端,完成双方密钥生成方式协商。

- 组网终端侧采用 DH 算法，生成公钥和私钥，发送组网终端的公钥与组网终端的设备信息给智
能家庭网关；

- 智能家庭网关通过 TCP 32768 端口收到对方的 DH 公钥后，采用 DH 算法，生成智能家庭网关的
公钥和私钥，并将公钥发送给组网终端，智能家庭网关端将组网终端的公钥与智能家庭网关的
私钥进行运算，得出共享密钥，组网终端侧与智能家庭网关端采用共享密钥加解密通讯数据，
加密方式为 AES-CBC 方式；

- 组网终端侧收到智能家庭网关的公钥后，将智能家庭网关的公钥与组网终端的私钥进行运算，
得出共享密钥，确认密钥协商过程结束；

- 组网终端向智能家庭网关发起注册请求，组网终端发送自己的设备信息给智能家庭网关，请求
智能家庭网关下发配置信息；

- 智能家庭网关回应组网终端 ACK 表示已收到该信息；

- 智能家庭网关与组网终端建立链路保活，开始 keepalive，验证双方是否在线。

- 智能家庭网关检查组网终端是否合法， 如果终端合法，智能家庭网关立即下发配置信息给组网
终端；如果组网终端不合法，智能家庭网关将请求手机 APP 向用户确认该组网终端是否合法，
由用户判断是否下发配置信息。用户确认组网终端合法后，组网终端将加入到智能家庭网关的
本地合法列表中。

- 当智能家庭网关发现本地配置变化，立即发送配置文件给组网终端，组网终端用智能家庭网关
发来的配置文件更新配置。

- 组网终端下挂设备状态上报组网终端向智能家庭网关上报当前在线下挂设备的 MAC、 IP、等信
息；

- 智能家庭网关收到组网终端上报的下挂设备信息后，回复响应消息 ACK 给组网终端，具体参数
请参见 6.1）；

- 用户修改了智能家庭网关的 LED、 WiFi 开关、 WiFi 定时等其他设置参数后，也同步到组网终
端；

- 组网终端配置完成后，并向智能家庭网关返回响应消息。




# 组网终端接入ap流程梳理

- 组网终端e-Link客户端需要同时连接两个e-Link服务器端： 智能网关e-Link插件提供的服务器端和
云客户端提供的e-Link服务器端。 e-Link客户端和组网终端云客户端之间的连接、密钥协商、注册、保
活等通信和交互过程与组网终端和智能家庭网关的交互相同。



# ap管理平台的接入流程
- 首先用ie浏览器登陆到https://180.168.75.148/login.php地址
- 启动vpn服务
- 启动vpn服务后登陆到https://apweb2.189cube.com/admin/admincenter.php,用户名和密码分别为debugtest debugtest


# ap管理平台下发json数据格式

## get_status请求(组合形式)

### 重启命令
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"cfg","set":{"ctrlcommand":"reboot"}}}
```

### 保存配置
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"cfg","set":{"ctrlcommand":"save"}}}
```

### 恢复出厂
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"cfg","set":{"ctrlcommand":"reset"}}}
```

### 获取wifi信息
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"wifi"}]}}
```

### 查询wifi开关状态
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"wifiswitch"}]}}
```

### 查询led开关状态
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"ledswitch"}]}}
```

### 查询wifi定时开关
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"wifitimer"}]}}
```

### 查询终端设备支持的频段
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"bandsupport"}]}}
```

### 查询cpu的速率
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"cpurate"}]}}
```

### 查询内存使用率
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"memoryuserate"}]}}
```

### 查询上传速率
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"uploadspeed"}]}}
```

### 查询下载速率
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"downloadspeed"}]}}
```

### 查询无线的状态
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"wlanstats"}]}}
```

### 查询无线的信道
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"channel"}]}}
```

### 查询设备的在线时长
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"onlineTime"}]}}
```

### 查询下挂终端数
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"terminalNum"}]}}
```

### 查询无线信道负载
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"load"}]}}
```

### 下挂终端去关联
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"real_devinfo"}]}}
```

### 无线信号检测
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"elinkstat"}]}}
```

### 查询ap wlan邻居信息
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"neighborinfo"}]}}
```

### 查询接入方式
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"networktype"}]}}
```

### 查询工作模式
```json
{"MAC":"40B0345078F9","RPCMethod":"SetPluginParams","Plugin_Name":"eLinkAP","Version":"1.0","Parameter":{"type":"get_status","get":[{"name":"workmode"}]}}
```
### 全部组合查询
```json
{
    "MAC": "40B0345078F9",
    "RPCMethod": "SetPluginParams",
    "Plugin_Name": "eLinkAP",
    "Version": "1.0",
    "Parameter": {
        "type": "get_status",
        "get": [
            {
                "name": "wifi"
            },
            {
                "name": "wifiswitch"
            },
            {
                "name": "wifitimer"
            },
            {
                "name": "bandsupport"
            },
            {
                "name": "cpurate"
            },
            {
                "name": "memoryuserate"
            },
            {
                "name": "uploadspeed"
            },
            {
                "name": "downloadspeed"
            },
            {
                "name": "onlineTime"
            },
            {
                "name": "terminalNum"
            },
            {
                "name": "networktype"
            },
            {
                "name": "workmode"
            },
            {
                "name": "real_devinfo"
            },
            {
                "name": "channel"
            }
        ]
    }
}
```



