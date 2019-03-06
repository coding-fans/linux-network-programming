.. 梅林科学上网秘籍
    FileName:   anti-gfw.rst
    Author:     Fasion Chan
    Created:    2019-02-28 19:20:23
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        Shadowsocks是一个稳定可靠的科学上网技术方案，需要安装代理客户端，配置繁琐。
        路由器刷上梅林(Merlin)固件，然后部署科学上网透明代理，可完美解决该问题。
        本文详细介绍这种方法，涵盖技术原理和配置方法两部分。
    :keywords: merlin, shadowsocks, ss-redir, ss-server, dnsmasq, tproxy, iptables, redirect, ipset, gfwlist, tcp, udp, ip rule, ip route

================
梅林科学上网秘籍
================

:doc:`/services/shadowsocks` 是一个稳定可靠的科学上网技术方案，
需要安装代理客户端，配置繁琐。
路由器刷上 **梅林** ( `Merlin`_ )固件，然后部署科学上网 **透明代理** ，可完美解决该问题。

本文详细介绍这种方法，涵盖 :ref:`merlin-anti-gfw-principle` 和 :ref:`merlin-anti-gfw-steps` 两部分。

.. note::

    **梅林** 路由固件很流行，是 **华硕** ( `ASUS`_ ) 官方固件衍生版本，属于半官方固件。
    与其他路由固件相比， **梅林** 运行更稳定，技术资料也更丰富。

硬件选型
========

由于 **梅林** 对华硕自家路由支持更稳定，因此 **强烈建议选用华硕路由** ，降低折腾成本。
参考 **梅林**  `固件下载页面`_ 上的支持型号，从中挑选一个性价比合适的即可。

本文选用 `RT-AC68U`_ ，其他支持型号包括(截止本文撰写)：

- `RT-AC3100`_
- `RT-AC3200`_
- `RT-AC5300`_
- `RT-AC68U`_
- `RT-AC86U`_
- `RT-AC87U`_
- `RT-AC88U`_
- `RT-AX88U`_

.. _merlin-anti-gfw-principle:

技术原理
========

开始学习 :ref:`merlin-anti-gfw-steps` 之前，先看看透明代理涉及的技术组件及工作原理。

.. figure:: /_images/services/merlin/anti-gfw/2b2a753196dc5087e6519268953ce0cd.png

如上图，蓝色部分为 **路由** 内部组件； **绿色** 部分为在路由下的客户机；
**紫色** 部分运行在墙外的 :doc:`/services/shadowsocks` :ref:`ss-server` ；
**黄色** 部分是被墙挡住的各种网络服务。

路由需要部署 `ss-redir` 模块，用于接收通过 `iptables`_ **重定向** ( `REDIRECT` )的 `TCP` 流量
和 `iptables` 透明代理 ( `TPROXY` )的 `UDP` 流量，
并通过 `ss-server` 转发出去。

由于被 `GFW` 墙的 `IP` 数量不少，用 `ipset`_ 创建一个 `IP集合`_ 进行管理更灵活高效，如图中的 `ss-ips` 。
`iptables` 根据该集合判断网络报文是否需要转发给 `ss-redir` ，
因此只有被墙的 `IP` 流量走代理，其他流量走默认网关。

`dnsmasq` 模块接管客户机 `DNS` 查询，防止 `GFW` 污染域名，并维护 `ss-ips` 集合。
`dnsmasq` 从 `gfwlist` 获得一份受 `GFW` 影响的域名，
如果 `DNS` 查询 `gfwlist` 中域名，则到 *8.8.8.8* 进行查询，并将查询结果返回给客户端。
`dnsmasq` 到 *8.8.8.8* 的 `DNS` 查询为普通 `UDP` 流量，只要 *8.8.8.8* 在 `ss-ips` 中，即可通过 `ss-redir` 转发出去，避免污染。

此外， `dnsmasq` 将 `gfwlist` 域名查询结果添加到 `ss-ips` 集合中。
这样一来，客户机后续到该 `IP` 的流量，均走 `ss-redir` 代理出去。
非常巧妙！

.. _merlin-anti-gfw-steps:

配置方法
========

常量说明
--------

.. csv-table:: 常量说明
    :header: "常量", "值", "说明"

    "ss-redir 端口", "29555", "字符串 `ss` 的二进制值"
    "UDP TProxy 标记", "0x7373/0x7373", "字符串 `ss` 的十六进制值"
    "UDP TProxy 路由编号", "0x5353", "字符串 `SS` 的十六进制值"

设置WiFi密码
------------

