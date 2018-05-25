.. HAProxy
    FileName:   haproxy.rst
    Author:     Fasion Chan
    Created:    2018-05-07 18:14:29
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :keywords: haproxy, 负载均衡, load balancing

=======
HAProxy
=======

`HAProxy <http://www.haproxy.org/>`_ 是一款开源负载均衡软件，同时提供4层(TCP)和7层(HTTP)代理。

负载均衡
========

由于普通机器单机处理能力以及可靠性的限制，服务需要在多台机器进行部署，形成集群。
这里，主要有两方面的考虑：

#. 提高可靠性；
#. 提高处理能力；

那么，用户(或客户端)如何访问多实例服务呢？

最简单的方案是在客户端进行重试。
以 `DNS` 解析为例，可以配置多台 `DNS` 服务器，一台查询失败则尝试另外一台：

.. figure:: /_images/services/haproxy/bb73a4fcd260578b645659c6dfd968f1.png
    :width: 512px

    *客户端重试*

这显然不是最佳的方案，客户端需要支持连接多台服务器，需要自带服务状态探测功能，
还需要具备切换能力。

一个更科学的架构是，提供一台服务器作为接入代理，为客户端访问实际服务器：

.. figure:: /_images/services/haproxy/6a1276e4ac55c2597729ae0551084ffd.gif
    :width: 580px

    *负载均衡*

在这种模式下，客户端只是直接访问代理服务器，由代理服务器将请求转发到后面的处理服务器。
而且，转发对客户端来说是透明的，跟访问一台服务器没啥区别。

此外，技术人员还可以在代理服务器上实现更强大的架构功能：

#. 代理服务器检查后端服务器状态，及时剔除故障服务器；
#. 代理服务器控制转发流量，处理能力强的服务器可以多分配一些流量；

这种架构就是 **负载均衡** 架构，这里的代理服务器就是 **负载均衡服务器** 。
而 `HAProxy <http://www.haproxy.org/>`_ 就是运行在负载均衡服务器之上，提供负载均衡能力的那个软件服务。

安装部署
========

以 `Ubuntu` 为例，使用 `apt` 进行安装：

.. code-block:: shell-session

    $ apt install haproxy

对于 `Linux` 系统， `Haproxy` 的配置一般存在 ``/etc`` 目录之下。
以 `Ubuntu` 为例，路径是 ``/etc/haproxy/haproxy.cfg`` 。
根据实际场景编辑配置文件，然后重启服务即可生效：

.. code-block:: shell-session

    $ service haproxy restart

.. _layer4-load-balancing:

4层负载均衡
===========

下面是以一个实际例子介绍4层负载均衡的配置方式。
这个故事是笔者为了科学上网，操碎心的故事。

最开始，在美国加州买了一台虚拟机，部署上 `shadowsocks` 服务端，在国内访问。
加州虽然挺遥远，延迟到部分时间在 200 毫秒不到，还是可以接受的。
但是，有时网络质量突然变差，丢包率急剧上升，几乎不可用。
虽然持续时间不会很长，但是在关键时刻确实很烦人。

为了解决这个问题，笔者又在香港买了另一台虚拟机，双保险科学上网。
平时哪台连不上就切到另一台，基本上没有不可用的情况。
顺便提一下，香港的服务器确实快，我从广州连过去延迟基本在 20 毫秒以下。

一台贵的香港服，一台便宜的加州服作为备胎，问题算是解决了。
然而，手工切来切去很不爽呀！

这时，我想起在广州还有一台服务器呢！
我在上面部一个 `HAProxy` 服务，将 `TCP` 连接分发到两台 `shadowsock` 不就好了吗？
于是，下图横空出世：

.. figure:: /_images/services/haproxy/bb0878dce5b5e85224e64c2f6208d69a.png
    :width: 480px

    *科学上网架构*

广州节点 `HAProxy` 的配置如下：

.. code-block:: cfg

    global
        log /dev/log    local0
        log /dev/log    local1 notice
        chroot /var/lib/haproxy
        user haproxy
        group haproxy
        daemon

    defaults
        log global
        mode    tcp
        option  dontlognull
        contimeout 5000
        clitimeout 50000
        srvtimeout 50000

    listen ss-proxy 0.0.0.0:443
        mode tcp
        maxconn 1024
        balance roundrobin
        server hk xxx.xxx.xxx.xxx:443
        server ca xxx.xxx.xxx.xxx:443

这里采用 ``tcp`` 模式，也就是 **4层负载均衡** 模式。
我的配置分为 ``global`` 、 ``defaults`` 以及 ``listen`` 总共 ``3`` 节：

``global`` 节主要是一些全局配置，包括日志、运行用户、组以及运行模式等。

``defaults`` 节主要是一些默认配置，包括代理模式、连接超时时长等。

``listen`` 节是重点所在，定义了服务绑定的地址及端口、代理模式、后端服务器以及负载均衡算法等。
例子中，侦听端口是 ``443`` ，转发到两台后端服务器，分别是香港服( ``hk`` )以及加州服( ``ca`` )。

客户端连接到 `HAProxy` 之后，就不用动不动切换服务器了——爽！

.. figure:: /_images/services/haproxy/fa48a31d52eaa2d04a30e8ddf1dcafe3.png
    :width: 252px

    *无须切换服务器*

下一步
======

如果你想知道如何实现科学上网，可以看看 :ref:`t-shadowsocks` 。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

