.. rinetd
    FileName:   rinetd.rst
    Author:     Fasion Chan
    Created:    2018-09-27 19:27:58
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
    :keywords:

======
rinetd
======

进行网络编程、服务部署时，经常需要临时对一些端口进行转发。

举个例子，笔者开发过一个 `Agent` 软件——
它部署于集群每台服务器上，对其上的某个服务进行管理，需要连接服务端口。
我在本地进行开发，需要部署一套相同的服务，这多少有些麻烦。
如果可以在本地部署端口转发服务，监听服务端口，将请求转发至部署有该服务的测试机，便可完美解决问题。

端口转发方案很多，:doc:`/services/ssh-port-forwarding` ， `NAT` 端口转发，甚至 :doc:`/services/haproxy` 之类的工具都可以排上用场。
然而，应对开发调试这种 **临时需求** ， `rinetd`_ 就能胜任，它足够简单轻量级，依赖也少。

.. warning::

    请不要将 `rinetd`_ 应用到生产服务！

    `rinetd`_ 底层采用 `select` 而不是 `epoll` 实现。
    众所周知， `select` 有致命缺陷，一次能处理的 `文件描述符`_ (套接字)非常有限，效率也比较低下。

    因此，**在高并发环境采用rinetd无异于自杀** 。

    更多细节可参考文章：`rinetd 在生产环境要谨慎使用 <https://blog.csdn.net/woshiaotian/article/details/78133195>`_ 。

安装
====

`rinetd`_ 可以运行在任何 `Unix` 系统中，但系统一般没有预装，需要用户自己动手。

安装也简单，只需视系统运行包管理工具。以 `Debian` 为例：

.. code-block:: shell-session

    $ apt install rinetd

笔者在 `OSX` 系统开发，则可以使用 `brew` 来安装：

.. code-block:: shell-session

    $ brew install rinetd

.. note::

    顺便提一下，笔者使用 `brew` 安装 `rinetd`_ 时，出现一些错误——提示不能写入 `/usr/local/sbin` 目录。

    看了一眼该目录，根本就不存在！这种情况可以自己建好 `/usr/local/sbin` 再试，所属用户、所属组以及权限可以参考 `/usr/local/bin` 。

配置
====

运行 `rinetd`_ 前，需要先准备一个配置文件。
配置文件非常简单，主要包含 :ref:`rinetd-forwarding-rules` 、 :ref:`rinetd-access-rules` 以及 :ref:`rinetd-logging-rules` 。

.. _rinetd-forwarding-rules:

转发规则
--------

转发规则定义了监听什么端口，以及转发到哪个目标地址上的哪个端口。形式如下：

.. code-block:: text

    bindaddress bindport connectaddress connectport

其中， `bindaddress` 是 **绑定地址** ； `bindport` 是 **绑定端口** (监听端口)；`connectaddress` 是 **目标地址** ； `connectport` 是 **目标端口** 。

举个例子：

.. code-block:: text

    206.125.69.81 80 10.1.1.2 80

这个配置告诉 `rinetd`_ 监听本地端口 `206.125.69.81:80` ，然后将请求转发到内网机器 `10.1.1.2` 的 `80` 端口。

.. _rinetd-access-rules:

访问控制规则
------------

访问控制规则规定了哪些来源 `IP` 可以连接。

访问控制规则可以分成两种： **放行规则** ( `allow rules` )以及 **禁止规则** ( `allow rules` )。
只要定义了放行规则，任何不匹配的来源 `IP` 将被拒绝连接；
只要定义了禁止规则，任何匹配的来源 `IP` 将被拒绝连接。

此外，访问控制规则还定义了两种不同的 **作用范围** ：

#. 写在 :ref:`rinetd-forwarding-rules` 之前的是 **全局规则** ，对所有转发规则均有效；
#. 写在某条转发规则后的，则只对当前转发规则有效。

回过头来看看访问控制规则怎么写：

.. code-block:: text

    allow pattern
    deny pattern

其中， `pattern` 为来源 `IP` 模式。例如：

.. code-block:: text

    allow 206.125.69.*

这条规则指示对来源 `IP` 在 `206.125.69.0/24` 这个段内的所有连接放行。

.. _rinetd-logging-rules:

日志输出规则
------------

日志输出规则定义日志输出文件以及格式：

.. code-block:: text

    logfile /var/log/rinetd.log
    logcommon

完整示例
--------

.. code-block:: text

    logfile /var/log/rinetd.log
    logcommon

    allow 10.*.*.*

    206.125.69.81 80 10.1.1.2 80
    allow 206.125.69.*

    10.101.1.2 3306 10.1.1.10 3306

部署
====

准备好配置文件后，通过 `-c` 选项启动 `rinetd` 命令即可：

.. code-block:: shell-session

    $ rinetd -c /etc/rinetd.conf

.. note::

    注意到，程序启动后便退出了。
    这是因为， `rinetd`_ 以后台进程(守护进程)的形式运行服务。
    执行 `ps aux | grep rinetd` 命令，可以找到该进程。

.. warning::

    再次强调，请不要将 `rinetd`_ 应用到生产服务！

    `rinetd`_ 底层采用 `select` 而不是 `epoll` 实现。
    众所周知， `select` 有致命缺陷，一次能处理的 `文件描述符`_ (套接字)非常有限，效率也比较低下。

    因此，**在高并发环境采用rinetd无异于自杀** 。

    更多细节可参考文章：`rinetd 在生产环境要谨慎使用 <https://blog.csdn.net/woshiaotian/article/details/78133195>`_ 。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `RINETD(8) <https://www.boutell.com/rinetd/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst


.. _文件描述符: https://learn-linux.readthedocs.io/zh_CN/latest/system-programming/file-io/file-descriptor.html
.. _rinetd: https://www.boutell.com/rinetd/

.. comments
    comment something out below

