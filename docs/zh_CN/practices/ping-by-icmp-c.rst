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
        本文使用C语言开发一个ping命令，以演示如何通过套接字发送和接收ICMP协议报文。
    :keywords: icmp, ping, c, checksum, socket, sendto, recvfrom, 编程

=====================================
ICMP协议编程实践：实现ping命令(C语言)
=====================================

众所周知， `ping`_ 命令通过 `ICMP` 协议探测目标 `IP` 并计算 **往返时间** 。
本文使用 `C` 语言开发一个 `ping` 命令，
以演示如何通过 **套接字** **发送** 和 **接收** `ICMP` 协议报文。

.. note::

    **程序源码** 可在本文末尾复制，或者在 `Github`_ 上下载： `ping.c`_ 。

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

定义一个结构体用以封装报文：

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c/struct
    :language: c
    :lines: 26-38
    :linenos:

前 *3* 个字段为 `ICMP` 公共头部；
中间 *2* 个字段为 **回显请求** 、 **回显答复** 惯例头部；
其余字段为 **数据负载** ，包括一个双精度 **发送时间戳** 以及一个固定的魔性字符串。

校验和
======

`ICMP` 报文校验和字段需要自行计算，计算步骤如下：

#. 以 *0* 为校验和封装一个用于计算的 **伪报文** ；
#. 将报文分成两个字节一组，如果总字节数为奇数，则在末尾追加一个零字节；
#. 对所有 **双字节** 进行按位求和；
#. 将高于 *16* 位的进位取出相加，直到没有进位；
#. 将校验和按位取反；

示例代码如下：

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c/calculate_checksum1
    :language: c
    :lines: 47-76
    :linenos:

套接字
======

编程实现网络通讯，离不开 **套接字** ( `socket`_ )，收发 `ICMP` 报文当然也不例外：

.. code-block:: c

    #include <arpa/inet.h>

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

调用 `sendto`_ 系统调用发送 `ICMP` 报文：

.. code-block:: c

    struct icmp_echo icmp;
    struct sockaddr_in peer_addr;

    sendto(s, &icmp, sizeof(icmp), 0, peer_addr, sizeof(peer_addr));

其中，第一个参数为 **套接字** ；
第二、三个参数为封装好的 `ICMP` **报文** 及 **长度** ；
第四、五个参数为 **目的地址** 及地址结构体长度。

调用 `recvfrom`_ 系统调用接收 `ICMP` 报文：

.. code-block:: c

    #define MTU 1500

    char buffer[MTU];
    struct sockaddr_in peer_addr;
    int addr_len = sizeof(peer_addr);

    recvfrom(s, buffer, MTU, 0, &peer_addr, &addr_len);

    struct icmp_echo *icmp = buffer + 20;

参数为接收缓冲区大小，这里用 *1500* 刚好是一个典型的 *MTU* 大小。
注意到， `recvfrom`_ 系统调用返回 `IP` 报文，去掉前 *20* 字节的 `IP` 头部便得到 `ICMP` 报文。

.. note::

    注意，创建 **原始套接字** ( *SOCK_RAW* )需要超级用户权限。

程序实现
========

掌握基本原理后，便可着手编写代码了。

首先，实现 *send_echo_request* 函数，用于发送 `ICMP` **回显请求** 报文：

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c
    :language: c
    :lines: 78-109
    :linenos:

第 *3-17* 行封装用于计算校验和的 **伪报文** ，
注意到 **类型** 字段为 *8* ，
**代码** 字段为 *0* ，
**校验和** 字段为 *0* ，
**标识符** 以及 **序号** 由参数指定；
第 *10* 行调用 *calculate_checksum* 函数计算 **校验和** ；
第 *25-26* 调 `sendto` 系统调用将报文发送出去。

对应地，实现 *recv_echo_reply* 用于接收 `ICMP` **回显答复** 报文：

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c
    :language: c
    :lines: 111-151
    :linenos:

第 *3-5* 行分配用于接收报文的 **缓冲区** ；
第 *9-10* 行调用 `recvfrom`_ 系统调用 **接收** 一个 **新报文** ；
第 *13-15* 接收报文 **超时** ，正常返回；
第 *21* 行从 `IP` 报文中取出 `ICMP` 报文；
第 *24-26* 行检查 `ICMP` **报文类型** ；
第 *29-31* 检查 **标识符** 是否匹配；
第 *32-38* 行计算 **往返时间** 并打印提示信息。

最后，实现 `ping` 行数，循环发送并接收报文：

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c
    :language: c
    :lines: 153-208
    :linenos:

第 *3-12* 行，初始化 **目的地址** 结构体；
第 *14-18* 行，创建用于发送、接收 `ICMP` 报文的 **套接字** ；
第 *20-27* 行，将套接字 **接收超时时间** 设置为 *0.1* 秒，
以便 **等待答复报文** 的同时有机会 **发送请求报文** ；
第 *30-31* 行，获取进程 *PID* 作为 **标识符** 、同时初始化报文 **序号** ；
接着，循环发送并接收报文；
第 *35-46* 行，当前时间达到发送时间则调用 `send_echo_request` 函数 **发送请求报文** ，
更新下次发送时间并自增序号；
第 *48-52* 行，调用 `recv_echo_reply` 函数 **接收答复报文** 。

将以上所有代码片段组装在一起，便得到 `ping.c` 命令。
迫不及待想运行一下：

.. code-block:: shell-session

    $ gcc -o ping ping.c
    $ sudo ./ping 8.8.8.8
    8.8.8.8 seq=1 25.70ms
    8.8.8.8 seq=2 25.28ms
    8.8.8.8 seq=3 25.26ms

*It works!*

程序源码
========

.. literalinclude:: /_src/c/icmp/ping/ping.c
    :name: c/icmp/ping/ping.c
    :language: c
    :linenos:

下一步
======

本节以 ``C`` 语言为例，演示了 `ICMP` 编程方法。
如果你对其他语言感兴趣，请按需取用：

- :doc:`./ping-by-icmp-py`

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `RFC 792 - Internet Control Message Protocol <https://tools.ietf.org/html/rfc792>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Github: https://github.com/fasionchan/linux-network-programming
.. _ping: http://man7.org/linux/man-pages/man8/ping.8.html
.. _ping.c: https://github.com/fasionchan/linux-network-programming/blob/master/src/c/icmp/ping/ping.c
.. _recvfrom: http://man7.org/linux/man-pages/man2/recvfrom.2.html
.. _sendto: http://man7.org/linux/man-pages/man2/sendto.2.html
.. _socket: http://man7.org/linux/man-pages/man2/socket.2.html

.. comments
    comment something out below

