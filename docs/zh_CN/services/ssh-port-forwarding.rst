.. SSH端口转发
    FileName:   port-forwarding.rst
    Author:     Fasion Chan
    Created:    2018-03-28 18:42:03
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        本文将介绍，如何利用 `ssh` 隧道进行端口转发。
        端口转发可分为两种：本地端口转发以及远程端口转发。
        在蓝服运行 ``ssh`` 命令，通过 ``-L`` 选项设置本地端口转发：
        ssh -L 8080:192.168.0.3:80 someuser@RedServer
        在蓝服运行 ``ssh`` 命令，通过 ``-R`` 选项设置远程端口转发：
        ssh -R 2222:localhost:22 someuser@GreenServer
    :keywords: ssh端口转发, 远程端口转发, 本地端口转发, 端口转发, port-forwarding

===========
SSH端口转发
===========

`ssh` 命令除了登录服务器，还可以在两台机器之间建立隧道。

通过隧道，你可以打通一些本来不可访问的网络或者系统，让不可能成为可能。
因此，完全有必要花点时间，学习如何使用 `ssh` 命令打洞。

本文将介绍，如何利用 `ssh` 隧道进行端口转发。端口转发可分为两种：

- :ref:`ssh-local-port-forwarding`
- :ref:`ssh-remote-port-forwarding`

.. note::

    下文图例用颜色区分不同的服务器，称蓝色服务器为蓝服，其他以此类推。

.. _ssh-local-port-forwarding:

本地端口转发
============

.. figure:: /_images/services/port-forwarding/355c9c3d956d2b9aab7156a5caa8f76c.png

如上图，蓝服需要访问绿服 ``80`` 端口，但是直连不通。
蓝服可以通过 ``22`` 端口登录红服，而红服可以连接到绿服。

这种情况，可以在蓝服与红服之间建一个隧道，将蓝服一个本地端口(如 ``8080`` )代理到绿服 ``80`` 端口上。
蓝服访问本地 ``8080`` 端口，就像访问绿服 ``80`` 端口一样。
也就是说，蓝服 ``8080`` 端口被映射(转发)到绿服 ``80`` 端口。

似乎挺不错的，具体如何操作呢？

在蓝服运行 ``ssh`` 命令，通过 ``-L`` 选项设置端口转发：

.. code-block:: shell-session

    fasion@BlueServer:~$ ssh -L 8080:192.168.0.3:80 someuser@RedServer

该命令将从蓝服登陆到红服(建立隧道)，并在本地(即蓝服)监听 ``8080`` 端口。
访问 ``8080`` 端口的请求将通过隧道到达红服，最终由红服负责转发到绿服 ``80`` 端口。
这种端口转发模式称为 **本地端口转发** ( `Local Port Forwarding` )。

选项 ``-L`` 参数有不同的指定方式：

- [bind_address:]bind_port:host:hostport
- [bind_address:]bind_port:remote_socket
- local_socket:host:hostport
- local_socket:remote_socket

``bind_address`` 和 ``bind_port`` 是指蓝服监听的地址端口对； ``host`` 和 ``hostport`` 指定被代理的端口，即红服 ``80`` 端口。

``bind_address`` 可以指定不同的值：

#. ``localhost`` 或者 ``127.0.0.1`` ，只有本地可以访问；
#. 特定 `IP` ，只能通过该 `IP` 访问；
#. ``*`` 或者 ``0.0.0.0`` ，没有任何限制；
#. 不填，默认行为同 3 ；

``host`` 和 ``hostport`` 可以是任何红服可到达的服务，包括红服本身：

.. figure:: /_images/services/port-forwarding/9fe45fe01680a9bc1a724b97694ffcb7.png

在上面这个例子中，红服的 ``80`` 端口被防火墙屏蔽了，而 ``22`` 端口访问正常。

这时也可以通过端口转发解决问题，``host`` 应该填 ``localhost`` 或者 ``127.0.0.1`` ， ``hostport`` 应该填 ``80`` ：

.. code-block:: shell-session

    fasion@BlueServer:~ $ ssh -L 8080:localhost:80 someuser@RedServer

注意到 Unix套接字 也是支持的，这里就不展开了。

.. _ssh-remote-port-forwarding:

远程端口转发
============

.. figure:: /_images/services/port-forwarding/16bcc9128d1c522ad386d584d1bbb63d.png

如上图，由于防火墙的存在，外部无法直接访问蓝服 ``22`` 端口。
蓝服可以登录绿服，而外部也可以正常访问绿服。
这种情况下，也可以通过端口转发打通网络。

在蓝服运行 ``ssh`` 命令，通过 ``-R`` 选项设置端口转发：

.. code-block:: shell-session

    fasion@BlueServer:~$ ssh -R 2222:localhost:22 someuser@GreenServer

该命令将从蓝服登陆绿服，并在绿服监听 ``2222`` 端口。
之后，访问绿服 ``2222`` 端口的请求，将通过隧道到达蓝服，由蓝服负责转发到其本地的 ``22`` 端口。

这种端口转发模式称为 **远程端口转发** ( `Remote Port Forwarding` )，与 **本地端口转发** 类似，但方向刚好相反。

选项 ``-R`` 参数与 ``-L`` 类似，不再赘述：

- [bind_address:]port:host:hostport
- [bind_address:]port:local_socket
- remote_socket:host:hostport
- remote_socket:local_socket
- [bind_address:]port

总结
====

`ssh` 端口转发就是通过 `ssh` 命令在两台机器之间建立隧道，一端负责 **监听** ，另一端负责 **转发** 。
由于方向不同，可分为两种不同的模式：

.. csv-table:: 端口转发模式
    :header: "模式", "监听端", "转发端"

    "本地端口转发", "本地", "远程"
    "远程端口转发", "远程", "本地"

很显然， **端口转发模式** 是由 **监听端** 命名的。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `How to Create SSH Tunnels <https://www.tunnelsup.com/how-to-create-ssh-tunnels/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

    .. meta::
        :description lang=zh:
        :keywords:


    .. raw:: html

        <img src="https://badge.juejin.im/entry/5abc770b518825558c47819b/likes.svg?style=flat-square">
