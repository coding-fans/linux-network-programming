.. 实验：Linux路由
    FileName:   experiment-linux-routing.rst
    Author:     Fasion Chan
    Created:    2019-02-17 16:08:00
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        本实验将一台Linux服务器配置成可转发IP报文的路由器，并观察报文转发行为。
        通过实验，读者可进一步加深对协议栈网络层的理解，
        明确IP协议全局寻址和数据路由两大核心功能，
        理解路由表的工作原理并掌握相关工具命令的用法。
    :keywords: linux, routing, route, net.ipv4.ip_forward, 路由, 路由表

===============
实验：Linux路由
===============

本实验将一台 `Linux`_ 服务器配置成可转发 `IP` 报文的路由器，并观察报文转发行为。
通过实验，读者可进一步加深对协议栈 :doc:`network-layer` 的理解，
明确 :doc:`ip` **全局寻址** 和 **数据路由** 两大核心功能，
理解路由表的工作原理并掌握相关工具命令的用法。

环境
====

通过 `Virtualbox`_ 创建 *3* 台虚拟机，分别是 *ant* 、 *bee* 、 *cicada* 。
虚拟机通过 **内部网络** 分别连到两个独立的网络 *lan0* 和 *lan1* ，网络结构如下：

.. figure:: /_images/protocols/experiment-linux-routing/67c7cba4b4aed0b856624a30ffce8f41.png

其中， *bee* 同时连到 *lan0* 和 *lan1* 。 *3* 台虚拟机地址见下表：

.. csv-table:: 表-1 实验虚拟机
    :header: "机器名", "lan0", "lan1"

    "Ant", "10.0.1.2", "\-"
    "Bee", "10.0.1.1", "192.168.1.1"
    "Cicada", "\-", "192.168.1.2"

步骤
====

根据 :doc:`data-link-layer` 知识，直接连接 *lan0* 或 *lan1*  的虚拟机间可以互相通讯。
因此， *ant* 可以 `ping` 通 *bee* ：

.. code-block:: shell-session

    fasion@ant:~$ ping 10.0.1.1
    PING 10.0.1.1 (10.0.1.1) 56(84) bytes of data.
    64 bytes from 10.0.1.1: icmp_seq=1 ttl=64 time=1.84 ms
    64 bytes from 10.0.1.1: icmp_seq=2 ttl=64 time=0.797 ms
    ^C
    --- 10.0.1.1 ping statistics ---
    2 packets transmitted, 2 received, 0% packet loss, time 1002ms
    rtt min/avg/max/mdev = 0.797/1.322/1.848/0.526 ms

当然了， *bee* 也可以 `ping` 通 *cicada* 。

依据网络结构图不难发现， *ant* 可以借助 *bee* 与 *cicada* 通讯，只需实现：

#. *ant* 先将报文发给 *bee* ；
#. *bee* 负责转发；

实现第 *1* 点，需借助 **路由表** ( `route table` )。
路由表是 **路由器** ( `router` )维护的一个映射表，用于查询报文下一跳，路由器据此转发报文。

初学者可能无法理解，没有转发功能的普通机器也需要路由表。
如果一台机器有两块网卡，其中一块连接 **外网** ，另一块连接 **外网** ，系统如何判断报文从哪块网卡发送出去？
答案还是 **路由表** 。

以 *ant* 为例，可以通过 `route` 命令查询机器当前路由表：

.. code-block:: shell-session

    fasion@ant:~$ route -n
    Kernel IP routing table
    Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
    0.0.0.0         10.0.2.2        0.0.0.0         UG    100    0        0 enp0s3
    10.0.0.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s8
    10.0.1.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s9
    10.0.2.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s3
    10.0.2.2        0.0.0.0         255.255.255.255 UH    100    0        0 enp0s3

现在，为 *ant* 添加一条路由，目的地址为 *192.168.1.0/24* 的报文下一跳发往 *10.0.1.1* ：

.. code-block:: shell-session

    root@ant:~$ route add -net 192.168.1.0/24 gw 10.0.1.1

再次确认路由添加完毕：

.. code-block:: shell-session

    fasion@ant:~$ route -n
    Kernel IP routing table
    Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
    0.0.0.0         10.0.2.2        0.0.0.0         UG    100    0        0 enp0s3
    10.0.0.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s8
    10.0.1.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s9
    10.0.2.0        0.0.0.0         255.255.255.0   U     0      0        0 enp0s3
    10.0.2.2        0.0.0.0         255.255.255.255 UH    100    0        0 enp0s3
    192.168.1.0     10.0.1.1        255.255.255.0   UG    0      0        0 enp0s9

同理， *cicada* 需要添加一条 *10.0.1.0/24* 网段的回程路由：

.. code-block:: shell-session

    root@cicada:~$ route add -net 10.0.1.0/24 gw 192.168.1.1

