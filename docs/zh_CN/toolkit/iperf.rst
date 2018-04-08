.. iperf
    FileName:   iperf.rst
    Author:     Fasion Chan
    Created:    2018-04-08 08:52:23
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        iperf是一个用来测量网络吞吐的工具。
        它既可以测量TCP吞吐，也可以测量UDP吞吐。
        使用iperf测量网络吞吐，用户需要一个接收并忽略流量的服务端以及一个生成测试流量的客户端。
    :keywords: iperf, 网络带宽测量, 网络吞吐测量

=====
iperf
=====

`iperf` 是一个用来 **测量网络吞吐** 的工具。
它既可以测量 `TCP` 吞吐，也可以测量 `UDP` 吞吐。
使用 `iperf` 测量网络吞吐，用户需要一个接收并忽略流量的服务端以及一个生成测试流量的客户端。

下文以 `TCP` 为例，演示使用 `iperf` 测量从 ``BlueServer`` 到 ``RedServer`` 的网络带宽：

.. _iperf-server:

服务端
======

在 ``RedServer`` 服务器上，运行 `iperf` 命令启动服务端：

.. code-block:: shell-session

    fasion@RedServer:~$ iperf -s
    ------------------------------------------------------------
    Server listening on TCP port 5001
    TCP window size: 85.3 KByte (default)
    ------------------------------------------------------------

注意到， `iperf` 服务端默认监听 ``5001`` 端口， `TCP` 窗口大小为 `85.3 KB` 。
用户可根据需要调整，详情请查看 :ref:`iperf-references` 。

.. _iperf-client:

客户端
======

在 ``BlueServer`` 服务器上，运行 `iperf` 命令启动客户端：

.. code-block:: shell-session

    fasionBlueServer:~$ iperf -c RedServer
    ------------------------------------------------------------
    Client connecting to RedServer, TCP port 5001
    TCP window size: 85.0 KByte (default)
    ------------------------------------------------------------
    [  3 ] local 192.168.56.2 port 38517 connected with 192.168.1.3 port 5001

客户端连上服务端之后，便开始生产并发送测试流量了。
除了使用机器名，还可以直接指定 ``RedServer`` 的 `IP` 地址。

.. _iperf-report:

测试报告
========

测试完成后， `iperf` 向屏幕输出测试报告：

.. code-block:: shell-session

    fasionBlueServer:~$ iperf -c RedServer
    ------------------------------------------------------------
    Client connecting to RedServer, TCP port 5001
    TCP window size: 85.0 KByte (default)
    ------------------------------------------------------------
    [  3 ] local 192.168.56.2 port 38517 connected with 192.168.1.3 port 5001
    [ ID ] Interval       Transfer     Bandwidth
    [  3 ]  0.0-11.4 sec  2.38 MBytes  1.75 Mbits/sec

从报告中可以得知，客户端在大约 ``12`` 秒的时间内，发送了 `2.38 MB` 数据，平均带宽是 `1.75 Mb/s` 。
服务端也会输出测试报告，格式是一样的，不再赘述。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. _iperf-references:

参考文献
========

#. `iperf(1) - Linux manual page <https://linux.die.net/man/1/iperf>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

    .. meta::
        :description lang=zh:
        :keywords:

