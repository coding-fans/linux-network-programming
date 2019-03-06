.. socat - Linux/Unix TCP 端口转发
    FileName:   socat-linux-unix-tcp-port-forwarding.rst
    Author:     Fasion Chan
    Created:    2019-02-20 18:59:10
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        socat功能跟NetCat一样，但更安全(支持chroot)，兼容多种协议，支持操作文件(file)、管道(pipe)、设备(device)、TCP套接字、Unix套接字、SOCKS客户端、CONNECT代理以及SSL等等。
    :keywords: socat, port forwarding, tcp, linux, unix, 端口转发

===============================
socat - Linux/Unix TCP 端口转发
===============================

不用 **防火墙** ( `firewall` )软件， `Linux/Unix` 系统能否实现 **端口转发** ( `port-forwarding` )呢？
`socat`_ ( `SOcket CAT` )是一个多用途双向数据转接工具，在 `Linux`_ 系统中如何安装？

`socat`_ 功能跟 :doc:`NetCat </toolkit/nc>` 一样，但更安全(支持 `chroot` )，兼容多种协议，
支持操作 **文件** ( `file` )、 **管道** ( `pipe` )、 **设备** ( `device` )、
`TCP` 套接字、 `Unix` 套接字、 `SOCKS` 客户端、 `CONNECT` 代理以及 `SSL` 等等。

.. note::

    本文译自：`socat: Linux / UNIX TCP Port Forwarder <https://www.cyberciti.biz/faq/linux-unix-tcp-port-forwarding/>`_

典型用途
========

#. `TCP` **端口转发** ( `port forwarding` )；
#. 外网留痕；
#. 攻击弱防火墙(安全测试)；
#. `Unix` 套接字的 `shell` 操作接口；
#. `IPv6` 转接；
#. 安全测试和研究；

安装
====

`Debian/Ubuntu` 下，只需执行 `apt` 命令：

.. code-block:: shell-session

    $ sudo apt update && sudo apt install socat

.. warning::

    以下示例命令可能在你的电脑上 **开端口** 或者通过 **套接字** 连接其他互联网用户。
    使用这个工具必须有 `TCP/IP` 以及 `Unix` 网络方面的知识。

端口转发
========

将 *80* 端口转发到 *202.54.1.5* ：

.. code-block:: shell-session

    # socat TCP-LISTEN:80,fork TCP:202.54.1.5:80

*TCP-LISTEN:80* 表示监听本地 *80* 端口； `fork` 表示创建子进程处理新连接；
*TCP:202.54.1.5:80* 表示将新连接转发到 *202.54.1.5：80* 。

经过以上命令，所有到 *80* 端口的 `TCP` 连接将被转发到 *202.54.1.5* ，与 :doc:`netcat </toolkit/nc>` 一样。

连接SSH服务器
=============

你可以连到远程 `SSH` 服务器( `server1` )，使用 `pty` 作为 `socat`_ 和 `ssh` 的通讯方式，
并将其作为 `ssh` 的控制 `tty` ( `ctty` )，并将 `pty` 作为新进程组所有者( `setsid` )，
这样 `ssh` 可以从 `socat` 获取密码。


.. code-block:: shell-session

    $ (sleep 5; echo YOURSSHPASSWORDHERE; sleep 5; echo date; sleep 1) | \
    socat - EXEC:'ssh -l userName server1.nixcraft.net.in',pty,setsid,ctty

操作Unix套接字
==============

`Unix` 套接字是非常常见的本地通讯方式，但是能够操作这类套接字的工具命令不多，
:doc:`/toolkit/socat` 是其中一个。
下面这个例子，通过 `HAProxy` 的 `Unix` 套接字，获取其运行信息，包含 `PID` 、启动时间等：

.. code-block:: shell-session

    # echo "show info" | socat unix-connect:/var/tmp/haproxy stdio

高级端口转发
============

.. code-block:: shell-session

    # socat -d -d -lmlocal2 \
    TCP4-LISTEN:80,bind=myaddr1,su=nobody,fork,range=10.0.0.0/8,reuseaddr \
    TCP4:www.nixcraft.net.in:80,bind=myaddr2

这个例子在 `accept()` `系统调用`_ 后 `fork` 子进程进行处理，因此可同时处理多个并发连接。
安全性方面也有所提高： `fork` 后 `su` 到 `nobody` 用户，避免权限泄露；
只允许来自 *10.0.0.0/8* 网段的连接。
由于设置了 `reuseaddr` ，就算子进程套接字还没有完全关闭，主进程退出即可重启。
由于指定 *-lmlocal2* 选项， `accept` 循环前日志将输出到 `stderr` 。
后续的日志以 *local2* 为标识转发到 *syslog* 。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `socat(1): Multipurpose relay - Linux man page <https://linux.die.net/man/1/socat>`_
#. `socat 项目 <http://www.dest-unreach.org/socat/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _linux: https://learn-linux.readthedocs.io/zh_CN/latest/
.. _socat: https://linux.die.net/man/1/socat
.. _系统调用: https://learn-linux.readthedocs.io/zh_CN/latest/system-programming/syscall/principle.html

.. comments
    comment something out below

