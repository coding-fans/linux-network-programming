.. 数据链路层
    FileName:   data-link-layer.rst
    Author:     Fasion Chan
    Created:    2018-01-28 11:45:04
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
   :description lang=zh: 将两服务器通讯问题扩展到多服务器通讯问题，需要解决寻址以及复用/分用问题，这就是数据链路层的主要作用。
   :keywords: 数据链路层, 寻址, 复用, 分用

.. _data-link-layer:

==========
数据链路层
==========

.. note::
    为了简化接下来的讨论，我们站在物理层的基础上，所有数据发送省略控制比特 ``1010`` 和 ``0101`` 。

前一节讨论了一个理想化模型——两服务器通讯。
现在，我们把问题进一步拓展一下：多台机器如何实现两两通讯？

多服务器通讯问题
================

下面，以三台服务器为例：

.. figure:: /_images/protocols/data-link-layer/2191e6281e268396eaf4d18303ab9fb5.png

    *多服务器通讯模型：共用信道*

图中，有 ``3`` 台服务器，名字分别是： ``ant`` 、 ``bee`` 以及 ``cicada`` 。
为了通讯，我们设想三者均连接至一根共用导线，每台服务器都可以改变导线电平，也可以检测导线电平。
进一步假设，在硬件层面，多机器冲突仲裁机制已经实现并且可用。
这样，是否解决了多服务器通讯问题？

.. _addressing:

寻址
====

假设， ``ant`` 向 ``bee`` 发送(粗体)一个数据 ``11110000`` 。
由于导线是共享的，所有机器都可以检测到电平信号。
换句话讲， ``bee`` 和 ``cicada`` 都会收到这个数据 ``11110000`` ，而 ``cicada`` 本不应该接收这个数据！
另一方面， ``bee`` 收到数据后，也不知道数据到底是谁发送给它的。

.. figure:: /_images/protocols/data-link-layer/129372fbd3cfab17acedd6632303680d.png

    *数据的困惑：我从哪来？要到哪去？*

因此，我们需要引入一些比特，用来标识数据的来源以及目的地。
我们的例子只有3台服务器，两个比特就足以唯一确定一台机器：

.. csv-table:: 表格-1
    :header: "机器", "比特"

    "ant", "00"
    "bee", "01"
    "cicada", "10"

那么，发送数据时，额外加上两个比特用于表示来源机器，另外两个比特表示目标机器，问题不就解决了吗？

.. figure:: /_images/protocols/data-link-layer/c40d1f04fd8f51b47cab3a2387d50bfa.png

``bee`` 收到数据后，检查前两个比特(红色)，值为 ``00`` ，便知道是 ``ant`` 发出来的；
检查紧接着的两个比特(绿色)，值为 ``01`` ，与自己匹配上，便愉快地收下了。
相反， ``cicada`` 收到数据后，发现 ``01`` 和自己 ``10`` 匹配不上，便丢弃这个数据。

新引入比特所起的作用，在计算机网络中称为 **寻址** 。
这两个比特也就称为 **地址** ，其中，红色为源地址，绿色为目的地址。
通过引入寻址，我们完美地解决了数据从哪来，到哪去的问题。

.. _multiplexing-demultiplexing:

复用/分用
=========

信道只有一个，但是通讯需求是无穷无尽的——传输研究数值、文件打印、即时通讯，不一而足。
如何解决这个矛盾呢？套路还是一样的——引入新的比特。

假设，总的通讯需求就上面这3个，那么，2个额外的比特便解决了问题。

.. csv-table:: 表格-2
    :header: "类型", "比特"

    "研究数据", "00"
    "文件打印", "01"
    "即时通讯", "10"

这时，假设 ``ant`` 向 ``bee`` 上报研究数据并打印一个文件：

.. figure:: /_images/protocols/data-link-layer/9c11e1fe6649cd4d5fb31a0869cf6545.png

    *信道复用：使用额外比特区分数据类型*

这样，通过新引入的紫色比特，我们实现了在同个信道上进行不同的通讯！
``bee`` 接收到数据后，根据紫色比特，决定数据如何处理。

接下来，从理论的视角来审视这个场景：

.. figure:: /_images/protocols/data-link-layer/967793342e62b831ef0ff7127a4b6537.png

    *复用信道*

信道只有一个，需要承载不同的通讯需求。
在发送端，通过加入紫色比特，将不同的数据通过一个共用信道发送出去，这个过程叫做 **复用** ( ``Multiplexing`` )；
在接收端，从共用信道上接收数据，然后检查紫色比特决定数据如何处理，这个过程叫做 **分用** ( ``Demultiplexing`` )。
在接下来的章节，我们将看到 ``复用`` - ``分用`` 这个概念贯彻计算机网络的始终。

.. figure:: /_images/protocols/data-link-layer/72936c895af9726f87498ffbe292accd.png
    :width: 200px

    *复用/分用*

到目前为止，我们引入了 ``3`` 种不同的比特，分别是 **源地址** 、 **目的地址** 以及 **数据类型** 。
对于这些比特的位数以及含义的约定，便成为 **网络协议** 。

总结
====

本节，我们解决了多台共用信道服务器间的通信问题，这相当于网络分层结构中的 **数据链路层** 。
数据链路层的主要作用包括：

- :ref:`addressing`
- :ref:`multiplexing-demultiplexing`

进度
====

.. figure:: /_images/protocols/data-link-layer/77c9104cf45312384fe61f198739136b.png

    *新技能Get✔️*

下一步
======

下一节，我们开始学习一个真实的数据链路层协议—— :ref:`ethernet-protocol` 。
届时，我们将看到 :ref:`ethernet-protocol` 与本节虚构的协议别无二致。

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. include:: /_fragments/wechat-reward.rst

.. comments
    comment something out below

    https://en.wikipedia.org/wiki/Medium_access_control

    参考文献
    ========