新路由启动后需要先设置 `WiFi` 以及 管理密码，操作方式请参考产品说明书，不再赘述。
如已完成此步骤，请跳过。

刷固件
------

在 `固件下载页面`_ 找到合适的固件版本、下载、并检查签名：

.. code-block:: shell-session

    $ shasum -a 256 RT-AC68U_384.9_0.trx
    d55e514b550430a528801d3e5c23b3932a3038094914db8ae43484d36355e2a2  RT-AC68U_384.9_0.trx

然后，打开路由管理界面，进入 *系统管理* 、 *固件升级* ，上传固件文件即可：

.. figure:: /_images/services/merlin/anti-gfw/f5d96be6de654a4ef65bebab5cd31e29.png
    :width: 512px

设置JFFS分区
------------

华硕路由内置了用于持久存储配置、脚本的闪存，文件系统为 `JFFS` ，挂载在 `/jffs` 目录。
`JFFS` 分区默认不开放写权限，需要在路由管理界面开启。

点击 *系统管理* 、 *系统设置* ，找到 *Persistent JFFS2 partition* 一节：

.. figure:: /_images/services/merlin/anti-gfw/cdf98ae07152776dc5c93de2146dc9ff.png
    :width: 500px

将 *Enable JFFS custom scripts and config* 设为是。

启动SSH服务
-----------

点击 *系统管理* 、 *系统设置* ，找到 *服务* 一节，开始设置 `SSH` 服务：

.. figure:: /_images/services/merlin/anti-gfw/a2a8ec24265b7950f5143c3d548b4537.png
    :width: 500px

如图， `SSH` 只对本地网络开放( `LAN only` )，并通过密钥授权。
如果不提供授权密钥，还可以通过路由管理账号及密码登录； **密钥授权更推荐** 。

登录
----

用路由管理账号，以 `SSH` 登录路由器：

.. code-block:: shell-session

    fasion@FasionAir:~$ ssh admin@router.asus.com
    admin@RT-AC68U-7CA8:/tmp/home/root# echo yeah

拿到 `SSH` 登录权限，后续就可以为所欲为了！

.. note::

    路由管理账号，即用于登录路由管理界面 `https://router.asus.com <https://router.asus.com>`_ 的账号。

JFFS分区简介
------------

路由分区表大致如下：

.. code-block:: shell-session

    admin@RT-AC68U-7CA8:/tmp/home/root# df
    Filesystem           1K-blocks      Used Available Use% Mounted on
    /dev/root                35456     35456         0 100% /
    devtmpfs                127744         0    127744   0% /dev
    tmpfs                   127848       660    127188   1% /tmp
    /dev/mtdblock4           64256      1760     62496   3% /jffs

其中， `/` 为根文件系统； `/dev/` 为设备文件系统； `/tmp` 为临时文件系统，重启即失效；
`/jffs` 为闪存文件系统，用于 **存储配置文件或脚本** 。

.. note::

    由于 `Shell` 输入提示串 ``admin@RT-AC68U-7CA8:/tmp/home/root#`` 有些冗长，
    后续将视情况作省略。
    如无特殊说明，以下操作均系操作路由器。

看看 `/jffs` 里头都放了啥：

.. code-block:: shell-session

    admin@RT:/tmp/home/root# cd /jffs/
    admin@RT:/jffs# ls
    configs         nmp_cl_json.js  scripts         syslog.log
    admin@RT:/jffs# ls scripts
    admin@RT:/jffs# ls configs

`config` 为配置文件目录，目前为空； `scripts` 为钩子脚本目录，目前也为空；
`syslog.log` 为系统日志文件。

接下来，我们将看到 `dnsmasq` `DNS` 解析规则配置文件将放置于 `config` 目录下；
而 `Entware`_ 相关 **钩子脚本** 将放置于 `scripts` 目录下。

.. note::

    **钩子脚本** 更深入介绍请查看 `Merlin Wiki <https://github.com/RMerl/asuswrt-merlin/wiki>`_ ：
    `User scripts <https://github.com/RMerl/asuswrt-merlin/wiki/User-scripts>`_ 。

插入U盘
-------

路由自身存储空间有限，需要借助 `U盘` 等外接介质存储安装的软件( `Entware`_ 要用 )。

`U盘` 插入路由 `USB` 接口后，系统将自动 `mount` 到 `/mnt` 下某个目录，目录与设备同名：

.. code-block:: shell-session

    admin@RT# df
    Filesystem           1K-blocks      Used Available Use% Mounted on
    /dev/root                35456     35456         0 100% /
    devtmpfs                127744         0    127744   0% /dev
    tmpfs                   127848       700    127148   1% /tmp
    /dev/mtdblock4           64256      1788     62468   3% /jffs
    /dev/sda1              7698460     82856   7224536   1% /tmp/mnt/sda1

