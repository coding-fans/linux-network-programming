.. 安装Consul
    FileName:   installation.rst
    Author:     Fasion Chan
    Created:    2019-01-22 19:29:41
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

    .. meta::
        :description lang=zh:
            开始使用Consul前，必须先在机器上安装。
            对于所有支持的系统和硬件平台，Consul均以二进制包的形式进行分发。
            因此，用户可以选择直接使用二进制包，或者自行从源码编译构建。
        :keywords: consul, install, 安装, PATH环境变量, 二进制包

==========
安装Consul
==========

开始使用 `Consul`_ 前，必须先在机器上安装。
对于所有支持的系统和硬件平台， `Consul` 均以 **二进制包** 的形式进行分发。
因此，用户可以选择直接使用二进制包，或者自行从源码 **编译构建** 。

二进制包
========

首先，在 `下载页面 <https://www.consul.io/downloads.html>`_ 选择并下载适用的二进制包，
二进制包为 `ZIP` 压缩包。

下载完毕后，解压二进制包。
`Consul`_ 只包含一个二进制程序，名字为： `consul` 。
包中任何其他文件均可放心删除，完全不影响 `Consul` 功能。

最后一步，确保 `consul` 二进制程序可以被 `PATH` 环境变量找到。
用户可以将 `consul` 移到 `PATH` 目录中一个，或者将 `consul` 所在目录加到 `PATH` 中：

.. code-block:: shell-session

    $ export PATH=/path/to/consul:$PATH

验证安装
========

`Consul`_ 安装完成后，需确保 `consul` 命令就绪，以验证安装效果。
新打开一个终端会话并运行以下命令：


.. code-block:: shell-session

    $ consul -v
    Consul v1.4.0
    Protocol 2 spoken by default, understands 2 to 3 (agent will automatically use protocol >2 when speaking to compatible agents)

如果遇到找不到 `consul` 的错误，说明 `PATH` 环境变量设置有误。
需要重新检查 `PATH` 环境变量，是否包含安装 `Consul` 的目录(即 `consul` 程序所在目录)。

下一步
======

`Consul` 安装完毕后，便可 :doc:`agent` ，一睹为快。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Install Consul | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/install>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