现在 `ping` *cicada* ，发现还是不通呀：

.. code-block:: shell-session

    fasion@ant:~$ ping 192.168.1.2
    PING 192.168.1.2 (192.168.1.2) 56(84) bytes of data.
    ^C
    --- 192.168.1.2 ping statistics ---
    2 packets transmitted, 0 received, 100% packet loss, time 1010ms

接着，在 *bee* 上运行 :doc:`/toolkit/tcpdump` 命令抓包，发现 *ant* 的报文确实到达 *bee* 了：

.. code-block:: shell-session

    root@bee:~$ tcpdump -ni any icmp
    tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
    listening on any, link-type LINUX_SLL (Linux cooked), capture size 262144 bytes
    08:56:53.200259 IP 10.0.1.2 > 192.168.1.2: ICMP echo request, id 1589, seq 1, length 64
    08:56:54.202828 IP 10.0.1.2 > 192.168.1.2: ICMP echo request, id 1589, seq 2, length 64

`ping` *bee* 另一个接口( *lan1* )的地址，发现已经通了：

.. code-block:: shell-session

    fasion@ant:~$ ping 192.168.1.1
    PING 192.168.1.1 (192.168.1.1) 56(84) bytes of data.
    64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=0.618 ms
    64 bytes from 192.168.1.1: icmp_seq=2 ttl=64 time=0.453 ms
    ^C
    --- 192.168.1.1 ping statistics ---
    2 packets transmitted, 2 received, 0% packet loss, time 1003ms
    rtt min/avg/max/mdev = 0.453/0.535/0.618/0.085 ms

在 *cicada* 上抓包，发现 *bee* 并没有将报文转发过来：

.. code-block:: shell-session

    root@cicada:~$ tcpdump -ni any icmp
    listening on any, link-type LINUX_SLL (Linux cooked), capture size 262144 bytes

`Linux`_ 有一个 `内核参数`_ ( `net.ipv4.ip_forward` )控制是否转发 `IP` 报文，默认设为 **否** ( *0* )。
修改该内核参数即可开启转发功能：

.. code-block:: shell-session

    root@bee:~# echo 1 > /proc/sys/net/ipv4/ip_forward

终于， *ant* `ping` 通了 *cicada* ：

.. code-block:: shell-session

    fasion@ant:~$ ping 192.168.1.2
    PING 192.168.1.2 (192.168.1.2) 56(84) bytes of data.
    64 bytes from 192.168.1.2: icmp_seq=1 ttl=63 time=1.58 ms
    64 bytes from 192.168.1.2: icmp_seq=2 ttl=63 time=1.24 ms
    ^C
    --- 192.168.1.2 ping statistics ---
    2 packets transmitted, 2 received, 0% packet loss, time 1002ms
    rtt min/avg/max/mdev = 1.244/1.414/1.584/0.170 ms

附录
====

ant
---

第一台机器 *ant* ，配置了 *3* 块网卡。

第一块网卡 `enp0s3` 为 **网络地址转换** ，用于让虚拟机通过母机 `NAT` 上网：

.. figure:: /_images/protocols/experiment-linux-routing/d736840c59690004cf61687db83736eb.png
    :width: 668px

第二块网卡 *enp0s8* 为 **仅主机网络** ( `Host-Only` )，用于虚拟机与母机通讯：

.. figure:: /_images/protocols/experiment-linux-routing/50aa9738351bf0d1a92d880542933bc7.png
    :width: 668px

母机可通过该网络访问虚拟机，例如使用 `ssh` 命令登录虚拟机等。

前两块网卡主要用途是虚拟机管理，与本实验关系不大。
第 *3* 块网卡 *enp0s9* ，为 **内部网络** ，将 *ant* 与本实验的 *lan0* 网络相连：

.. figure:: /_images/protocols/experiment-linux-routing/e6abba96aadc0edda29aa4df235d981a.png
    :width: 668px

*ant* 网卡地址配置由 `Netplan`_ 管理，配置如下：

.. code-block:: yaml

    network:
        ethernets:
            enp0s3:
                addresses: []
                dhcp4: true
            enp0s8:
                addresses: [10.0.0.2/24]
            enp0s9:
                addresses: [10.0.1.2/24]
        version: 2

*ant* 各网卡地址信息如下：

.. code-block:: shell-session

    fasion@ant:~$ ip addr
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host
           valid_lft forever preferred_lft forever
    2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:36:1b:57 brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic enp0s3
           valid_lft 85405sec preferred_lft 85405sec
        inet6 fe80::a00:27ff:fe36:1b57/64 scope link
           valid_lft forever preferred_lft forever
    3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:68:c5:a1 brd ff:ff:ff:ff:ff:ff
        inet 10.0.0.2/24 brd 10.0.0.255 scope global enp0s8
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe68:c5a1/64 scope link
           valid_lft forever preferred_lft forever
    6: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:02:b5:d0 brd ff:ff:ff:ff:ff:ff
        inet 10.0.1.2/24 brd 10.0.1.255 scope global enp0s9
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe02:b5d0/64 scope link
           valid_lft forever preferred_lft forever