由此可见， `U盘` 设备文件为 `/dev/sda1` ，挂载到 `/tmp/mnt/sda1` 目录下。

进入 `U盘` 文件系统，可以看到其中存储的文件(如有)：

.. code-block:: shell-session

    admin@RT# ls /mnt/sda1

安装Entware
-----------

`Entware`_ 是嵌入式设备的软件包管理工具，类似 `Debian` 下的 `apt` ，不可或缺。

`Merlin`_ 固件已内置了 `Entware`_ 初始化脚本，直接执行即可：

.. code-block:: shell-session

    admin@RT# entware-setup.sh
     Info:  This script will guide you through the Entware installation.
     Info:  Script modifies "entware" folder only on the chosen drive,
     Info:  no other data will be changed. Existing installation will be
     Info:  replaced with this one. Also some start scripts will be installed,
     Info:  the old ones will be saved on Entware partition with name
     Info:  like /tmp/mnt/sda1/jffs_scripts_backup.tgz

     Info:  Looking for available partitions...
    [1] --> /tmp/mnt/sda1
     =>  Please enter partition number or 0 to exit

需要选一个可用的外接存储分区，这里选 `U盘` 分区 ( `/tmp/mnt/sda1` )，输入 `1` 。

接下来，脚本将下载安装各种依赖库，由于下载站点在境外，速度可能很慢，耐心等。
如果下载失败或者速度无法忍受，可以设置一个可用的代理(如果手头刚好有)：

.. code-block:: shell-session

    admin@RT# export http_proxy=xxx.xxx.xxx.xxx:xxxx
    admin@RT# entware-setup.sh

安装软件包
----------

`Entware`_ 提供了装包工具 `opkg` ，软件安装 `So easy` ！

先安装 `shadowsocks-libev` 系列工具：

.. code-block:: shell-session

    admin@RT# opkg install shadowsocks-libev-ss-local
    admin@RT# opkg install shadowsocks-libev-ss-redir
    admin@RT# opkg install shadowsocks-libev-ss-tunnel

.. note::

    本文只用到 `ss-redir` ，其余两个亦可不装。

安装网络工具 `tcpdump` 和 `dig` ：

.. code-block:: shell-session

    admin@RT# opkg install tcpdump
    admin@RT# opkg install bind-dig

部署 ss-redir
-------------

`ss-redir` 负责将 `TCP` 和 `UDP` 流量转发到 :ref:`ss-server <ss-server>` ，配置方法与 :ref:`ss-local <ss-local>` 类似。

先编辑配置文件 `/opt/etc/shadowsocks.json` ，指定 **ss-server地址** 、 **本地监听地址** 、
**加密方式** 以及 **超时时间** ：

.. code-block:: json

    {
        "server": "example.com or X.X.X.X",
        "server_port": 443,

        "local_address": "0.0.0.0",
        "local_port": 29555,

        "method": "aes-256-cfb",
        "password": "password",

        "timeout": 300
    }

现在路由命令行下启动 `ss-redir` 服务， `-u` 开始 `UDP` 转发， `-v` 输出调试信息：

.. code-block:: shell-session

    admin@RT# ss-redir -c /opt/etc/shadowsocks.json -u -v

.. note::

    如果不用 `screen` 等工具，你可能需要新开一个登陆会话继续下面的操作。

IP集合
------

创建名为 `ss-ips` 的 **哈希型** `IP集合`_ ，并将 *8.8.8.8* 加进去：

.. code-block:: shell-session

    admin@RT# ipset -N ss-ips hash:ip
    admin@RT# ipset -A ss-ips 8.8.8.8

以下是等价命令，只是命令行参数风格不同，至于选哪种则看个人喜好：

.. code-block:: shell-session

    admin@RT# ipset create ss-ips hash:ip
    admin@RT# ipset add ss-ips 8.8.8.8

UDP转发规则
-----------

`UDP` 转发依赖 `TProxy` 相关内核模块，先确认内核已支持：

.. code-block:: shell-session

    # modprobe xt_TPROXY

`iptables` 新增 `TProxy` 规则将去往 `ss-ips` 的 `UDP` 报文转发到 `ss-redir` 服务 *29555* 端口，并打上标签 *0x7373/0x7373* (用于路由策略) 。

.. code-block:: shell-session

    # iptables -t mangle -A PREROUTING -p udp -m set --match-set ss-ips dst -j TPROXY --on-port 29555 --tproxy-mark 0x7373/0x7373

