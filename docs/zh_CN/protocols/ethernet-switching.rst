.. 以太网交换
    FileName:   ethernet-switch.rst
    Author:     Fasion Chan
    Created:    2018-02-06 20:34:05
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        集线器工作在物理层，中继物理信号，所有端口处于同一冲突域并共享带宽。
        交换机工作在数据链路层，根据目的地址转发以太网帧，并能自动学习MAC地址。
        交换机每个端口是独立的冲突域，带宽也是独立的，传输效率比集线器更高。
    :keywords: 交换, switch, 集线器, 交换机, MAC地址自动学习, mac address auto learning

.. _ethernet-switching:

==========
以太网交换
==========

我们知道，通过 :ref:`ethernet-protocol` 通讯的计算机需要通过网线连接到一起。
那么，如何将多根网线连接在一起呢？

.. _ethernet-hub:

集线器
======

最简单的方式是将所有网线接到一个集线器( ``Hub`` )，如下图：

.. figure:: /_images/protocols/ethernet-switching/acb0a0ebc8a69fb9ce5a21ac74ef35ae.png

    *集线器*

集线器内部构造很简单，可以理解成只是把所有网线连接到一起而已。
换句话讲，集线器充当了 **共用导线** 的功能。

.. figure:: /_images/protocols/ethernet-switching/53cf4917a5902a9a82c038b28a6fd332.gif

    *集线器构造*

这样一来，某个端口发送出去的电信号，将传送到所有其他端口：

.. figure:: /_images/protocols/ethernet-switching/49e8d8c6daff998f64eea9541fdc6ac2.jpg

    *信号传输*

也就是说，从一台计算机发送出来的数据，将传送到其他所有计算机上。
以 ``A`` 往 ``B`` 发送数据为例：

.. figure:: /_images/protocols/ethernet-switching/3026a7ee9579986199ce1c6971fb02ae.png

    *数据广播*

看起来就像 ``A`` 发起了 **广播** ，所有其他计算机都可以收到这个数据。
由于数据帧中包含 **目的地址** ，最终只有 ``B`` 接收并处理这个数据。
因此并无大碍，至少是可以正常工作的。

尽管如此，还是存在一些缺陷，主要体现在两方面：

1. 所有计算机(端口)共享带宽；
2. 所有计算机(端口)处于同一 **冲突域** (一台计算机发送，其他只能等待)；

这两方面缺陷注定了集线器传输效率不高，在接入端口数较多的情况下更是如此。

总结一下，集线器工作于物理层，主要特点如下：

- 放大中继物理电信号；
- 所有端口同属一个冲突域；
- 所有端口共享带宽；
- 延伸网络访问距离；
- 扩展终端数量；

.. _layer2-switching:

二层交换
========

为了解决 :ref:`ethernet-hub` 效率低下的尴尬，需要设计一种更高级的网络设备，根据目的 ``MAC`` 地址，将数据准确转发到对应端口。

如下图，中间节点是转发设备，与 ``3`` 台计算机连接。
转发设备维护一张端口与对应计算机 ``MAC`` 地址的映射表。
这样一来， 转发设备接到 ``A`` 机器发给 ``C`` 机器的数据后，根据目的 ``MAC`` 地址查映射表，将数据准确传送到对应的端口 ``3`` 。

.. figure:: /_images/protocols/ethernet-switching/d9b3b635c3812953fae1ad2d0c34ca90.png

    *根据目的地址转发数据*

现在，传输模型现在更有针对性了，数据准确转发到正确的端口，其他端口不再收到多余的数据：

.. figure:: /_images/protocols/ethernet-switching/8a367213e20260730a4c41dea1a18e68.png

    *更有针对性的传输模式*

不仅如此，计算机 ``A`` 与 ``B`` 通讯的同时，其他计算机也可通讯，互不干扰。
每个端口是一个独立的冲突域，带宽也是独立的。

:ref:`ethernet-hub` 的缺陷全解决了！

.. _ethernet-switcher:

以太网交换机
============

能够根据 :ref:`ethernet-frame` 目的地址转发数据的网络设备就是 **以太网交换机** ( ``Ethernet Switch`` )：

.. figure:: /_images/protocols/ethernet-switching/175f7e911cc3a09f0f88487c91ae0143.png

    *交换机*

长相跟 :ref:`ethernet-hub` 没啥区别嘛。
当然了，很多网络设备都是一个满身端口的盒子，关键还得看内部构造。

