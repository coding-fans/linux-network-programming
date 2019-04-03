.. ICMP协议编程实践：实现ping命令
    FileName:   ping-by-icmp-py.rst
    Author:     Fasion Chan
    Created:    2019-01-17 19:27:07
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        众所周知，ping命令通过ICMP协议探测目标IP并计算往返时间。
        本文使用Python语言开发一个ping命令，以演示如何通过套接字发送和接收ICMP协议报文。
    :keywords: icmp, ping, python, checksum, socket, sendto, recvfrom, 编程

==========================================
ICMP协议编程实践：实现ping命令(Python语言)
==========================================

众所周知， `ping`_ 命令通过 `ICMP` 协议探测目标 `IP` 并计算 **往返时间** 。
本文使用 `Python`_ 语言开发一个 `ping` 命令，
以演示如何通过 **套接字** **发送** 和 **接收** `ICMP` 协议报文。

.. note::

    **程序源码** 可在本文末尾复制，或者在 `Github`_ 上下载： `ping.py`_ 。

报文封装
========

`ICMP` 报文承载在 `IP` 报文之上，头部结构非常简单：

.. figure:: /_images/protocols/icmp/476c9d2e44224eaa078f80bdbad440f9.gif
    :width: 500px

注意到， `ICMP` 头部只有三个固定字段，其余部分因消息类型而异。固定字段如下：

- *type* ， **消息类型** ；
- *code* ， **代码** ；
- *checksum* ， **校验和** ；

`ICMP` 报文有很多不同的类型，由 *type* 和 *code* 字段区分。
而 `ping`_ 命令使用其中两种：

.. figure:: /_images/protocols/ping/c633276d3679c45943a4f2d7c2b55e05.png
    :width: 773px

    *ping命令原理*

如上图，机器 *A* 通过 **回显请求** ( *Echo Request* ) 询问机器 *B* ；
机器 *B* 收到报文后通过 **回显答复** ( *Echo Reply* ) 响应机器 *A* 。
这两种报文的典型结构如下：

.. figure:: /_images/protocols/ping/31beaa9ddfb5278c7cd98dc4c8624a5b.png
    :width: 616px

对应的 *type* 以及 *code* 字段值列举如下：

.. csv-table:: 表-1 回显报文类型
    :header: "名称", "类型", ”代码“

    "回显请求", "8", "0"
    "回显答复", "0", "0"

按照惯例，回显报文除了固定字段，其余部分组织成 *3* 个字段：

- **标识符** ( *identifier* )，一般填写进程 `PID` 以区分其他 `ping` 进程；
- **报文序号** ( *sequence number* )，用于编号报文序列；
- **数据** ( *data* )，可以是任意数据；

按 `ICMP` 规定， **回显答复** 报文原封不动回传这些字段。
因此，可以将 **发送时间** 封装在 **数据负载** ( `payload` )中，
收到答复后将其取出，用于计算 **往返时间** ( *round trip time* )。

`Python` 标准库 `struct`_ 模块提供了用于 **封装网络报文** 的工具，可以这样封装数据负载：

.. code-block:: python

    import struct

    sending_ts = time.time()
    payload = struct.pack('!d', sending_ts)

这段代码将当前时间戳封装起来，其中 *!* 表示 **网络** :doc:`endianness` ， *d* 表示双精度浮点。

封装报文头部也是类似的：

.. code-block:: python

    header = struct.pack('!BBHHH', _type, code, checksum, ident, seq)
    icmp = header + payload

其中， *B* 表示长度为一个字节的无符号整数， *H* 表示长度为两个字节的无符号整数。

校验和
======

`ICMP` 报文校验和字段需要自行计算，计算步骤如下：

#. 以 *0* 为校验和封装一个用于计算的 **伪报文** ；
#. 将报文分成两个字节一组，如果总字节数为奇数，则在末尾追加一个零字节；
#. 对所有 **双字节** 进行按位求和；
#. 将高于 *16* 位的进位取出相加，直到没有进位；
#. 将校验和按位取反；

示例代码如下：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/calculate_checksum1
    :language: python
    :lines: 23-41
    :linenos:

套接字
======

编程实现网络通讯，离不开 **套接字** ( `socket`_ )，收发 `ICMP` 报文当然也不例外：

.. code-block:: python

    from socket import socket, AF_INET, SOCK_RAW, IPPROTO_ICMP

    s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)

调用 `sendto`_ 系统调用发送 `ICMP` 报文：

.. code-block:: python

    s.sendto(icmp, 0, ('xxx.xxx.xxx.xxx', 0))

其中，第一个参数为封装好的 `ICMP` 报文；
第二个参数为发送标志位，无特殊要求一般填 *0* ；
第三个参数为目的 `IP` 地址-端口对，端口这里填 *0* 。

调用 `recvfrom`_ 方法接收 `ICMP` 报文：

.. code-block:: python

    ip, (src_ip, _) = s.recvfrom(1500)
    icmp = ip[20:]