新增 `IP` 路由策略，将带有 *0x7373/0x7373* 标签的流量送到环回设备 `lo` ：

.. code-block:: shell-session

    # ip route add local 0.0.0.0/0 dev lo table 5353
    # ip rule add fwmark 0x7373/0x7373 lookup 5353

其中，第一条命令新增一条路由条目，将报文送到 `lo` ，编号为 *5353* ；
第二条命令新增一条规则，规定被 `iptables` 打上 *0x7373/0x7373* 标签的报文查找 *5353* 号路由。

如果仅需转发挂在路由下设备的 `UDP` 流量，以上操作足矣。
但为了同时转发路由本身的流量，需要同时在 `OUTPUT` 链上打标签。
打标签目的只是让本地流量走策略路由，因此标签值无关紧要。

.. code-block:: shell-session

    # iptables -t mangle -A OUTPUT -p udp -m set --match-set ss-ips dst -j MARK --set-mark 0xffff/0xffff

.. note::

    为了防止域名污染，需要配置 `dnsmasq` ，将受污染域名转到 *8.8.8.8* 上查询。
    因此，需要转发路由自身的 `UDP` 流量。

好了，用 `dig` 命令向 *8.8.8.8* 查询 `Google` 的域名，以此验证配置效果：

.. code-block:: shell-session

    # dig @8.8.8.8 www.google.com

    ;; ANSWER SECTION:
    www.google.com.         288     IN      A       172.217.6.36

如无意外， `dig` 将返回正确的地址；同时，以 `-v` 参数部署的 `ss-redir` 将输出相关日志。
至此， `UDP` 流量转发规则配置成功！

TCP转发规则
-----------

`TCP` 转发规则更简单，将去往 *ss-ips* 的 `TCP` 报文 **重定向** 到 `ss-redir`  *29555* 端口即可：

.. code-block:: shell-session

    # iptables -t nat -A PREROUTING -p tcp -m set --match-set ss-ips dst -j REDIRECT --to-ports 29555
    # iptables -t nat -A OUTPUT -p tcp -m set --match-set ss-ips dst -j REDIRECT --to-ports 29555

将上一步查询得到的 `Google` `IP` 地址加入 *ss-ips* 集合，并用 :doc:`/toolkit/curl` 请求该地址：

.. code-block:: shell-session

    # ipset -A ss-ips 172.217.6.36
    # curl 172.217.6.36
    <HTML><HEAD><meta http-equiv="content-type" content="text/html;charset=utf-8">
    <TITLE>301 Moved</TITLE></HEAD><BODY>
    <H1>301 Moved</H1>
    The document has moved
    <A HREF="http://www.google.com/">here</A>.
    </BODY></HTML>

可见，连接已经通了！
至此， `TCP` 流量转发规则配置成功！

Dnsmasq域名查询策略
-------------------

`dnsmasq` 是一个轻量级的网络基础设施，普通路由用它来实现 `DNS` 查询策略。
`dnsmasq` 监听 *53* 端口，并对路由下设备提供 `DNS` 服务。

`dnsmasq` 代客户端查询上级 `DNS` 服务器(运营商提供)，也配置指定域名的查询服务器。

.. code-block:: text

    server=/google.com/8.8.8.8

这个配置告诉 `dnsmasq` ， *google.com* 域名(包括子域名)到 *8.8.8.8* 查询。

这样，一旦客户端向 `dnsmasq` 查询 *google.com* ， `dnsmasq` 便到 *8.8.8.8* 查询并返回。
由于 *8.8.8.8* 在 *ss-ips* 内，查询产生的 `UDP` 流量将走 `ss-redir` 代理，免受污染。

此外，还需配置 `dnsmasq` 将查询返回的 `IP` 地址加到 *ss-ips* 中：

.. code-block:: text

    ipset=/google.com/ss-ips

这样，客户端与目标 `IP` 的所有 `TCP` 或 `UDP` 流量将走 `ss-redir` 代理出去。

总结起来，对于每个被墙的域名，需要写两条规则：

.. code-block:: text

    server=/google.com/8.8.8.8
    ipset=/google.com/ss-ips

配置准备好后，写到 `/jffs/configs/dnsmasq.conf.add` 文件并重启 `dnsmasq` 服务：

.. code-block:: shell-session

    # cat > /jffs/configs/dnsmasq.conf.add <<EOF
    server=/google.com/8.8.8.8
    ipset=/google.com/ss-ips
    EOF
    # service restart_dnsmasq

之后，用 `dig` 命令确认 `dnsmasq` 可以正确处理 `google.com` 的查询了：

.. code-block:: shell-session

    # dig www.google.com