.. note::

    `Virtualbox`_ 虚拟网卡与虚拟机内看到的网卡名可以通过 `MAC` 地址对应起来。
    `Virtualbox` 可查到虚拟网卡的 `MAC` 地址：

    .. figure:: /_images/protocols/experiment-linux-routing/d53d263de098a395a56d790fdcd02398.png
        :width: 668px

bee
---

第二台机器 *bee* 前两块网卡 *enp0s3* 、 *enp0s8* 与 *ant* 一样，不再赘述。

第 *3* 块网卡 *enp0s9* 为 **内部网络** ，将 *bee* 与 *lan0* 网络相连；
第 *4* 块网卡 *enp0s10* 也是 **内部网络** ，将 *bee* 与 *lan1* 网络相连：

.. figure:: /_images/protocols/experiment-linux-routing/d065b9ab37eef77473f1f796667e062e.png
    :width: 668px

*bee* 网卡地址配置由 `Netplan`_ 管理，配置如下：

.. code-block:: yaml

    network:
        ethernets:
            enp0s3:
                addresses: []
                dhcp4: true
            enp0s8:
                addresses: [10.0.0.3/24]
            enp0s9:
                addresses: [10.0.1.1/24]
            enp0s10:
                addresses: [192.168.1.1/24]
        version: 2

*bee* 各网卡地址信息如下：

.. code-block:: shell-session

    fasion@bee:~$ ip addr
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host
           valid_lft forever preferred_lft forever
    2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:7c:5b:52 brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic enp0s3
           valid_lft 86252sec preferred_lft 86252sec
        inet6 fe80::a00:27ff:fe7c:5b52/64 scope link
           valid_lft forever preferred_lft forever
    6: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:4f:73:cf brd ff:ff:ff:ff:ff:ff
        inet 10.0.0.3/24 brd 10.0.0.255 scope global enp0s8
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe4f:73cf/64 scope link
           valid_lft forever preferred_lft forever
    9: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:b4:8f:ff brd ff:ff:ff:ff:ff:ff
        inet 10.0.1.1/24 brd 10.0.1.255 scope global enp0s9
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:feb4:8fff/64 scope link
           valid_lft forever preferred_lft forever
    11: enp0s10: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:9d:ae:3b brd ff:ff:ff:ff:ff:ff
        inet 192.168.1.1/24 brd 192.168.1.255 scope global enp0s10
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe9d:ae3b/64 scope link
           valid_lft forever preferred_lft forever

cicada
------

第二台机器 *cicada* 前两块网卡 *enp0s3* 、 *enp0s8* 与 *ant* 一样，不再赘述。

第 *3* 块网卡 *enp0s9* 为 **内部网络** ，将 *cicada* 与 *lan1* 网络相连。

*cicada* 网卡地址配置由 `Netplan`_ 管理，配置如下：

.. code-block:: yaml

    network:
        ethernets:
            enp0s3:
                addresses: []
                dhcp4: true
            enp0s8:
                addresses: [10.0.0.4/24]
            enp0s9:
                addresses: [192.168.1.2/24]
        version: 2

*cicada* 各网卡地址信息如下：

.. code-block:: shell-session

    fasion@cicada:~$ ip addr
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host
           valid_lft forever preferred_lft forever
    2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:65:11:0a brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic enp0s3
           valid_lft 86056sec preferred_lft 86056sec
        inet6 fe80::a00:27ff:fe65:110a/64 scope link
           valid_lft forever preferred_lft forever
    4: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:f7:a4:29 brd ff:ff:ff:ff:ff:ff
        inet 192.168.1.2/24 brd 192.168.1.255 scope global enp0s9
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fef7:a429/64 scope link
           valid_lft forever preferred_lft forever
    5: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
        link/ether 08:00:27:0c:82:71 brd ff:ff:ff:ff:ff:ff
        inet 10.0.0.4/24 brd 10.0.0.255 scope global enp0s8
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe0c:8271/64 scope link
           valid_lft forever preferred_lft forever

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/comment-system.rst

.. _Linux: https://learn-linux.readthedocs.io/zh_CN/latest/
.. _NetPlan: https://learn-linux.readthedocs.io/zh_CN/latest/administration/network/netplan.html
.. _Virtualbox: https://learn-linux.readthedocs.io/zh_CN/latest/environment/virtualbox/index.html
.. _内核参数: https://learn-linux.readthedocs.io/zh_CN/latest/administration/kernel/sysctl.html

.. comments
    comment something out below

    路由表可以分为 **静态** 和 **动态** 两种，静态路由表由管理员配置； **动态** 路由可以
