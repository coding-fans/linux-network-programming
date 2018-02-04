.. ip
    FileName:   ip.rst
    Author:     Fasion Chan
    Created:    2018-01-28 10:16:41
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

==
ip
==

`ip <http://man7.org/linux/man-pages/man8/ip.8.html>`_ 是一个用来查询或者维护 **路由** (``Routing`` )、
**网络设备** ( ``Device`` )、 **策略路由** ( ``Policy Routing`` )以及 **隧道** ( ``Tunnel`` )的网络工具。

本文提供一些用法实例，详细文档请查看手册： `ip(8) - Linux manual page <http://man7.org/linux/man-pages/man8/ip.8.html>`_ ，或者命令行下运行 :doc:`man` 命令：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ man ip

地址
====

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ ip addr
    1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1
        link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
        inet 127.0.0.1/8 scope host lo
           valid_lft forever preferred_lft forever
        inet6 ::1/128 scope host
           valid_lft forever preferred_lft forever
    2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
        link/ether 08:00:27:4a:14:df brd ff:ff:ff:ff:ff:ff
        inet 10.0.2.15/24 brd 10.0.2.255 scope global enp0s3
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fe4a:14df/64 scope link
           valid_lft forever preferred_lft forever
    3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
        link/ether 08:00:27:c8:04:83 brd ff:ff:ff:ff:ff:ff
        inet 192.168.56.10/24 brd 192.168.56.255 scope global enp0s8
           valid_lft forever preferred_lft forever
        inet6 fe80::a00:27ff:fec8:483/64 scope link
           valid_lft forever preferred_lft forever

.. comments
    comment something out blow

