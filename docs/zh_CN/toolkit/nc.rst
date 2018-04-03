.. nc
    FileName:   nc.rst
    Author:     Fasion Chan
    Created:    2018-04-03 20:37:38
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        nc是一个简单但功能很强大的工具，在网络调试中非常有用。
        管理员可以用nc干很多事情，包括：网络测试、文件传输、端口扫描等等。
    :keywords: nc, 端口扫描, 文件传输

==
nc
==

`nc` 是一个简单但功能很强大的工具，在 **网络调试** 中非常有用。

管理员可以用 `nc` 干很多事情，包括：

- :ref:`nc-network-testing`
- :ref:`nc-protocol-interchanging`
- :ref:`nc-data-transfer`
- :ref:`nc-port-scanning`

除此之外，还有很多很多。

.. _nc-network-testing:

网络测试
========

网络配置变更后，需要测试效果。

举个例子，用 `iptables` 配置端口转发后，需要确认转发效果。

直接部署服务进行测试，不一定是一个好选择。
因为测出问题时，可能无法确定到底是 ①端口转发配置 还是 ②服务本身 有问题。
当然了， 部署服务也需要时间不是？

因此，需要 **尽量避免不必要的影响因素** ，提高针对性。

在这个场景，最好是通过一个非常简单的网络服务来测试，有针对地验证端口连通性。
那么， `nc` 就是个不错的工具。

在目标服务器(下文称为 **服务端**)上，运行 `nc` 命令，监听需要验证的端口：

.. code-block:: shell-session

    fasion@192.168.56.11:~$ nc -l 12345

该命令中， ``-l`` 选项指定监听模式， ``12345`` 为监听端口号。


然后，在另一台机器上(下文称为 **客户端**)，连接该端口：

.. code-block:: shell-session

    fasion@192.168.1.22:~$ nc 192.168.56.11 12345

如果成功连上，则说明端口连通性没问题，端口转发配置是 `OK` 的。

这时，客户端可以发送一些数据给服务端，输入要发送的内容并按回车：

.. code-block:: shell-session

    fasion@192.168.1.22:~$ nc 192.168.56.11 12345
    hello world

一点都不意外，服务端收到了信息，并在屏幕上输出：

.. code-block:: shell-session

    fasion@192.168.56.11:~$ nc -l 12345
    hello world

服务端也可以发送一些数据给客户端，操作方式是一样的：

.. code-block:: shell-session

    fasion@192.168.56.11:~$ nc -l 12345
    hello world
    something else

客户端接到数据后，也在屏幕上输出：

.. code-block:: shell-session

    fasion@192.168.1.22:~$ nc 192.168.56.11 12345
    hello world
    something else

或许，古时候，人们就是用 `nc` 命令来聊天的！

.. _nc-protocol-interchanging:

协议交换
========

`nc` 还可以与服务器进行协议交换。

以 `HTTP` 协议为例，连接 `百度 <https://www.baidu.com>`_ 服务器：

.. code-block:: shell-session

    fasion@ubuntu:~$ nc www.baidu.com 80

然后向服务器发送一个 `HTTP` 请求：

.. code-block:: shell-session

    fasion@ubuntu:~$ nc www.baidu.com 80
    GET / HTTP/1.1
    Host: www.baidu.com

输入的第一行是 **请求行** ( `Request Line` )，分成 **请求方法** ( `Request Method` )、 **资源路径** 以及 **协议版本** 共3部分。

第二行是 **请求头部** ( `Request Header` )，例子中只填 ``Host`` 头，指定主机(域名)。

最后是一个空行，这个空行是协议的一部分，不可省略。

