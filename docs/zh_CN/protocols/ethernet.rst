.. 以太网协议
    FileName:   ethernet.rst
    Author:     Fasion Chan
    Created:    2018-01-28 10:11:32
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. _ethernet-protocol:

==========
以太网协议
==========

在 :ref:`data-link-layer` 一节，我们虚构了一个网络协议，用于解决多服务器通讯的主要问题。
接下来，介绍一个真实的数据链路层协议—— **以太网协议** 。

使用以太网协议进行通信的实体间，必须通过某种介质直接相连。
通信介质可以是真实的物理设备，如网线、 :ref:`nic` 等；
也可以是通过虚拟化实现的虚拟设备。

以太网协议在这类通信中，主要解决 :ref:`addressing` 以及 :ref:`multiplexing-demultiplexing` 两大问题。

.. _ethernet-frame:

以太网帧
========

**以太网协议** 与 :doc:`./data-link-layer` 中虚构出来的协议几乎一摸一样。

在以太网中，数据通信的基本单位是 **以太网帧** ( ``Frame`` )。协议规定以太网帧的数据格式如下(注意这里的单位为字节而不是比特了)：

.. figure:: /_images/protocols/data-link-layer/97c13f044de260baf0ed8051091dd251.png

    *以太网帧：目的地址、源地址、类型、数据、校验和*

是不是有种似曾相识的感觉！

以太网帧，分为 **头部** ( ``Header`` )、 **数据** ( ``Data`` )以及 **校验和** ( ``Checksum`` )总共 ``3`` 大部分。

其中，头部依次是一个 ``6`` 字节长的 **目的地址** ，用于标记数据由哪台机器接收；
一个 ``6`` 字节长的 **源地址** ，用于标记数据由哪台机器发送；
一个 ``2`` 字节长的 **类型** ，用于标记数据包该如何处理， ``0x0800`` 表示承载的是一个 ``IP`` 包(后续介绍)。

除了长度有所拓展之外，跟我们虚构出来的协议并无二致。
对了，有一点差异——真实的协议中， **目的地址** 放在最前面。
这其中有什么特殊考虑吗？

确实是有的。 接收方收到数据最先处理 **目的地址** ，如果发现数据不是自己的，后面的字段以及数据就无需处理了。
基础网络协议影响方方面面，设计时处理效率也是一个非常重要的考量。

**数据** 可以是任何需要发送的信息，长度可变， ``46`` 至 ``1500`` 个字节。
数据还有另一个更形象的称谓， **负荷** ( ``Payload`` )。
自己脑补数据 **搭载** 交通工具旅行的画面。

哈哈，跟我们假设的套路一模一样！

.. _nic:

网卡
====

现实世界中，计算机通过 **网线** 连接到一起：

.. figure:: /_images/protocols/ethernet/3026a7ee9579986199ce1c6971fb02ae.png

    *以太网网络*

每台通过网线通讯的计算机都需要安装一个硬件设备—— **网卡** ( ``NIC`` )， ``NIC`` 是 ``Network Interface Controller`` 的缩写。

从物理的层面看，网卡负责将比特流转换成电信号发送出去；
反过来，也负责将检测到的电信号转换成比特流并接收。

从软件的层面看，发送数据时，内核协议栈负责封装以太网帧(填充 **目的地址** ， **源地址** ， **类型** 和 **数据** 并计算 **校验和**)，并调用网卡驱动发送；
接收数据时，负责验证 **目的地址** 、 **校验和** 并取出数据部分，交由上层协议栈处理。

.. figure:: /_images/protocols/ethernet/825d32a1e84c6f1ba1c6970fd677e56a.jpg

    *网卡*

每块网卡出厂时，预先分配了一个全球唯一的 :ref:`mac-address` ，并烧进硬件。
不管后来网卡身处何处，接入哪个网络， ``MAC`` 地址均不变。
当然，某些操作系统也允许修改网卡的 ``MAC`` 地址。

.. _promisc-mode:

混杂模式
--------

正常情况下，网卡忽略目的地址与自己不符的数据帧。
如果想要网卡接收所有数据帧，可以开启 **混杂模式** ：

.. code-block:: console

    fasion@ubuntu:~$ sudo ip link set enp0s8 promisc on
    fasion@ubuntu:~$ ip link show enp0s8
    3: enp0s8: <BROADCAST,MULTICAST,PROMISC,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
        link/ether 08:00:27:0e:18:e5 brd ff:ff:ff:ff:ff:ff

设置完成之后，网卡 ``enp0s8`` 带上 ``PROMISC`` 标志，代表混杂模式已经开启。

混杂模式开启后，网卡接收到的数据，不管目的地址是否是自己，都会提交驱动程序处理。
这在使用 ``tcpdump`` 等工具抓包分析网络数据时特别有用。

