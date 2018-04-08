.. 以太网编程实践(Python语言)
    FileName:   ethernet-programming-c.rst
    Author:     Fasion Chan
    Created:    2018-01-28 11:45:04
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
   :description lang=zh: 以太网编程实践以Python语言为例，讲解如何发送以太网数据帧，内容包括：命令行参数处理，MAC地址转换，查询网卡地址及序号，套接字发送数据帧等。
   :keywords: 以太网编程, Linux, Python, 发送以太网数据帧, argparse, MAC地址, 套接字

==========================
以太网编程实践(Python语言)
==========================

.. note::
    `Python <https://www.python.org/>`_ 目前有两个主流版本，我们以 `Python 3 <https://docs.python.org/3/>`_ 为准。

本节的目标是，实现一个 ``Python`` 程序 ``send_ether.py`` ，用于通过网卡发送以太网数据帧。
我们将从最基础的知识开始，一步步朝着目标努力。

``send_ether`` 在前面章节已经用过，并不陌生，基本用法如：表格-1。

.. csv-table:: 表格-1 命令行选项
    :header: "选项", "含义"

    "-i --iface", "发送网卡名"
    "-t --to", "目的MAC地址"
    "-T --type", "类型"
    "-d --data", "待发送数据"

下面是一个命令行执行实例：

.. code-block:: shell-session

    $ python send_ether.py -i enp0s8 -t 0a:00:27:00:00:00 -T 0x1024 -d "Hello, world!"

.. _parse-command-line-arguments-py:

处理命令行参数
==============

我们要解决的第一问题是，如何获取命令行选项。
在 ``Python`` 中，命令行参数可以通过 `sys <https://docs.python.org/3/library/sys.html>`_ 模块 `argv <https://docs.python.org/3/library/sys.html#sys.argv>`_ 属性获取：

.. code-block:: python

    import sys
    sys.argv

以上述命令为例， ``argv`` 列表等价于：

.. code-block:: python

    [
        "send_ether.py",
        "-i",
        "enp0s8",
        "-t",
        "0a:00:27:00:00:00",
        "-T",
        "0x1024",
        "-d",
        "Hello, world!",
    ]

你可以遍历这个列表，各种判断 ``-i`` 、 ``-t`` ，获取想要的参数值。

不过，更优雅的方式是使用标准库 `argparse <https://docs.python.org/3/library/argparse.html>`_ 。
以上述命令为例， ``argparse`` 用法如下：

.. literalinclude:: /_src/python/ethernet/send_ether.py
    :caption: parse_arguments()
    :name: py/parse_arguments
    :language: python
    :lines: 116-164
    :linenos:

如上，我们定义了一个函数 ``parse_arguments`` ，来解析命令行参数。
``parse_arguments`` 依赖标准库 ``argparse`` ，处理步骤如下：

#. 实例化一个解析器对象( ``3`` 行)；
#. 登记命令行选项配置( ``5`` - ``34`` 行)；
#. 调用 ``parse_args`` 方法解析命令行参数( ``37`` 行)；
#. 返回结果；

.. _ethernet-frame-py:

以太网帧
========

接下来，重温 :ref:`ethernet-frame` ，看到下图应该不难回忆：

.. figure:: /_images/protocols/data-link-layer/97c13f044de260baf0ed8051091dd251.png

    *以太网帧：目的地址、源地址、类型、数据、校验和*

从数学的角度，重新审视以太网帧结构：
每个字段有固定或不固定的 **长度** ，单位为字节。
字段开头与帧开头之间的距离称为 **偏移量** ，第一个字段偏移量是 ``0`` ，
后一个字段偏移量是前一个字段偏移量加长度，依次类推。

各字段 **长度** 以及 **偏移量** 列举如下：

.. csv-table:: 表格-3 以太网帧字段长度及偏移量
    :header: "字段", "长度(字节)", "偏移量(字节)"

    "目的地址", "6", "0"
    "源地址", "6", "6"
    "类型", "2", "12"
    "数据", "46-1500", "14"

以 `struct`_ 语言，数据帧头部可以表示为： ``!6s6sH`` 。
其中， ``!`` 表示网络字节序，即大端； ``6s`` 表示一个 ``6`` 字节长的字段；
``H`` 表示一个 ``2`` 字节整数，即 ``short`` 。

.. _convert-mac-address-py:

转换MAC地址
===========

二进制
------

``MAC`` 地址为 ``6`` 字节长，在 ``Python`` 中，用 ``bytes`` 类型表示：

.. code-block:: python

    >>> n = b'\x08\x00\x27\xc8\x04\x83'
    >>> n
    b"\x08\x00'\xc8\x04\x83"

注意到，第三个字节为 ``0x27`` ，在 `ASCII`_ 表中表示 ``'`` ，是可读的。

冒分十六进制
------------

`ASCII`_ 表只定义了字节中的 ``7`` 位，低于 ``127`` 部分，
而且其中有相当部分是不可读的(或者说不可显示的)。
因此，不能直接使用 `ASCII`_ 显示 ``MAC`` 地址。

尽管如此，还是可以用多个可读字符表示一个原始字节，思想与 `Base64`_ 类似。
使用 ``2`` 个十六进制字符就可表示一个原始字节，为了可读性，字节间用冒号 ``:`` 分隔。
这就是 **冒分十六进制** ：

.. code-block:: python

    >>> s = '08:00:27:c8:04:83'
    >>> s
    '08:00:27:c8:04:83'

mac_ntoa
--------

将二进制转换成可读形式比较简单，定义一个格式化传即可：

.. literalinclude:: /_src/python/ethernet/show_mac.py
    :caption: mac_ntoa(n)
    :name: py/mac_ntoa
    :language: python
    :lines: 30-41
    :linenos:

