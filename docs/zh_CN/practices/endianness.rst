.. 字节序
    FileName:   endianness.rst
    Author:     Fasion Chan
    Created:    2019-01-20 10:52:10
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        在大端系统，多字节数据类型起始字节先存储；相反，在小端系统，多字节数据类型末尾字节先存储。
        如果无法人为确定当前系统字节序是大端还是小端，可以编写一个简单程序进行判断。
        htons，将短整数(两字节)从主机字节序转成网络字节序；
        htonl，将长整数(四字节)从主机字节序转成网络字节序；
        ntohs，将短整数(两字节)从网络字节序转成网络字节序；
        ntohl，将长整数(四字节)从网络字节序转成网络字节序。
    :keywords: endianness, big endian, little endian, 字节序, 大端小端, 网络字节序, htons, htonl, ntohs, ntohl

======
字节序
======

**大端** ( *Big endian* ) 和 **小端** ( *Little Endian* )是计算机存储 **多字节** 数据的两种不同方式。

定义
====

在 **大端** 系统，多字节数据类型 **起始字节先存储** ；
相反，在 **小端** 系统，多字节数据类型 **末尾字节先存储** 。

假设有一个 *4* 字节长整数变量 *y* ，值为 *0x01234567*  (十六进制表示法) 。
在 **大端** 系统上，该变量保存为以下 *4* 个字节： *0x01* 、 *0x23* 、 *0x45* 、以及 *0x67* ；
在 **小端** 系统上，存储顺序刚好相反：

.. figure:: /_images/practices/c/endianness/a993cff0c201d6645e72b88e38831dc0.png
    :width: 641px

辅助记忆
========

记忆大端小端定义很容易搞混，通过以下图形可以更好地记住，非常形象：

.. figure:: /_images/practices/c/endianness/e785ff930df62e39597e8b0b4409d50e.png

内存表现形式
============

如果需要查看多字节数据类型在 **内存** 中的表现形式，
可以遍历数据每个字节，以 **十六进制** 将字节打印出来。
下面是一份示例代码：

.. literalinclude:: /_src/c/endianness/show.c
    :caption:
    :name: src/c/show
    :language: c
    :lines: 13-
    :linenos:

字节序判断
==========

如果无法人为确定当前系统字节序是大端还是小端，可以编写一个简单程序进行判断：

.. literalinclude:: /_src/c/endianness/endianness.c
    :caption:
    :name: src/c/endianness
    :language: c
    :lines: 13-
    :linenos:

原理很好理解，定义一个整数，值为 *1* ，然后判断其 **第一个字节** 的值。

网络字节序
==========

不同字节序系统进行网络通信，不加以注意是要乱套的。

为了解决这个问题，约定进行网络通信时统一采用 **大端** 字节序。
因此， **大端** 字节序也称为 **网络字节序** 。

封装网络报文时，需要在 **主机字节序** 和 **网络字节序** 之间转换。
`C` 库提供了转换函数(宏)：

- `htons`_ ，将 **短整数** (两字节)从 **主机字节序** 转成 **网络字节序** ；
- `htonl`_ ，将 **长整数** (四字节)从 **主机字节序** 转成 **网络字节序** ；
- `ntohs`_ ，将 **短整数** (两字节)从 **网络字节序** 转成 **网络字节序** ；
- `ntohl`_ ，将 **长整数** (四字节)从 **网络字节序** 转成 **网络字节序** ；

举个例子，在填充地址结构体端口字段时，需要通过 `htons`_ 进行转换：

.. code-block:: c

    addr.sin_port = htons(80)

转换函数有 *4* 个之多，借助下表可以更好地记忆：

.. csv-table:: 字节序转换助记
    :header: "字母", “含义”

    "h", "主机字节序"
    "n", "网络字节序"
    "s", "短整数(两字节)"
    "l", "长整数(四字节)"

优缺点
======

这两种表示方式各有优缺点：

对于 **小端** 字节序，取不同长度整数的 **汇编指令** 处理方式相同：都是从第 *0* 字节开始。
此外，由于地址 **偏移量** 与 **字节** 的关系一一对应， **多精度** 数学例程很容易实现。

对于 **大端** 字节序，由于高位字节先存储，判断数字 **正负** 只需检查 **第一个字节** (偏移量为 *0* )。
因此，无需跳过一些字节以定位包含 **符号** 的字节，也无需知晓 **变量长度** 。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Little Endian vs Big Endian <https://thebittheories.com/little-endian-vs-big-endian-b4046c63e1f2>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _htons: http://man7.org/linux/man-pages/man3/htons.3.html
.. _htonl: http://man7.org/linux/man-pages/man3/htonl.3.html
.. _ntohs: http://man7.org/linux/man-pages/man3/ntohs.3.html
.. _ntohl: http://man7.org/linux/man-pages/man3/ntohl.3.html

.. comments
    comment something out below

