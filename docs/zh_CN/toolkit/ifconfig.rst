.. ifconfig
    FileName:   ifconfig.rst
    Author:     Fasion Chan
    Created:    2018-01-28 10:16:41
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

========
ifconfig
========

:doc:`ifconfig` 是一个用于 **配置网卡** 的工具命令。

.. note::

    `ifconfig` 是一个古老的工具，现在显得有些过时了。

    更推荐使用 :doc:`ip` 命令，这是网络配置工具的集大成者。

状态查询
========

`ifconfig` 不带任何参数即可查询当前所有已 **启用** ( `up` )网卡的状态：

.. code-block:: shell-session

	$ ifconfig
	eth1      Link encap:Ethernet  HWaddr 08:00:27:af:9e:56
			  inet addr:192.168.56.11  Bcast:192.168.56.255  Mask:255.255.255.0
			  inet6 addr: fe80::a00:27ff:feaf:9e56/64 Scope:Link
			  UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
			  RX packets:154299 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:147277 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:1000
			  RX bytes:11778201 (11.7 MB)  TX bytes:141390329 (141.3 MB)

	lo        Link encap:Local Loopback
			  inet addr:127.0.0.1  Mask:255.0.0.0
			  inet6 addr: ::1/128 Scope:Host
			  UP LOOPBACK RUNNING  MTU:16436  Metric:1
			  RX packets:96 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:96 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:0
			  RX bytes:8692 (8.6 KB)  TX bytes:8692 (8.6 KB)

以网卡 ``eth1`` 为例，第一行显示这是一块以太网( ``ethernet`` )网卡，硬件地址是 ``08:00:27:af:9e:56`` ；
``inet`` 一行显示网卡对应的 ``IP`` 地址是 ``192.168.56.11`` ，掩码是 ``255.255.255.0`` ；

带上 ``-a`` 参数即查询所有网卡，包括 **禁用** 的( `down` )：

.. code-block:: shell-session

	$ ifconfig -a
	eth0      Link encap:Ethernet  HWaddr 08:00:27:aa:87:65
			  inet addr:10.0.2.15  Bcast:10.0.2.255  Mask:255.255.255.0
			  BROADCAST MULTICAST  MTU:1500  Metric:1
			  RX packets:239850 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:232433 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:1000
			  RX bytes:39353582 (39.3 MB)  TX bytes:160813991 (160.8 MB)

	eth1      Link encap:Ethernet  HWaddr 08:00:27:af:9e:56
			  inet addr:192.168.56.11  Bcast:192.168.56.255  Mask:255.255.255.0
			  inet6 addr: fe80::a00:27ff:feaf:9e56/64 Scope:Link
			  UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
			  RX packets:154411 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:147333 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:1000
			  RX bytes:11788649 (11.7 MB)  TX bytes:141396393 (141.3 MB)

	lo        Link encap:Local Loopback
			  inet addr:127.0.0.1  Mask:255.0.0.0
			  inet6 addr: ::1/128 Scope:Host
			  UP LOOPBACK RUNNING  MTU:16436  Metric:1
			  RX packets:96 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:96 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:0
			  RX bytes:8692 (8.6 KB)  TX bytes:8692 (8.6 KB)

带上网卡名则查询指定网卡：

.. code-block:: shell-session

	$ ifconfig eth0
	eth0      Link encap:Ethernet  HWaddr 08:00:27:aa:87:65
			  inet addr:10.0.2.15  Bcast:10.0.2.255  Mask:255.255.255.0
			  BROADCAST MULTICAST  MTU:1500  Metric:1
			  RX packets:239850 errors:0 dropped:0 overruns:0 frame:0
			  TX packets:232433 errors:0 dropped:0 overruns:0 carrier:0
			  collisions:0 txqueuelen:1000
			  RX bytes:39353582 (39.3 MB)  TX bytes:160813991 (160.8 MB)

启停
====

启用网卡 ``eth0`` ：

.. code-block:: shell-session

    $ ifconfig eth0 up

禁用网卡 ``eth0`` ：

.. code-block:: shell-session

    $ ifconfig eth0 down

特性开关
========

开启混杂( ``promisc`` )模式：

.. code-block:: shell-session

    $ ifconfig eth0 promisc

关闭混杂模式：

.. code-block:: shell-session

    $ ifconfig eth0 -promisc

配置
====

配置度量值( ``metric`` )：

.. code-block:: shell-session

    $ ifconfig eth0 metric 1

配置 `MTU` ：

.. code-block:: shell-session

    $ ifconfig eth0 mtu 1500

配置 `IP` 地址以及掩码(可以分开)：

.. code-block:: shell-session

    $ ifconfig eth0 192.168.5.1 netmask 255.255.255.0

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. _ifconfig-references:

参考文献
========

#. `ifconfig(8) - Linux manual page <https://linux.die.net/man/8/ifconfig>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out blow