至于被 `GFW` 封杀的域名有哪些，可以看看 `gfwlist`_ 项目，从中生成一份即可。

笔者更倾向自行维护一个小型的列表，包括 `Google` 、 `Youtube` 、 `Facebook` 等常用站点即可。

自动化
------

前面介绍的操作都是纯手工实施的，而且路由重启后将失效，一点也不完美。
我们需要将以上操作通过脚本等方式固化下来，随路由启动而执行，避免重复劳动。

ss-redir服务化
++++++++++++++

首先将 `ss-redir` 服务化，以便随路由一起启动。
`ss-redir` 已提供了服务脚本，路径为 */opt/etc/init.d/S22shadowsocks* 。
仅需调整其中命令行参数部分，示例如下：

.. code-block:: text

    PROCS=ss-redir
    ARGS="-c /opt/etc/shadowsocks.json -u"

修改完毕后，可尝试启动服务：

.. code-block:: shell-session

    # /opt/etc/init.d/S22shadowsocks start

检查启动状态：

.. code-block:: shell-session

    # /opt/etc/init.d/S22shadowsocks check

后续，就算路由重启， `ss-redir` 服务也会自动运行，无须手工启动。

钩子脚本
++++++++

`Merlin`_ 支持自定义钩子脚本，系统在特定时间点调用。
在 `/jffs/scripts` 目录，可以找到初始化 `Entware` 时创建的脚本。
常用 **钩子脚本** 及对应 **调用时间** 点列举如下：

- `server_start` ，路由服务启动完毕后被调用；
- `nat-start` ，路由更新 `iptables` `nat` 表后调用，可在此添加自定义 `nat` 规则；
- `firewall-start` ，路由开启防火墙后调用，可在此添加非 `nat` 规则；

因此，可将 `ipset` 、 `iptables` 等命令写到钩子脚本，随路由启动而执行。

随路由启动执行部分放入 `/jffs/scripts/server-start` ：

.. code-block:: shell

    ipset -N ss-ips hash:ip
    ipset -A ss-ips 8.8.8.8

    ip route add local 0.0.0.0/0 dev lo table 5353
    ip rule add fwmark 0x7373/0x7373 lookup 5353

`mangle` 规则放入 `/jffs/scripts/firewall-start` ：

.. code-block:: shell

    if modprobe xt_TPROXY; then
        iptables -t mangle -A PREROUTING -p udp -m set --match-set ss-ips dst -j TPROXY --on-port 29555 --tproxy-mark 0x7373/0x7373
        iptables -t mangle -A OUTPUT -p udp -m set --match-set ss-ips dst -j MARK --set-mark 0xffff/0xffff
    fi

`nat` 规则放入 `/jffs/scripts/nat-start` ：

.. code-block:: shell

    iptables -t nat -A PREROUTING -p tcp -m set --match-set ss-ips dst -j REDIRECT --to-ports 29555
    iptables -t nat -A OUTPUT -p tcp -m set --match-set ss-ips dst -j REDIRECT --to-ports 29555

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `AsusWRT/Merlin Wiki <https://github.com/RMerl/asuswrt-merlin/wiki>`_
#. `from-ss-redir-to-linux-nat <https://vvl.me/2018/06/09/from-ss-redir-to-linux-nat/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _ASUS: https://www.asus.com/
.. _Entware: https://github.com/Entware/Entware
.. _gfwlist: https://github.com/gfwlist/gfwlist
.. _ipset: http://ipset.netfilter.org/ipset.man.html
.. _iptables: http://man7.org/linux/man-pages/man8/iptables.8.html
.. _IP集合: http://ipset.netfilter.org/
.. _Merlin: https://asuswrt.lostrealm.ca/
.. _REDIRECT: http://man7.org/linux/man-pages/man8/iptables.8.html

.. _固件下载页面: https://asuswrt.lostrealm.ca/download

.. _RT-AC3100: https://www.asus.com/us/Networking/RT-AC3100/
.. _RT-AC3200: https://www.asus.com/us/Networking/RTAC3200/
.. _RT-AC5300: https://www.asus.com.cn/Networking/RT-AC5300/
.. _RT-AC68U: https://www.asus.com.cn/Networking/RTAC68U/
.. _RT-AC86U: https://www.asus.com.cn/Networking/RT-AC86U/
.. _RT-AC87U: https://www.asus.com.cn/Networking/RTAC87U/
.. _RT-AC88U: https://www.asus.com.cn/Networking/RT-AC88U/
.. _RT-AX88U: https://www.asus.com.cn/Networking/RT-AX88U/

.. comments
    comment something out below

