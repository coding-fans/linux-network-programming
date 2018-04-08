.. 以太网编程实践(C语言)
    FileName:   ethernet-programming-c.rst
    Author:     Fasion Chan
    Created:    2018-01-28 11:45:04
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
   :description lang=zh: 以太网编程实践以C语言为例，讲解如何发送一个以太网数据帧，内容包括：命令行参数处理、MAC地址转换，查询网卡地址及序号，套接字发送数据帧等。
   :keywords: 以太网编程, Linux, C, 发送以太网数据帧, argp, MAC地址, 套接字, gcc

=====================
以太网编程实践(C语言)
=====================

本节的目标是，实现一个命令 ``send_ether`` ，用于通过网卡发送以太网数据帧。
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

    $ send_ether -i enp0s8 -t 0a:00:27:00:00:00 -T 0x1024 -d "Hello, world!"

.. _parse-command-line-arguments:

处理命令行参数
==============

我们要解决的第一问题是，如何获取命令行选项。
在 ``C`` 语言中，命令行参数通过 ``main`` 函数参数 ``argc`` 以及 ``argv`` 传递：

.. code-block:: c

    int main(int argc, char *argv[]);

.. fix vim style
    *a*

以上述命令为例，程序 ``main`` 函数获得的参数等价于：

.. code-block:: c

    int argc = 9;

    char *argv[] = {
        "send_ether",
        "-i",
        "enp0s8",
        "-t",
        "0a:00:27:00:00:00",
        "-T",
        "0x1024",
        "-d",
        "Hello, world!",
    };

.. fix vim style
    *a*

这时，你可能要开始对 ``argv`` 进行解析，各种判断 ``-i`` 、 ``-t`` 啦。
当然了，如果是学习或者编程练习，这样做是可以的，编程的诀窍就是勤练习嘛。

但更推荐的方式是，站在巨人的肩膀上——使用 `GNU`_ 提供的 `Argp`_ 。
下面以解析 ``send_ether`` 参数为例，介绍 `Argp`_ 的用法。

首先，定义一个结构体 ``arguments`` 用于存放解析结果，
结构体包含 ``iface`` 、 ``to`` 、 ``type`` 以及 ``data`` 总共 ``4`` 个字段：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption:
    :name: c/send_ether/arguments
    :language: c
    :lines: 291-306
    :linenos:

接着，实现一个选项处理函数 ``opt_handler`` 。
`Argp`_ 每成功解析出一个命令行选项，将调用该函数进行处理：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption:
    :name: c/send_ether/opt_handler
    :language: c
    :lines: 309-339
    :linenos:

其中，参数 ``key`` 是命令行选项配置键，一般为短选项值；
参数 ``arg`` 是选项参数值(如果有)；
参数 ``state`` 是解析上下文，从中可以取到存放解析结果的结构体 ``arguments`` 。
处理函数逻辑非常简单，根据解析到选项，将参数值存放到 ``arguments`` 结构体。

最后，实现一个解析函数 ``parse_arguments`` ，接收参数 ``argc`` 以及 ``argv`` ，返回解析结果： ``arguments`` ：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption:
    :name: c/send_ether/parse_arguments
    :language: c
    :lines: 342-399
    :linenos:

解析函数执行以下步骤：

#. 定义程序文档 ``doc`` 以及位置参数文档 ``args_doc`` ，用于参数解析失败时输出程序用法提示用户；
#. 定义命令行选项配置，总共 ``4`` 个选项，配置的每个字段含义见表格-2；
#. 申明结构体 ``argp`` 用于存放先前定义的各种配置；
#. 申明结构体 ``arguments`` 用于存放解析结构，并填充默认值；
#. 调用库函数 ``argp_parse`` 进行解析，参数请参考 `Argp`_ 文档；

.. csv-table:: 表格-2 选项配置字段含义
    :header: "字段名", "含义"
    :widths: 10 40

    "name", "选项名，一般为长选项"
    "key", "选项键，一般为短选项"
    "arg", ""
    "flags", "选项标志位，OPTION_ARG_OPTIONAL表示可选"
    "doc", "选项文档(用法描述)"
    "group", "选项组，这里省略"

这样，在 ``main`` 函数里，只需要调用 ``parse_arguments`` 便可获得解析结果。
如果，用户给出了错误的选项，程序将输出提示信息并退出。
解决方案很完美！

.. _ethernet-frame-c:

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

在程序编写中，可能会经常用到这些常量。
如果每次都直接使用数值，很考验记忆能力，出错是迟早的事情。

在 ``C`` 语言中，可以用 **宏定义** 将这些常量固化下来。
定义一次，无限使用：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption: 以太网宏定义
    :name: c/frame_macros
    :language: c
    :lines: 25-33
    :linenos:

