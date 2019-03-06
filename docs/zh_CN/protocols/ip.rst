.. IP协议
    FileName:   ip.rst
    Author:     Fasion Chan
    Created:    2019-02-17 11:09:15
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        我们引入网络层，以实现全局寻址和数据路由需求，现在开始学习网络层协议——IP协议。
        IP报文头部包括：版本、首部长度、总长度、TTL、协议、检验和源地址、目的地址、可选选项等字段，结构如下。
    :keywords: ip, 协议, ip header, ip头部

======
IP协议
======

我们引入 :doc:`network-layer` ，以实现 **全局寻址** 和 **数据路由** 需求，现在开始学习网络层协议—— `IP` 协议。

IP报文
======

`IP` 报文结构如下图所示：

.. figure:: /_images/protocols/ip/bed8aacc992dc152c1bf79ff7c5a9033.png

报文头部字段包括：

- **版本** ( `version` )，用于标识 **协议版本** ，在协议发展迭代的同时保持高度可兼容性。
  目前部署最成熟的是 `IPv4` ， `IPv6` 则正快速发展中。
- **首部长度** ( `internet header length` 或 `IHL` )，由于 `IP` 头部长度一定为 *4* 的整数倍，
  这个字段以 *4* 字节为单位。典型的 `IP` 报文首部为 *20* 字节， *IHL=5* 。
- **总长度** ( `total length` )，即 `IP` 报文总长度，字段长度为 *16* 位，因此 `IP` 报文最大长度为 :math:`2^{16} = 65535` 。
- **源地址** ( `source address` )，据此接收方可以获悉发送源。
- **目的地址** ( `destination address` )，中间节点据此转发报文。
- **可选选项** ( `options` )，通过 **首部长度** 可以判断报文是否带可选选项。

下一步
======

.. toctree::
    :titlesonly:

    实验：Linux路由 <experiment-linux-routing>

.. comments
    comment something out below