请求发出送出去后，百度服务器返回响应(第 ``5`` - ``35`` 行)：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~$ nc www.baidu.com 80
    GET / HTTP/1.1
    Host: www.baidu.com

    HTTP/1.1 200 OK
    Date: Tue, 03 Apr 2018 07:25:57 GMT
    Content-Type: text/html
    Content-Length: 14615
    Last-Modified: Fri, 30 Mar 2018 07:37:00 GMT
    Connection: Keep-Alive
    Vary: Accept-Encoding
    Set-Cookie: BAIDUID=65766304892D3B4B82AD6D49DB7FCF43:FG=1; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
    Set-Cookie: BIDUPSID=65766304892D3B4B82AD6D49DB7FCF43; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
    Set-Cookie: PSTM=1522740357; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
    P3P: CP=" OTI DSP COR IVA OUR IND COM "
    Server: BWS/1.1
    X-UA-Compatible: IE=Edge,chrome=1
    Pragma: no-cache
    Cache-control: no-cache
    Accept-Ranges: bytes

    <!DOCTYPE html><!--STATUS OK-->
    <html>
    <head>
    <meta http-equiv="equivcontent-type" content="text/html;charset=utf-8">
    <meta http-equiv="X-UAA-Compatible" content="IE=Edge">
    <link rel="dns-prefetch" href="//s1.bytesdstatic.com"/>
    <link rel="dns-prefetch" href="//t1.baidu.com"/>
    <link rel="dns-prefetch" href="//t2.baidu.com"/>
    <link rel="dns-prefetch" hrefref="//t3.baidu.com"/>
    <link rel="dns-prefetch" href="//t10.baidu.com"com/>
    <link rel="dns-prefetch" href="//t11.baidu.com"/>
    <link rel="dns-pathrefetch" href="//t12.baidu.com"/>
    <link rel="dns-prefetch" href="//b1.b1bdstatic.com"/>
    <title>百度一下，你就知道</title>


其中第 ``5`` 行是 **状态行** (`Status Line`)，分成 **协议版本** 、 **状态码** (`Status Code`)以及 **描述信息** 三部分，状态码 ``200`` 表示正常。

第 ``6`` - ``20`` 行是 **响应头部** (`Response Header`)，头部有很多， `Cookie` 就是通过 ``Set-Cookie`` 头带下来的。

第 ``22`` - ``35`` 行是 **消息体** (`Message Body`)，这个部分在 `HTTP` 中是可选的，不一定每个响应都有。
例子中的消息体就是百度首页网页内容(有节选)，内容类型是 ``html`` ，由 ``Content-Type`` 头部指定。

`HTTP` 协议更深入内容超出本节范畴，不再赘述。

.. _nc-data-transfer:

数据传输
========

在 :ref:`nc-network-testing` 小节中演示了如何使用 `nc` 命令在两台服务器间发送数据。
结合 **标准输入/输出重定向** ，我们可以用 `nc` 命令在服务器间传文件！

在文件接收服务器上，以监听模式运行 `nc` 命令，并将标准输出重定向到指定位置：

.. code-block:: shell-session

    fasion@192.168.56.11:~$ nc -l 12345 > test.out

在文件发送服务器上，运行 `nc` 命令连接接收端，并将标准输入重定向到要传输的文件：

.. code-block:: shell-session

    fasion@192.168.1.22:~$ nc 192.168.56.11 12345 < test.in

文件传完后， `nc` 命令自动退出。

.. _nc-port-scanning:

端口扫描
========

知道一台服务器打开了哪些端口对外提供服务非常有用，比如：可以确认是否有内部端口意外对外开放。

借助 `nc` 命令，端口扫描非常简单：

.. code-block:: shell-session

    fasion@ubuntu:~$ nc -z 192.168.56.11 10-1000
    Connection to 192.168.56.11 port 22 [tcp/ssh] succeeded!

该命令对 `IP` 地址为 ``192.168.56.11`` 的服务器进行端口扫描，扫描范围是从 ``10`` 到 ``1000`` 。
从命令输出可以获悉， ``22`` 端口是开启并对外服务的，运行的服务 **应该** 是 ``ssh`` 。

注意，这里用的词是—— **应该** 。
虽然端口号有约定俗成的惯例，但并不强制。
确定一个端口的真实服务，需要做一些协议上的交互：

.. code-block:: shell-session

    fasion@ubuntu:~$ echo 'QUIT' | nc 192.168.56.11 22
    SSH-2.0-OpenSSH_5.3p1 Debian-3ubuntu7.1
    Protocol mismatch.

该命令连接到服务器 ``22`` 端口，并向服务器发送一个字符串 ``QUIT`` 。
从服务器发来的信息可以完全确定， ``22`` 端口运行的就是 ``ssh`` 服务！

.. warning::

    如果有内部端口被扫出来，必须检查防火墙配置并修复。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `nc(1) - Linux manual page <https://linux.die.net/man/1/nc>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