再看看现实中的交换机长啥样：

.. figure:: /_images/protocols/ethernet-switching/b19011363b3ff0b28385c915b380d71c.jpg

    *一台真实的交换机*

总结起来，以太网交换机是 **二层网络设备** ，特点如下：

- 根据目的地址转发数据
- 每个端口是独立冲突域
- 每个端口带宽独立

.. _mac-address-auto-learning:

MAC地址学习
===========

在 :ref:`layer2-switching` 一节，我们讨论了一种根据映射表转发数据的方法。
现在问题来了，映射表如何获得？

最原始的方式是，维护一张静态映射表。
当新设备接入，向映射表添加一条记录；
当设备移除，从映射表删除对应记录。

然而，纯手工操作方式多少有些烦躁。

好在计算机领域可以实现各种花式的自动化——通过算法自动学习映射表。
先来看看大致思路：

.. figure:: /_images/protocols/ethernet-switching/f3040c8f5ccbf7852c9d7a0945daf057.png

    *映射表为空，先广播；同时学习A的地址*

初始状态，映射表是空的。
此时， ``A`` 向 ``B`` 发送一个数据帧 ``Frame1`` ，映射表查不到 ``B`` 的记录，将数据帧转发到其他所有端口。

交换机通过 ``Fa0/1`` 端口收到数据，便知道 ``A`` 连接 ``Fa0/1`` 端口，而数据帧的源地址就是 ``A`` 的地址！
此时，可以将 ``A`` 的地址和端口 ``Fa0/1`` 作为一条记录加入映射表。
交换机学习到 ``A`` 的地址！

.. figure:: /_images/protocols/ethernet-switching/928e0063dc9a2c642e0125a75f52639c.png

    *根据映射表准确转发；同时学习B的地址*

接着， ``B`` 向 ``A`` 回复一个数据 ``Frame2`` 。
由于映射表已经有关于 ``A`` 的记录了，数据直接转发到端口 ``Fa0/1`` 。
同样，交换机学习到 ``B`` 的地址。

.. figure:: /_images/protocols/ethernet-switching/151c80dff5fa653e7d7b563adfba7c41.png

    *地址学习完毕*

当 ``C`` 开始发送数据时，交换机同样学到其地址，学习过程完成！
这就是 **MAC地址自动学习** 的过程。

.. _ethernet-network:

以太网络
========

以太网络可以由多个设备构建而成：

.. figure:: /_images/protocols/ethernet-switching/7fc0203c170eca22891e36ced4f8e5cf.jpg

    *以太网络*

图中的网络包含了两个连接设备，一个 :ref:`ethernet-hub` ，一个 :ref:`ethernet-switcher` 。
留几个思考题：

- 哪些端口是独立的冲突域？
- 哪些端口带宽是独立的？
- 哪些机器间传输数据互不干扰？

总结
====

本节，介绍了两种常见的网络连接设备：

:ref:`ethernet-hub` 工作在 :ref:`physical-layer` ，只是放大中继物理电信号。
集线器所有端口处于同一冲突域，也共享带宽。
这个两个缺陷导致集线器传输效率不高。

:ref:`ethernet-switcher` 工作在 :ref:`data-link-layer` ，能理解 :ref:`ethernet-frame` ，并根据目的地址进行转发。
交换机每个端口都是独立的冲突域，带宽也是独立的，因此传输比集线器更有效率。

进行 :ref:`ethernet-switching` ，交换机需要维护地址-端口映射表。
交换机可以通过 :ref:`mac-address-auto-learning` 自动完成这个过程。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. include:: /_fragments/wechat-reward.rst

.. comments
    comment something out blow

    http://www.tomshardware.com/answers/id-2972121/diy-ethernet-hub.html
    http://eastmanreference.com/the-difference-between-an-ethernet-splitter-a-switch-and-a-hub/
    http://www.brocade.com/content/html/en/hardware-installation-guide/FI_08030_FCX/GUID-E4748A6D-949C-42B9-AD26-3B5D71970CEE.html
    https://learn.sparkfun.com/tutorials/rotary-switch-potentiometer-hookup-guide
    https://www.ibm.com/developerworks/cn/linux/1310_xiawc_networkdevice/
    http://support.huawei.com/enterprise/docinforeader!loadDocument1.action?contentId=DOC1000062629&partNo=10082
    https://telconotes.wordpress.com/2013/03/09/how-a-switch-works/