.. _convert-mac-address:

转换MAC地址
===========

.. _mac_ntoa:

mac_ntoa
--------

函数 ``mac_ntoa`` 将 ``MAC`` 地址由二进制形式转化成可读形式(冒分十六进制)，
形如 ``08:00:27:c8:04:83`` ：

.. literalinclude:: /_src/c/ethernet/show_mac.c
    :caption: void mac_ntoa(unsigned char \*n, char \*a)
    :name: c/mac_ntoa.v1
    :language: c
    :lines: 20-42
    :linenos:

参数 ``n`` 为二进制形式，长度为 ``6`` 字节；参数 ``a`` 为存放可读形式的缓冲区，长度至少为 ``18`` 字节(包含末尾 ``\0`` 字节)。

在 ``mac_ntoa`` 函数体，逐一遍历 ``MAC`` 地址 ``6`` 个字节，调用 ``C`` 库函数 ``sprintf`` 将字节十六进制输出到缓冲区。
注意到，除了首字节，需要额外输出前缀冒号 ``:`` 。

.. _mac_aton:

mac_aton
--------

可读形式转化为二进制形式稍微有点复杂，因为需要做合法性检查。
``08:00:27:c8:04:83`` 是一个合法的 ``MAC`` 地址，
而 ``08:00:27:c8:04:8g`` 就不是( ``g`` 超出十六进制范围)，
``08-00-27-c8-04-83`` 也不是(不是冒号 ``:`` 分隔)。

因此，需要先判断一个字符是不是合法的十六进制字符，可以通过一个宏解决：

.. literalinclude:: /_src/c/ethernet/others/send_ether.v1.c
    :caption: IS_HEX(c)
    :name: c/IS_HEX
    :language: c
    :lines: 36-40
    :linenos:

十六进制字符必须在 ``0`` 和 ``9`` 之间，或者 ``a`` 和 ``f`` 之间，或者 ``A`` 和 ``F`` 之间。
宏 ``IS_HEX`` 就是上述定义的程序语言表达，看似很长很复杂，其实很简单。

那么，两个字节的可读十六进制如何转换成其表示的原始字节呢？
以 ``c8`` 为例，需要转换成字节 ``0xc8`` ，计算方式如下：

.. code-block:: c

    0xc8 == 12 * 16 + 8 == (12 << 4) | 8

那么，从字符 ``c`` 如何得到数值 ``12`` 呢？计算方式如表格-4(有所省略)：

.. csv-table:: 表格-4 十六进制字符数值计算方式
    :header: "字符", "数值", "计算方式"

    "'0'", "0", "'0' - '0'"
    "'1'", "1", "'1' - '0'"
    "'A'", "10", "'A' - 'A' + 10"
    "'B'", "11", "'B' - 'A' + 10"
    "'a'", "10", "'a' - 'a' + 10"
    "'b'", "11", "'b' - 'a' + 10"

现在，可以通过一个宏 ``HEX`` 来完成十六进制字符到数值的转换，定义如下：

.. literalinclude:: /_src/c/ethernet/others/send_ether.v1.c
    :caption: HEX(c)
    :name: c/HEX
    :language: c
    :lines: 42-46
    :linenos:

需要注意，需要先判断是否是小写字符，大写字母次之，数字最后，因为三者在 `ASCII`_ 表就是这个顺序。
有了宏 ``HEX`` 之后，转换不费吹灰之力：

.. code-block:: c

    (HEX(high_byte) << 4) | HEX(low_byte)

注意到，这里使用位运算代替乘法以及加法，因为位运算更高效。

做了这么多准备，终于可以操刀 ``mac_aton`` 函数了：

.. literalinclude:: /_src/c/ethernet/others/send_ether.v1.c
    :caption: int mac_aton(const char \*a, unsigned char \*n)
    :name: c/mac_aton.v1
    :language: c
    :lines: 49-85
    :linenos:

参数 ``a`` 是可读形式，形如 ``08:00:27:c8:04:83`` ，至少 ``18`` 字节(末尾 ``\0`` )；
参数 ``n`` 是用于存储二进制形式的缓冲区，需要 ``6`` 字节。

函数体执行 ``6`` 次循环，每次处理一个字节。
第一个字节之后，需要检查冒号 ``:`` 并跳过。
转换前，先检查高低两个字节是否都是合法十六进制。
转换时，调用刚刚讨论的转换算法，并移动缓冲区。

当然了，用通过 ``C`` 库函数，一行代码就可以完成转换过程：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption: int mac_aton(const char \*a, unsigned char \*n)
    :name: c/mac_aton
    :language: c
    :lines: 36-52
    :linenos:

弄清来龙去脉之后，使用库函数是不错的：①开发效率更高；②代码更健壮。
:ref:`mac_ntoa` 函数也可以用一行代码完成，留作读者练习。