参数为接收缓冲区大小，这里用 *1500* 刚好是一个典型的 *MTU* 大小。
注意到， `recvfrom`_ 系统调用返回 `IP` 报文，去掉前 *20* 字节的 `IP` 头部便得到 `ICMP` 报文。

.. note::

    注意，创建 **原始套接字** ( *SOCK_RAW* )需要超级用户权限。

程序实现
========

掌握基本原理后，便可着手编写代码了。

首先，实现 *pack_icmp_echo_request* 函数，用于封装 `ICMP` **回显请求** 报文：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/pack_icmp_echo_request
    :language: python
    :lines: 60-70
    :linenos:

第 *2-9* 行封装用于计算校验和的 **伪报文** ，
注意到 **类型** 字段为 *8* ，
**代码** 字段为 *0* ，
**校验和** 字段为 *0* ，
**标识符** 、 **序号** 以及 **数据负载** 字段由参数指定；
第 *10* 行调用 *calculate_checksum* 函数计算 **校验和** ；
第 *11* 行替换伪报文中的校验和并返回。

对应地，实现 *unpack_icmp_echo_reply* 用于解析 `ICMP` **回显答复** 报文：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/unpack_icmp_echo_reply
    :language: python
    :lines: 72-84
    :linenos:

接着，实现 *send_routine* 用于循环发送 `ICMP` **回显请求** 报文：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/send_routine
    :language: python
    :lines: 86-105
    :linenos:

该函数需要 *5* 个参数，分别如下：

- *sock* ，用于发送报文的 **套接字** ；
- *addr* ，目标 `IP`  **地址** ；
- *ident* ， **标识符** ；
- *magic* ，打包在数据负载中的魔性字符串；
- *stop* ，停止发送标识；

第 *3* 行定义 **报文序号** ，从 *1* 开始递增；
接着是发送循环，不停发包，每次相隔一秒；
第 *7* 行获取 **发送时间戳** ；
第 *11* 行将时间戳以及魔性字符串打包成 **数据负载** ；
第 *14* 行调用 *pack_icmp_echo_request* 封装 **回显请求** 报文；
第 *17* 行调用 `sendto`_ 系统调用 **发送报文** ；
第 *19-20* 行自增发送序号并等待一秒。

同样，实现 *recv_routine* 函数用于循环接收 `ICMP` **回显答复** 报文：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/recv_routine
    :language: python
    :lines: 107-127
    :linenos:

第 *4* 行调用 `recvfrom`_ 系统调用接收 `ICMP` 报文；
第 *7* 行调用 *unpack_icmp_echo_reply* **解析报文** ；
第 *8-9* 行忽略非回显答复报文；
第 *13-14* 行检查标识符并忽略非法报文(可能是响应其他进程的)；
第 *16* 行从 **数据负载** 中取出 **发送时间戳** ；
第 *17-21* 行，计算 **往返时间** 并输出提示。

报文 **发送** 和 **接收** 均实现完毕，如何让程序同时干两件事情呢？
可以选用 **线程** 方案：

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py/threading
    :language: python
    :lines: 129-160
    :linenos:

第 *3* 行创建用于发送、接收报文的 **套接字** ；
第 *6* 行获取进程 *PID* 作为 **标识符** ；
第 *16-18* 行启动一个 **子线程** 执行 **报文发送** 函数；
第 *20-24* 行 **主线程** 执行 **报文接收** 函数直至用户按下 *ctrl-C* ；
第 *27* 行程序退出前，通知发送线程退出并回收线程资源( `join` )。

将以上所有代码片段组装在一起，便得到 `ping.py` 命令。
迫不及待想运行一下：

.. code-block:: shell-session

    $ sudo python ping.py 8.8.8.8
    8.8.8.8 seq=1 23.18ms
    8.8.8.8 seq=2 22.25ms
    8.8.8.8 seq=3 34.18ms

*It works!*

程序源码
========

.. literalinclude:: /_src/python/icmp/ping/ping.py
    :name: python/icmp/ping/ping.py
    :language: python
    :linenos:

下一步
======

本节以 ``C`` 语言为例，演示了 `ICMP` 编程方法。
如果你对其他语言感兴趣，请按需取用：

- :doc:`./ping-by-icmp-c`

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `RFC 792 - Internet Control Message Protocol <https://tools.ietf.org/html/rfc792>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Github: https://github.com/fasionchan/linux-network-programming
.. _ping: http://man7.org/linux/man-pages/man8/ping.8.html
.. _ping.py: https://github.com/fasionchan/linux-network-programming/blob/master/src/python/icmp/ping/ping.py
.. _Python: https://python-book.readthedocs.io/zh_CN/latest/
.. _recvfrom: http://man7.org/linux/man-pages/man2/recvfrom.2.html
.. _sendto: http://man7.org/linux/man-pages/man2/sendto.2.html
.. _socket: http://man7.org/linux/man-pages/man2/socket.2.html
.. _struct: https://docs.python.org/3/library/struct.html

.. comments
    comment something out below