mac_aton
--------

将冒分十六进制形式中的冒号去除之后，得到的是 ``6`` 个原始字节的 `Base16`_ 形式：

.. code-block:: pycon

    >>> '61:62:63:64:65:00'.replace(':', '')
    '616263646500'

接下来使用 `Base16`_ 解码便可得到二进制形式的 ``MAC`` 地址：

.. literalinclude:: /_src/python/ethernet/send_ether.py
    :caption: mac_aton(a)
    :name: py/mac_aton
    :language: python
    :lines: 35-46
    :linenos:

.. _fetch-iface-mac-py:

获取网卡地址
============

发送以太网帧，我们需要 **目的地址** 、 **源地址** 、 **类型** 以及 **数据** 。
**目的地址** 以及 **数据** 分别由命令行参数 ``-t`` 以及 ``-d`` 指定。
那么， **源地址** 从哪来呢？

别急， ``-i`` 参数不是指定发送网卡名吗？——发送网卡物理地址就是 **源地址** ！
现在的问题是，如何获取网卡物理地址？

``Linux`` 下可以通过 ``ioctl`` 系统调用获取网络设备信息，``request`` 类型是 ``SIOCGIFHWADDR`` 。
下面，写一个程序 ``show_mac.py`` ，演示查询网卡物理地址的方法。
``show_mac.py`` 需要接收一个参数，以指定待查询网卡名：

.. code-block:: shell-session

    $ python show_mac.py enp0s8
    IFace: enp0s8
    MAC: 08:00:27:c8:04:83

``show_mac.py`` 程序源码如下：

.. literalinclude:: /_src/python/ethernet/show_mac.py
    :caption:
    :name: show_mac.py
    :language: python
    :linenos:

程序入口 ``main`` 函数先从 ``sys.argv`` 列表中取出参数：待查网卡名；
然后调用 ``fetch_iface_mac`` 函数获取网卡物理地址；
最后输出结果。

``fetch_iface_mac`` 接收两个参数：①待查网卡名 ``iface`` ；②套接字 ``s`` 。
传递套接字的目的是，复用现有套接字，避免套接字创建及销毁的开销。
如无可用套接字，函数负责创建。

``fetch_iface_mac`` 函数执行步骤如下：

#. 检查套接字，必要时创建( ``56`` - ``57`` 行)；
#. 使用 `struct <https://docs.python.org/3/library/struct.html>`_ 模块构造二进制结构体 ``struct ifreq`` ，填充网卡名( ``60`` 行)；
#. 调动 ``ioctl`` 系统调用获取 ``MAC`` 地址( ``64`` 行)；

.. warning::
    fetch_iface_mac 函数依赖垃圾回收机制销毁套接字。
    因此，存在这样的可能性：套接字在垃圾回收触发前就耗尽了。

    更严谨的做法是，显示调用s.close关闭套接字。
    这样一来，虽然内存回收仍然依赖垃圾回收机制，
    但套接字文件描述符已经及时关闭，降低耗尽风险。

.. _send-ethernet-frame-py:

发送以太网帧
============

``Linux`` 下，发送以太网帧，需要使用原始套接字。
接下来我们看看发送代码：

.. literalinclude:: /_src/python/ethernet/send_ether.py
    :caption: /python/ethernet/send_ether.py
    :name: send_ether.py
    :language: python
    :lines: 74-113
    :linenos:

函数总共接收 ``5`` 个参数：发送网卡名 ``iface`` ；目的地址 ``to`` ； 类型 ``_type`` ；
待发送数据 ``data`` ；以及套接字 ``s`` 。

函数执行步骤如下：

#. 检查目的 ``MAC`` 地址，必要时转换( ``17`` - ``18`` 行)；
#. 检查待发送数据，确保是 ``bytes`` 类型( ``21`` - ``22`` 行)；
#. 检查套接字，必要时创建( ``25`` - ``26`` 行)；
#. 套接字绑定发送网卡( ``29`` 行)；
#. 查询发送网卡 ``MAC`` 地址，也就是数据帧的源地址( ``32`` 行)；
#. 封装数据帧，包括头部和数据两部分( ``35`` - ``37`` 行)；
#. 发送数据帧( ``40`` 行)；

整个程序代码有点长，就不在这里贴了，请在 ``GitHub`` 上查看：
`python/ethernet/send_ether.py <https://github.com/fasionchan/linux-network-programming/blob/master/python/ethernet/send_ether.py>`_ 。

总结
====

本节，我们从 :ref:`parse-command-line-arguments-py` 开始，
重温 :ref:`ethernet-frame-py` ，
学习如何 :ref:`convert-mac-address-py` 以及如何 :ref:`fetch-iface-mac-py` ，
一步步实现终极目标： :ref:`send-ethernet-frame-py` 。

下一步
======

本节以 ``Python`` 语言为例，演示了以太网编程方法。
如果你对其他语言感兴趣，请按需取用：

- :doc:`./ethernet-programming-c`

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `ascii(7) - Linux manual page <http://man7.org/linux/man-pages/man7/ascii.7.html>`_
#. `ioctl_list(2) - Linux manual page <http://man7.org/linux/man-pages/man2/ioctl_list.2.html>`_
#. `packet(7) - Linux manual page <http://man7.org/linux/man-pages/man7/packet.7.html>`_
#. `RFC 4648 <https://tools.ietf.org/html/rfc4648>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _ASCII: https://zh.wikipedia.org/wiki/ASCII
.. _Base16: https://tools.ietf.org/html/rfc4648
.. _Base64: https://zh.wikipedia.org/wiki/Base64
.. _struct: https://docs.python.org/3/library/struct.html