.. _fetch-iface-mac:

获取网卡地址
============

发送以太网帧，我们需要 **目的地址** 、 **源地址** 、 **类型** 以及 **数据** 。
**目的地址** 以及 **数据** 分别由命令行参数 ``-t`` 以及 ``-d`` 指定。
那么， **源地址** 从哪来呢？

别急， ``-i`` 参数不是指定发送网卡名吗？——发送网卡物理地址就是 **源地址** ！
现在的问题是，如何获取网卡物理地址？

``Linux`` 下可以通过 `ioctl`_ 系统调用获取网络设备信息，``request`` 类型是 ``SIOCGIFHWADDR`` 。
下面，写一个程序 ``show_mac`` ，演示查询网卡物理地址的方法。
``show_mac`` 需要接收一个参数，以指定待查询网卡名：

.. code-block:: shell-session

    $ show_mac enp0s8
    IFace: enp0s8
    MAC: 08:00:27:c8:04:83

``show_mac`` 程序源码如下：

.. literalinclude:: /_src/c/ethernet/show_mac.c
    :caption:
    :name: show_mac.c
    :language: c
    :linenos:

程序先定义函数 ``mac_ntoa`` 用于将 ``MAC`` 地址从二进制形式转换成可读形式，浅析网卡地址一节介绍过，不再赘述。

接着是程序入口 ``main`` 函数，主体逻辑如下：

#. 创建一个套接字，类型不限( ``47`` - ``51`` 行)；
#. 将待查询网卡名填充到 ``ifreq`` 结构体( ``54`` - ``55`` 行)；
#. 调用 ``ioctl`` 系统调用查询网卡物理地址( ``SIOCGIFHWADDR`` )，内核将物理地址填充到 ``ifreq`` 结构体( ``58`` - ``62`` 行)；
#. 从 ``ifreq`` 结构体取出 ``MAC`` 地址并转换成可读形式( ``65`` - ``66`` 行)；
#. 输出结果( ``69`` - ``70`` 行)；

.. _compile-c:

编译
----

好了，程序编写完成！那么，怎么让程序代码跑起来呢？
对于 ``C`` 语言，需要先将源代码编译成可执行程序，方可执行。
``Linux`` 下，可以使用 `gcc <http://man7.org/linux/man-pages/man1/gcc.1.html>`_ 来编译代码：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ ls
    _build  c  docs  python  README.md
    fasion@ubuntu:~/lnp$ cd c/ethernet/
    fasion@ubuntu:~/lnp/c/ethernet$ ls
    send_ether.c  show_mac.c
    fasion@ubuntu:~/lnp/c/ethernet$ gcc -o show_mac show_mac.c
    fasion@ubuntu:~/lnp/c/ethernet$ ls
    send_ether.c  show_mac  show_mac.c
    fasion@ubuntu:~/lnp/c/ethernet$ ./show_mac enp0s8
    IFace: enp0s8
    MAC: 08:00:27:c8:04:83

如上，主要步骤包括：

#. 进入源码 ``show_mac.c`` 所在目录 ``c/ethernet/`` ( ``3`` 行)；
#. 运行 `gcc <http://man7.org/linux/man-pages/man1/gcc.1.html>`_ 命令编译程序， ``-o`` 指定生成可执行文件名，( ``6`` 行)；
#. 运行程序 ``show_mac`` ( ``9`` 行)；

.. _code-reuse:

代码复用
--------

更进一步，可以将代码重构成获取网卡地址的通用函数 ``fetch_iface_mac`` ，以便在后续的开发中复用：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption:
    :name: c/fetch_iface_mac
    :language: c
    :lines: 55-104
    :linenos:

``fetch_iface_mac`` 函数总共有 ``3`` 个参数：

- ``iface`` ：指定待查询网卡名；
- ``mac`` ：用于存放 ``MAC`` 地址的缓冲区，至少 ``6`` 字节；
- ``s`` ：套接字，可以复用已有实例，避免创建开销；

.. note::
    如果没有现成套接字可用，可以给 ``s`` 参数传特殊值 ``-1`` 。
    函数将创建临时套接字，用完销毁。
    这个套路在其他函数封装中也会用到，后续不再赘述。

接下来，看看 ``fetch_iface_mac`` 函数体部分，逻辑与 ``show_main`` 程序 ``main`` 函数类似。
注意到，在函数开头，需要视情况创建临时套接字。
在函数结尾处，需要对临时套接字进行回收。
套接字创建后，后续系统调用如果失败，函数需要提前返回，千万别忘了回收临时套接字！
函数 ``fetch_iface_mac`` 中，使用 ``goto`` ( ``34`` 行)将程序逻辑跳转到资源回收处，这个套路在 ``C`` 语言中也算经典。