.. _mac-address:

MAC地址
=======

``MAC`` 地址由 ``6`` 个字节组成( ``48`` 位)，可以支持 ``2`` 的 ``48`` 次方，也就是 ``281474976710656`` 个网络设备(比如网卡)。

.. _mac-address-uniqueness:

全球唯一
--------

.. figure:: /_images/protocols/ethernet/aedd5053f3b00748b3fae089741d3b5d.jpg

    *MAC地址*

如图， ``MAC`` 地址 ``6`` 个字节可以划分成两半部分：

- ``3`` 字节长的 **厂商代码** ( ``OUI`` )，由国际组织分配给不同的网络设备商；
- ``3`` 字节长的 **序列号** ( ``SN`` )，由厂商分配给其生产的网络设备；

以此保证全球唯一性。

.. _colon-hexadecimal-notation:

冒分十六进制
------------

``MAC`` 地址 ``6`` 个字节如何展示呢？
能不能当做 `ASCII`_ 来解读并显示？

恐怕不能。
一个字节总共有 ``8`` 个位，而 `ASCII`_ 只定义了其中的 ``7`` 位。
而且， `ASCII`_ 也定义了很多控制字符，能显示的也只有字母、数字以及一些常用符号。
以上述地址为例，只有 ``0x5B`` 这个字节是可以显示的，对应着 ``[`` 。

好在，我们可以用多个可读字符表示一个原始字节。
我们将一个字节分成两部分，高 ``4`` 位以及低 ``4`` 位，每部分可以用一个十六进制字符来表示。

以 ``0x00`` 这个字节为例，可以用两个字符 ``00`` 表示。
这样一来，整个地址可以用一个 ``12`` 字节长的字符串表示： ``0010A4BA875B`` 。
为了进一步提高可读性，可以在中间插入冒号 ``:`` ： ``00:10:A4:BA:87:5B`` 。

这就是冒分十六进制( ``Colon Hexadecimal Notation`` )。

注意到，冒分十六进制总共需要 ``17`` 个字节。
如果算上结尾处的 ``\0`` ，那么将是 ``18`` 个字节，原来的整整 ``3`` 倍！

实验
====

.. figure:: /_images/protocols/ethernet/7315024ce1bc2ba86f9419e24f1fb27b.png

    *实验网络*

如图，我们用 ``Virtualbox`` 创建 ``3`` 台虚拟机，依次是 ``ant`` 、 ``bee`` 以及 ``cicada`` 。
``3`` 台虚拟机均安装 ``Linux`` 操作系统，这里我们选择 ``Ubuntu 16.04`` 。

.. figure:: /_images/protocols/ethernet/96ea51c90a1bd203d59bc8d247e38204.png

    *Virtualbox虚拟机*

每台虚拟机均配置两块网卡：网卡一用于从宿主 ``SSH`` 登录服务器(虚拟机)；
网卡二则连接到同一个以太网络 ``insect`` ：

.. figure:: /_images/protocols/ethernet/4f8f9e4be6c4202234738affb2b19778.png

    *网卡配置*

.. attention::
    这里混杂模式选择 **全部允许** ，即对应网卡开启 **混杂模式** 。

实验环境搭建完毕，接下来登录服务器做实验！登录 ``ant`` ，查看网卡信息：

.. code-block:: console

    fasion@ant:~$ ip link
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
        link/ether 08:00:27:15:e4:94 brd ff:ff:ff:ff:ff:ff
    3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT group default qlen 1000
        link/ether 08:00:27:99:7e:e8 brd ff:ff:ff:ff:ff:ff

总共有 ``3`` 个网络设备：
``lo`` 是环回设备，从这个设备发送出去的数据帧最终还是送回本机；
``enp0s3`` 是第一块网卡，用于与宿主机通讯；
``enp0s8`` 是第二块网卡，连接到我们的实验以太网络 ``insect`` 。

其他机器的设备结构也是一样的，下面列一下每台机器 ``enp0s8`` 网卡绑定的地址：

.. csv-table:: 表格-1 enp0s8网卡物理地址
    :header: "机器", "物理地址"

    "ant", "08:00:27:99:7e:e8"
    "bee", "08:00:27:58:d6:61"
    "cicada", "08:00:27:0e:18:e5"

接下来，我们通过以太网数据帧从 ``ant`` 往 ``bee`` 发送一个数据。
为了观察数据，我们先在 ``bee`` 以及 ``cicada`` 上运行 ``tcpdump`` 命令，嗅探网络数据包：

.. code-block:: console

    fasion@bee:~$ sudo tcpdump -eni enp0s8
    [sudo] password for fasion:
    tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
    listening on enp0s8, link-type EN10MB (Ethernet), capture size 262144 bytes

接着，使用 ``send_ether`` 命令发送数据帧：

.. code-block:: console

    fasion@ant:~$ sudo send_ether -i enp0s8 -t 08:00:27:58:d6:61 -T 0x1024 -d 'Hello, world!'

``send_ether`` 命令 ``-i`` 选项指定发送网卡名；
``-t`` 选项是目的 ``MAC`` 地址，即 ``bee`` 服务器 ``enp0s8`` 网卡的地址；
``-T`` 选项指定类型，这里我们随便取了一个 ``0x1024`` ；
``-d`` 选项指定数据，这里是最经典的 ``Hello, world!``

在 ``bee`` 上，我们看到了 ``ant`` 发送过来的数据包：

.. code-block:: console

    fasion@bee:~$ sudo tcpdump -eni enp0s8
    [sudo] password for fasion:
    tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
    listening on enp0s8, link-type EN10MB (Ethernet), capture size 262144 bytes
    18:26:12.203490 08:00:27:99:7e:e8 > 08:00:27:58:d6:61, ethertype Unknown (0x1024), length 60:
        0x0000:  4865 6c6c 6f2c 2077 6f72 6c64 2100 0000  Hello,.world!...
        0x0010:  0000 0000 0000 0000 0000 0000 0000 0000  ................
        0x0020:  0000 0000 0000 0000 0000 0000 0000       ..............

注意到，由于数据至少是 ``46`` 字节，系统自动在 ``Hello, world!`` 后面填充 ``\0`` 字节。

另外，服务器 ``cicada`` 也收到这个数据了：

.. code-block:: console

    fasion@cicada:~$ sudo tcpdump -eni enp0s8
    [sudo] password for fasion:
    tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
    listening on enp0s8, link-type EN10MB (Ethernet), capture size 262144 bytes
    18:26:12.498898 08:00:27:99:7e:e8 > 08:00:27:58:d6:61, ethertype Unknown (0x1024), length 60:
        0x0000:  4865 6c6c 6f2c 2077 6f72 6c64 2100 0000  Hello,.world!...
        0x0010:  0000 0000 0000 0000 0000 0000 0000 0000  ................
        0x0020:  0000 0000 0000 0000 0000 0000 0000       ..............

但是， ``cicada`` 检查数据帧目的地址是 ``08:00:27:58:d6:61`` ，
而自己网卡地址是 ``08:00:27:0e:18:e5`` 并不匹配，
便知道这个数据不是自己的。

如果关闭 :ref:`promisc-mode` ， ``cicada`` 服务器网卡将忽略这个数据。

总结
====

本节，我们学习了一个真实的数据链路层协议—— **以太网协议** ，以太网协议的通信单位为 :ref:`ethernet-frame` ( ``Frame`` )。
一个 **以太网帧** 由 **头部** 、 **数据** 以及 **检验和** ``3`` 部分组成，而头部又由 **目的地址** 、 **源地址** 以及 **类型** ``3`` 个字段组成。

计算机通过以太网进行通讯，需要安装 :ref:`nic` ，并用网线连接起来。
每块网卡出厂前，都预先分配一个 :ref:`mac-address-uniqueness` 的 :ref:`mac-address` 并烧进硬件。
``MAC`` 地址由 ``6`` 个字节组成，在以太网协议中用于 :ref:`addressing` 。

一般情况下，网卡忽略目的地址与自己不符的数据帧。
开启 :ref:`promisc-mode` 后，网卡将接收所有数据，不管目的地址是什么。

进度
====

.. figure:: /_images/protocols/ethernet/fb28fd42d6a7e8d61ebfab093e8e906d.png

    *新技能Get✔️*

下一步
======

如果你对如何编写程序发送以太网数据帧感兴趣，那么可以看看 :doc:`/practices/index` 相关章节。
根据喜欢的编程语言，按需取用：

- :doc:`/practices/ethernet-programming-c`
- :doc:`/practices/ethernet-programming-py`

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `RFC 894 <https://tools.ietf.org/html/rfc894>`_

.. include:: /_fragments/wechat-reward.rst

.. _ASCII: http://man7.org/linux/man-pages/man7/ascii.7.html

.. comments
    comment something out blow

    pictures
    http://communitytechnology.github.io/docs/cck/networking/learn-networking-basics/
    http://blog.globalknowledge.com/2012/07/12/does-a-mac-address-mean-apple-invented-it/
    http://www.wikiwand.com/en/Ethernet_frame
    https://www.slideshare.net/MelvinCabatuan1/data-communication-part2
    http://hawk1715.tripod.com/id23.html
    colon hexadecimal notation
