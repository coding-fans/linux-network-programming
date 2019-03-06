.. tcpdump
    FileName:   tcpdump.rst
    Author:     Fasion Chan
    Created:    2018-01-28 20:20:59
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        tcpdump是一个网络抓包工具，用于嗅探经过主机的网络报文，排查问题时非常有用。
        本文介绍tcpdump的基本用法，并提供大量典型示例。
    :keywords: tcpdump, 抓包工具,

=======
tcpdump
=======

`tcpdump`_ 是一个 **网络抓包** 工具，用于嗅探经过主机的网络报文，排查问题时非常有用。
本文介绍 `tcpdump` 的 **基本用法** ，并提供大量 **典型示例** 。

基本用法
========

抓取经过 `enp0s3` :ref:`nic` *80* 端口的报文：

.. code-block:: shell-session

    $ tcpdump -ni enp0s3 port 80

其中， *-n* 表示不将地址转化成名字，这个选项在网络工具中很常见， :doc:`ip` 、 `ping` 、 `ss` 等命令也有。
*-i* 选项指定所嗅探的 **网络设备** ；
*port 80* 是一个 **表达式** ( `expression` )，用于过滤报文，表示只输出端口为 *80* 的报文。

.. note::

    网络工具命令一般通过 `DNS` 反解将 `IP` 地址转化成域名输出展示。
    `DNS` 查询有一定耗时，特别是记录查询失败时，因此命令输出可能会有明显延迟。

    例如，运行 `ping` 命令时， `ICMP` 回显答复报文已经收到，而命令却还没输出。
    这种卡顿现象只需指定 *-n* 选项即可解决。

典型示例
========

嗅探环回流量， *-i* 指定环回设备 `lo` ：

.. code-block:: shell-session

    $ tcpdump -ni lo

嗅探所有网络流量， *-i* 指定任意网络设备，为特殊关键字 *any* ：

.. code-block:: shell-session

    $ tcpdump -ni any

通过表达式过滤 `ICMP` 协议流量：

.. code-block:: shell-session

    $ tcpdump -ni any icmp

通过表达式过滤给定主机( `IP` 地址)的流量：

.. code-block:: shell-session

    $ tcpdump -ni any host 10.0.1.2

过滤给定主机 *80* 端口的流量：

.. code-block:: shell-session

    $ tcpdump -ni any host 10.0.1.2 and port 80

匹配 `TCP` 分组标志位，以 `SYN` 为例：

.. code-block:: shell-session

    $ tcpdump -ni any port 443 and "tcp[tcpflags] & tcp-syn != 0"

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `tcpdump(1) - Linux manual page <http://man7.org/linux/man-pages/man1/tcpdump.1.html>`_

.. _tcpdump: http://man7.org/linux/man-pages/man1/tcpdump.1.html

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/comment-system.rst

.. comments
    comment something out blow