好了， ``fetch_iface_mac`` 函数开发大功告成！在接下来的开发中，我们将看到 **代码复用** 的强大威力！

.. _send-ethernet-frame:

发送以太网帧
============

``Linux`` 下，发送以太网帧，需要通过原始套接字。
创建一个类型为 ``SOCK_RAW`` 的套接字，与发送网卡进行绑定，便可发送数据了。

先来看看套接字如何与发送网卡绑定：

.. literalinclude:: /_src/c/ethernet/others/send_ether.v1.c
    :caption: int bind_iface(int s, char const \*iface)
    :name: c/bind_iface
    :language: c
    :lines: 186-218
    :linenos:

``bind_iface`` 函数接收两个参数： ``s`` 是待绑定套接字， ``iface`` 是发送网卡名。

通过 ``bind`` 系统调用将套接字与发送网卡绑定，但不能直接用网卡名，需要先获取网卡序号( ``ifindex`` )。
获取网卡序号套路与 :ref:`fetch-iface-mac` 类似，这里不再赘述。

最后，再来看看 ``send_ether`` 函数：

.. literalinclude:: /_src/c/ethernet/others/send_ether.v1.c
    :caption: int send_ether(char const \*iface, unsigned char const \*to, short type, char const \*data, int s)
    :name: c/send_ether.v1
    :language: c
    :lines: 221-303
    :linenos:

函数主要逻辑如下：

#. 创建套接字，类型为 ``SOCK_RAW`` ( ``26`` 行)；
#. 调用 ``bind_iface`` 函数绑定发送网卡( ``33`` 行)；
#. 分配 ``char`` 数组用于填充待发送数据帧( ``47`` 行)；
#. 根据字段偏移量填充数据帧，数据必要时截断( ``48`` - ``64`` 行)；
#. 计算数据帧总长度( ``66`` 行)；
#. 调用 ``sendto`` 系统调用发送数据帧( ``68`` 行)；

整个程序代码有点长，就不在这里贴了，请在 ``GitHub`` 上查看：
`c/ethernet/others/send_ether.v1.c <https://github.com/fasionchan/linux-network-programming/blob/master/c/ethernet/others/send_ether.v1.c>`_ 。

数据帧封装
----------

我们可以进一步优化，将 **以太网帧** 封装成一个结构体：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption: struct ethernet_frame
    :name: c/ethernet_frame
    :language: c
    :lines: 188-203
    :linenos:

这样一来，帧字段与结构体字段一一对应，更加清晰。
而且，填充以太网帧不需要手工指定偏移量，只需填写结构体相关字段即可：

.. literalinclude:: /_src/c/ethernet/send_ether.c
    :caption: fill ethernet frame
    :name: c/fill_ethernet_frame
    :language: c
    :lines: 250-269
    :linenos:

同样，全量代码可以在 ``GitHub`` 上查看：
`c/ethernet/send_ether.c <https://github.com/fasionchan/linux-network-programming/blob/master/c/ethernet/send_ether.c>`_ 。

总结
====

本节，我们从 :ref:`parse-command-line-arguments` 开始，
重温 :ref:`ethernet-frame-c` ，
学习如何 :ref:`convert-mac-address` 以及如何 :ref:`fetch-iface-mac` ，
一步步实现终极目标： :ref:`send-ethernet-frame` 。

此外，在 :ref:`compile-c` 小节，我们第一次编译并执行 ``C`` 语言程序。
在 :ref:`code-reuse` 小节，我们将零散的代码逻辑封装成可复用的通用函数，并涉猎一些 ``C`` 语言经典设计方式。
由于篇幅有限，讲解点到即止，但也足以作为一个不错的起点。

下一步
======

本节以 ``C`` 语言为例，演示了以太网编程方法。
如果你对其他语言感兴趣，请按需取用：

- :doc:`./ethernet-programming-py`

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `packet(7) - Linux manual page <http://man7.org/linux/man-pages/man7/packet.7.html>`_
#. `ascii(7) - Linux manual page <http://man7.org/linux/man-pages/man7/ascii.7.html>`_
#. `netdevice(7) - Linux manual page <http://man7.org/linux/man-pages/man7/netdevice.7.html>`_
#. `ioctl_list(2) - Linux manual page <http://man7.org/linux/man-pages/man2/ioctl_list.2.html>`_
#. `gcc(1) - Linux manual page <http://man7.org/linux/man-pages/man1/gcc.1.html>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Argp: https://www.gnu.org/software/libc/manual/html_node/Argp.html
.. _ASCII: http://man7.org/linux/man-pages/man7/ascii.7.html
.. _GNU: https://www.gnu.org/
.. _ioctl: http://man7.org/linux/man-pages/man2/ioctl.2.html

.. comments
    comment something out below
