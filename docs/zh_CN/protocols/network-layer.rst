.. 网络层
    FileName:   network-layer.rst
    Author:     Fasion Chan
    Created:    2018-04-05 10:12:26
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        为解决数据链路层的局限性，需要引入新的一层——网络层以及新的地址——网络层地址。
        网络层地址按网络拓扑分配，实现准确的全球寻址。
        网络层转发设备，即路由器，根据路由表实现数据包精确转发。
    :keywords: 网络层, network layer, 广播风暴, 路由, 路由表

======
网络层
======

 :ref:`data-link-layer` 解决了多台共用信道计算机之间的通讯问题。

 在这一层，每台计算机与 :ref:`ethernet-switcher` 或者 :ref:`ethernet-hub` 之类的设备连接，并用 :ref:`ethernet-protocol` 进行通信：

.. figure:: /_images/protocols/network-layer/a6c0bc01c575cc19d29aca64a681ac72.png

    以太网

数据链路层局限性
================

随着接入的计算机不断增加，交换机一定遇到瓶颈。
试想，如果全世界的计算机都接入，需要造一台多大的交换机(或许就根本无法实现)！

聪明如你，通过多台交换机组网不就解决中心化问题了？
如下图，用一根网线将两台交换机连接起来：

.. figure:: /_images/protocols/network-layer/a13531e70d79522933ce39e3294cdb25.png

    多交换机组网

这样组网确实可以扩展以太网络的规模，但是远远达不到可以组建全球网络的水平。

瓶颈主要是以下几点：

#. 广播风暴；
#. 地址表规模(MAC地址分散性)；

在 :doc:`ethernet-switching` 一节，我们知道 :doc:`data-link-layer` 转发设备(即 **交换机** )的工作原理。
交换机对于不认识的目的 `MAC` 地址，采取广播策略。
这意味着与陌生节点第一次通讯时，数据包需要 **广播到所有节点** 。
这便是 **广播风暴** ，网络规模越大，广播流量越恐怖。

为了保证地址的唯一性， `MAC` 地址按 **设备厂商** 划分并烧在网卡设备内。
某个组织内部使用的设备可能千差万别，因此没有办法对地址表进行合并。
合并要求地址是 **连续** 的，可形成 **地址段** 转发策略。

因此，我们需要在数据链路层的基础上，解决以上两大难题。

.. _ip-routing:

网络层
======

为了解决 :doc:`data-link-layer` 的局限性，需要引入新的一层，以及新的地址。
新的一层为 **网络层** ，新的地址就是 **网络层地址** 。
在网络层转发数据的中间节点，称为 **网络层路由** 。
网络层主要职责在于：实现 **全球寻址** 以及 **数据路由** 。

每台参与网络通讯的计算机分配一个唯一的地址，即 **网络层地址** 。
网络层地址按 **网络拓扑** 分配，保证组织内部的地址是连续的。
例如，给某个公司分配地址 `123.58.173.x` ，只有最后一个数字是不同的。
这样，一条转发配置便可为几百个地址服务。相应地，地址表规模也可下降若干数量级。

参考 :doc:`./data-link-layer` ，定义网络层的数据包结构，包含 **地址** 、 **数据** 以及 **类型** 等信息。
**网络层包** 承载在 **数据链路层包** 之上，换句话讲，数据链路层包 **数据负载** 就是一个 **网络层包** 。

**网络层路由** 存有 **路由表** ，规定了目的地址与与下一跳的对应关系。
**路由表** 看起来与数据链路层 `MAC地址表` 颇为类似，但更加高级：

1. 支持 **地址段** 。一条记录配置某个区间地址的下一跳，有效降低路由表规模。
2. 支持 **高级学习算法** 。例如选择一条跳数最少的路径。

路由接到 **网络层包** 之后，以 **目的地址** 检索路由表决定如何发给下一跳：


.. figure:: /_images/protocols/network-layer/da4c3f29e3c36011339479ddee156a71.png

    数据包路由

中间路由接力转发，直到数据包到达目的地。
如果路由表找不到下一跳，路由不会广播数据包，而是悄悄丢弃，避免广播风暴。

总结
====

本节，我们分析了 :doc:`data-link-layer` 以及 :doc:`ethernet` 的局限性。
**广播风暴** 以及 `MAC` 地址表 **分散性** 严重制约了依赖以太网组建全球网络的可能性。

因此，需要引入新的一层—— **网络层** ，以实现 **全球寻址** 以及 高效的 **数据路由** 。

进度
====

.. figure:: /_images/protocols/network-layer/5f49a11af15701d9d347b83d6556af4d.png

    *新技能Get✔️*

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below


    图中四台机器 :ref:`mac-address` 如下表：

    .. csv-table:: 表格-1 MAC地址
        :header: "机器", "MAC地址"

        "左上", "MAC1"
        "左下", "MAC2"
        "右上", "MAC3"
        "右下", "MAC4"

    交换机端口连接关系如下表：

    .. csv-table:: 表格-2 交换机端口连接
        :header: "交换机", "端口", "连接对象"

        "Switch 1", "1", "左上"
        "Switch 1", "2", "左下"
        "Switch 1", "3", "Switch 2"
        "Switch 2", "1", "右上"
        "Switch 2", "2", "右下"
        "Switch 2", "3", "Switch 1"

    通过 :ref:`mac-address-auto-learning` ，两台交换机最终得到地址表如下：

    .. csv-table:: 表格-2 交换机端口连接
        :header: "交换机", "端口", "地址"

        "Switch 1", "1", "MAC1"
        "Switch 1", "2", "MAC2"
        "Switch 1", "3", "MAC3 MAC4"
        "Switch 2", "1", "MAC3"
        "Switch 2", "2", "MAC4"
        "Switch 2", "3", "MAC1 MAC2"

    造成 **广播风暴** 的根源是 **地址学习** 策略的局限性。
    如果转发配置可以在流量产生前做好，k
