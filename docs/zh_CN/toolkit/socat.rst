.. socat
    FileName:   socat.rst
    Author:     Fasion Chan
    Created:    2019-02-20 18:55:14
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        socat(SOcketCAT)是一个多用途双向数据转接工具。
        socat功能跟NetCat一样，但更安全(支持chroot)，兼容多种协议，
        支持操作文件(file)、管道(pipe)、设备(device)、TCP套接字、Unix套接字、
        SOCKS客户端、CONNECT代理以及SSL等等。
    :keywords: socat, 端口转发, tcp, udp, port forward

=====
socat
=====

`socat`_ ( `SOcket CAT` )是一个多用途双向数据转接工具。

`socat`_ 功能跟 :doc:`NetCat </toolkit/nc>` 一样，但更安全(支持 `chroot` )，兼容多种协议，
支持操作 **文件** ( `file` )、 **管道** ( `pipe` )、 **设备** ( `device` )、
`TCP` 套接字、 `Unix` 套接字、 `SOCKS` 客户端、 `CONNECT` 代理以及 `SSL` 等等。

安装
====

Linux
-----

在 `Linux` 系统，使用包管理器进行安装，以 `Debian/Ubuntu` 为例：

.. code-block:: shell-session

    # apt update && apt install socat

OSX
---

在 `OSX` 系统，使用装包神奇 `brew` 进行安装：

.. code-block:: shell-session

    $ brew update && brew install socat

典型用法
========

端口转发
--------

TCP
+++

监听本地 *80* 端口，并将请求转发(代理)到 *10.0.0.1:8080* 上：

.. code-block:: shell-session

    # socat TCP-LISTEN:80,reuseaddr,fork TCP:10.0.0.1:8080

UDP
+++

监听本地 *53* 端口，并将请求转发(代理)到 *10.0.0.1:5353* 上：

.. code-block:: shell-session

    # socat UDP-LISTEN:53,reuseaddr,fork UDP:10.0.0.1:5353

.. comments
    comment something out below

