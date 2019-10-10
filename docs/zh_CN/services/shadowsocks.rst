..
    Author: fasion
    Created time: 2018-02-24 20:29:39
    Last Modified by: fasion
    Last Modified time: 2019-10-10 09:42:26

.. meta::
    :description lang=zh:
        为访问Google等网站，需要一些技术手段绕过GFW限制，俗称翻墙。
        shadowsocks是目前比较稳定的翻墙技术方案，本文介绍其原理以及部署方法。
        shadowsocks可以使用Python方式进行安装：sudo pip install shadowsocks 。
        也可以使用apt方式进行安装：sudo apt install shadowsocks-libev 。
    :keywords: shadowsocks, ssserver, sslocal, 翻墙, Google, GFW, gfwlist, gfwlist2pac

===========
Shadowsocks
===========

本文介绍 `shadowsocks`_ 科学上网方案的 :ref:`shadowsocks-principle` 以及 :ref:`shadowsocks-deployment` 。

.. note::

    自己维护 `shadowsocks`_ 服务是一件 **烦心事** ，你需要花精力去处理各种异常。
    当 `IP` 被封时，要么掏钱换个 `IP` ，要么买一个新服务器重新部署。

    人生苦短，请用 `Just My Socks`_ ！

    这是一个 `shadowsocks` 云服务，最低套餐每月 *2.88* 美金( *20* 人民币左右)，
    即可获得稳定的服务，比自己买 `Linode`_ 服务器来部署还便宜呢！
    `Just My Socks`_ 最大的优势是， `IP` 被封时自动换 `IP` ，再也不用自己瞎操心了！

背景
====

从前的从前，网络是自由的，访问 `Google <https://www.google.com/>`_ 畅通无阻：

.. figure:: /_images/services/shadowsocks/1a7062411c7f928fc8a66b826d55fd59.png

    *畅通无阻的时代*

后来，一堵名为 `GFW` 的墙分隔了这边和那边。
这样，所有出国流量都需要经过墙的审核，不适内容将被过滤：

.. figure:: /_images/services/shadowsocks/414a1496fcf6ff2ebeb8adab6b1c8bec.png

    *Evil GFW*

过滤手段很多，包括但不局限于：

- DNS污染
- IP封杀
- 关键字过滤

对付境外反动势力的同时，又要尽量不要影响正常流量， `GFW` 可谓用心良苦。

但很不幸，还是有一小部分正常访问需求受到影响，比如： `Google` 。
作为程序员，很难想象离开 `Google` 该如何编程！

为了工作和学习的需要，我们需要一些可以绕过限制的技术手段。
就目前而言，比较稳定的技术方案是 `shadowsocks <https://shadowsocks.org/en/index.html>`_ 。

.. warning::
    - 请不要使用本文介绍的技术做非法的事情，否则将受到法律的制裁！
    - 请不要私自架设翻墙服务供他人使用！
    - 对于读者任何违法行为，本文不负任何法律责任！

.. _shadowsocks-principle:

技术原理
========

.. figure:: /_images/services/shadowsocks/52b3f5f2d83dbcad3388f4e75a0f74a6.png

    *shadowsocks 技术原理*

如上图， `shadowsocks <https://shadowsocks.org/en/index.html>`_ 的原理很简单，只需要在 `GFW` 两端各自部署一个代理节点。
境外的节点为 ``SS Server`` ，下文称为 :ref:`ss-server` ；
境内的节点为 ``SS Local`` ，下文称为 :ref:`ss-local` ；
上网的 ``PC`` 或者移动设备等，则统一称为 :ref:`ss-client` ；
``SS Local`` 可以是一台独立的服务器，也可以是 ``PC`` 或移动设备上一个应用进程。

``SS Server`` 运行一个后台服务， ``SS Local`` 也运行一个后台服务。
两个服务通过 ``TCP`` 相连并交换数据，数据采用加密算法加密。
这相当于，在一堵墙上打了一个洞——加密隧道。这样一来：

- 真正的请求在 ``SS Server`` 发起，不受到 `GFW` 域名污染；
- ``SS Server`` 的 ``IP`` 不在 `GFW` 黑名单内，便绕过了 ``IP`` 封杀；
- 经过加密的报文，`GFW` 读不懂，便不会被过滤了；

再来看看，经过 `shadowsocks` 转发的一次请求如何进行。完整步骤如下：

#. :ref:`ss-client` 发起的请求，被代理到 :ref:`ss-local` ；
#. 请求通过加密隧道到达 :ref:`ss-server` ；
#. :ref:`ss-server` 向目标服务器(如 Google)发起请求；
#. 目标服务器返回响应；
#. :ref:`ss-server` 通过加密隧道回传响应内容；
#. :ref:`ss-local` 向 :ref:`ss-client` 返回响应；

虽然饶了一大圈，但总算达到目的了。

.. _shadowsocks-deployment:

部署方式
========

`shadowsocks` 有多种部署方式：:ref:`shadowsocks-python` 以及 :ref:`shadowsocks-apt` 等，择一即可。

.. note::

    自己维护 `shadowsocks`_ 服务是一件 **烦心事** ，你需要花精力去处理各种异常。
    当 `IP` 被封时，要么掏钱换个 `IP` ，要么买一个新服务器重新部署。

    人生苦短，请用 `Just My Socks`_ ！

    这是一个 `shadowsocks` 云服务，最低套餐每月 *2.88* 美金( *20* 人民币左右)，
    即可获得稳定的服务，比自己买 `Linode`_ 服务器来部署还便宜呢！
    `Just My Socks`_ 最大的优势是， `IP` 被封时自动换 `IP` ，再也不用自己瞎操心了！

.. _shadowsocks-python:

Python
------

:ref:`ss-server-python` 部署方式适应性较广。
不管什么 `Linux` 发行版，只要有可用的 `Python` 执行环境，即可安装部署。
不足之处是，服务配置参数以及启停方式需要自己维护。

.. _shadowsocks-apt:

Apt
---

`Debian` 下更推荐使用 :ref:`ss-server-apt` 的方式部署，一行命令搞定安装。
安装后，服务配置维护在 ``/etc`` 下，内置 ``init.d`` 启停，不需要自己维护。
修改配置后即可启动服务，简便清晰。
局限性则是发行版支持有限，适应性较差。

.. _ss-server:

服务端
======

部署服务端，首先需要有一台境外服务器。
国外有很多 `VPS` 服务商， `Linode`_ 口碑就不错。
有需要的童鞋可以考虑在 `Linode`_ 上购买一个，最低配置一个月 5 美元，也不贵。

注册时，麻烦填上笔者的推荐码 ``8e45c4effcbbafe17fd593b5064d219bfab37e7d`` 。

`VPS` 就绪后，便可以着手配置服务端。
前面提到，部署方式有多种，根据自己的场景选择一个：

.. _ss-server-python:

Python
------

首先，使用 ``pip`` 安装 ``shadowsocks`` 包：

.. code-block:: shell-session

    $ sudo pip install shadowsocks

安装完毕后， ``ssserver`` 以及 ``sslocal`` 等命令就绪。

启动服务端，需要运行 ``ssserver`` 命令。例子如下：

.. code-block:: shell-session

    $ ssserver -p $SERVER_PORT -k $PASSWORD -m $METHOD

其中， ``-p`` 选项指定 **服务端口** ， ``-k`` 选项指定 **加密密钥** ， ``-m`` 参数指定 **加密算法** 。
``ssserver`` 运行后，服务端开始对外服务。

``ssserver`` 命令详细用法请通过 ``--help`` 参数查看：

.. code-block:: shell-session

    $ ssserver --help
    usage: ssserver [OPTION]...
    A fast tunnel proxy that helps you bypass firewalls.

    You can supply configurations via either config file or command line arguments.

    Proxy options:
      -c CONFIG              path to config file
      -s SERVER_ADDR         server address, default: 0.0.0.0
      -p SERVER_PORT         server port, default: 8388
      -k PASSWORD            password
      -m METHOD              encryption method, default: aes-256-cfb
      -t TIMEOUT             timeout in seconds, default: 300
      --fast-open            use TCP_FASTOPEN, requires Linux 3.7+
      --workers WORKERS      number of workers, available on Unix/Linux
      --forbidden-ip IPLIST  comma seperated IP list forbidden to connect
      --manager-address ADDR optional server manager UDP address, see wiki

    General options:
      -h, --help             show this help message and exit
      -d start/stop/restart  daemon mode
      --pid-file PID_FILE    pid file for daemon mode
      --log-file LOG_FILE    log file for daemon mode
      --user USER            username to run as
      -v, -vv                verbose mode
      -q, -qq                quiet mode, only show warnings/errors
      --version              show version information

    Online help: <https://github.com/shadowsocks/shadowsocks>

.. _ss-server-apt:

apt
---

`Debian` 系列发行版，建议使用 `apt` 命令安装：

.. code-block:: shell-session

    $ sudo apt update
    $ sudo apt install shadowsocks-libev

.. note::
    注意：版本较低的系统没有 ``apt`` 命令，请使用 ``apt-get`` 命令，效果一样。

如果系统源中还没有 ``shadowsocks-libev`` 包导致安装失败，
可以先在 ``/etc/apt/sources.list.d`` 目录内新增一个兼容配置，再运行上述安装命令：

.. code-block:: shell-session

    $ sudo sh -c 'printf "deb http://httpredir.debian.org/debian jessie-backports main" > /etc/apt/sources.list.d/jessie-backports.list'

``shadowsocks-libev`` 包提供了 ``ss-server`` 以及 ``ss-local`` 等命令，用法跟 `Python` 版本的类似。
当然了，我们不会直接运行这些命令。
因为 ``shadowsocks-libev`` 包也提供了标准的配置文件以及启停脚本，这种服务管理方式更加科学。

安装完成， ``ss-server`` 就在后台运行了。
配置是默认的，可以查看 ``/etc/shadowsocks-libev/config.json`` 文件。
格式如下：

.. code-block:: json

    {
        "server": "example.com or X.X.X.X",
        "server_port": 443,
        "password": "password",
        "timeout": 300,
        "method": "aes-256-cfb"
    }

比较关键的配置是服务器监听 **地址** 及 **端口** 、 **加密密钥** 以及 **加密算法** ，本地端配置时必须与这里一致。

用户可以根据自己特定的场景，调整配置文件。
配置配置后，需运行以下命令重启服务方能生效：

.. code-block:: shell-session

    $ sudo service shadowsocks-libev restart

.. _ss-local:

本地端
======

本地端部署在境内，可以是一台独立的 :ref:`ss-local-server` ，也可以是 ``PC`` 或移动设备上一个 :ref:`ss-local-client` 。

.. _ss-local-client:

客户端应用
----------

这种方式配置起来最简单，只需要在需要翻墙的设备上下载一个客户端软件，填一下配置即可。
请在 `shadowsocks 客户端 <https://shadowsocks.org/en/download/clients.html>`_ 上找到合适的版本并下载安装。

客户端安装完毕，任务栏上图标可以看到操作菜单：

.. figure:: /_images/services/shadowsocks/7fd2db7ae2ff76b72c80fb37ec2a5704.png
    :width: 282px

    *操作菜单*

点击服务器，可以看到已经配置的服务端列表，支持配置多个服务端，快速切换：

.. figure:: /_images/services/shadowsocks/4834c68c408cab86dffc0b3e1883fd23.png
    :width: 282px

    *服务器列表*

点击服务器设置，可以配置一个新的服务端：

.. figure:: /_images/services/shadowsocks/54a6f208109c0d2f797a7a90ead24bfe.png
    :width: 590px

    *服务器设置*

需要填上服务端 **地址** 及 **端口** ， **加密密钥** 以及 **加密算法** 。

.. warning::
    注意，必须与服务端配置保持一致！

接下来，确保勾选 :ref:`ss-local-client-pac-auto` ，便可实现翻墙了！
先跟 `Google <https://www.google.com/>`_ 问个好： :ref:`hi-google` 。

在这个场景，shadowsocks 客户端应用扮演 :ref:`ss-local` 角色，与 :ref:`ss-server` 建立加密隧道。
系统及其他应用则是 :ref:`ss-client` 的角色，网络请求直接代理到客户端应用。

回过头来对比一下三种代理模式有什么区别：

.. _ss-local-client-pac-auto:

PAC自动模式
+++++++++++

选择该模式，客户端将向系统加入代理配置。

对于 `OSX` 系统，打开 **系统偏好设置** -> **网络** -> **高级** -> **自动代理配置** 可以查看：

.. figure:: /_images/services/shadowsocks/d6532d5c297cbb741d28e2608bac7aef.png
    :width: 668px

    *代理设置*

设置代理配置后，系统及流量器等其他应用均通过代理上网。

注意到，只有 `PAC` 文件罗列的受 `GFW` 影响的站点走代理。
其他流量不走代理直接访问，更有效率。
因此，:ref:`ss-local-client-pac-auto` 是最推荐的模式。

那么，怎么知道哪些站点被墙呢？
—— `Github <https://github.com>`_ 上有一个项目，维护了受 `GFW` 影响的站点列表： `gfwlist <https://github.com/gfwlist/gfwlist>`_ 。
此外，还有一个好用的工具 `gfwlist2pac <https://pypi.python.org/pypi/gfwlist2pac>`_ ，根据 `gfwlist <https://github.com/gfwlist/gfwlist>`_ 生成 `PAC` 文件。

.. _ss-local-client-global:

全局模式
++++++++

该模式与 :ref:`ss-local-client-pac-auto` 类似，不同的是：系统所有流量都走代理。
这样，访问国内网站需要到国外兜一圈再回来，速度很慢很慢。
因此，一般不采用 :ref:`ss-local-client-global` 。

.. _ss-local-client-manual:

手动模式
++++++++

该模式下，客户端应用只提供代理服务，并不向系统添加任何代理配置。

.. figure:: /_images/services/shadowsocks/87f0c64ca5e4550c99d90611b9291a20.png
    :width: 480px

    *客户端应用提供的代理服务*

因此，需要走代理的应用要手动配置一下。
举个例子， `Chrome` 浏览器通过 `Proxy SwitchyOmega <https://chrome.google.com/webstore/detail/proxy-switchyomega/padekgcemlokbadohgkifijomclgjgif?hl=zh-CN>`_ 插件将请求代理过去。
插件配置方式如下：

.. figure:: /_images/services/shadowsocks/b0dee7132ba67802f5d75b6ee0eef32e.png
    :width: 817px

    *Chrome代理设置*

:ref:`ss-local-client-manual` 一般也不采用，除非只想让某些应用使用代理。

.. _ss-local-server:

服务器
------

:ref:`ss-local-client` 这种部署方式已经足够简便，唯一的不足在于每台需要使用代理的设备都要安装。
而且， `iOS` 系统限制严格，客户端在后台运行一段时间后就被系统强制暂停，需要时不时打开激活。

如果国内也有台服务器，可以将 :ref:`ss-local` 部署成服务器模式。
由于 :ref:`ss-local` 提供的代理协议是 `socks` 或者 `http` 等常见协议，使用代理的设备或软件不需要再另行安装客户端。

本地端部署需要安装的包与 :ref:`ss-server` 相同，不再赘述。

下面看看本地端服务如何启动：

Python
++++++

使用 `Python` 方式安装，需要手工启动服务：

.. code-block:: shell-session

    $ sslocal -s $SERVER_ADDR -p $SERVER_PORT -b $BIND_ADDR -l $BIND_PORT -k $PASSWORD -m $METHOD

服务启动后，与 :ref:`ss-server` 连接建立加密隧道，并提供一个 `socks5` 代理服务。

其中， ``-s`` 与 ``-p`` 选项指定 :ref:`ss-server` **地址** 与 **端口** ；
``-b`` 与 ``-l`` 选项指定 `socks5` 服务监听的 **地址** 与 **端口** ；
``-k`` 与 ``-k`` 选项指定 **加密密钥** 与 **加密算法** 。

再次强调： **加密密钥** 与 **加密算法** 配置必须与 :ref:`ss-server` 保持一致。

Apt
+++

.. _ss-client:

客户端
======

:ref:`ss-local` 部署成服务器，需要手动配置客户端。
配置方式虽随系统环境而异，但大多简单易懂。

上文提到 `OSX` 系统以及 `Chrome` 浏览器代理设置方式，可供读者参考。
其他系统及应用软件的设置方式也是类似的，不再赘述。

.. _hi-google:

Hi, Google!
===========

经过一番努力之后，可以正常使用 `Google <https://www.google.com/>`_ 等工具了！尽情享用吧！

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `写给非专业人士看的 Shadowsocks 简介 <https://vc2tea.com/whats-shadowsocks/>`_
#. `Shadowsocks 折腾记 <https://thief.one/2017/02/22/Shadowsocks%E6%8A%98%E8%85%BE%E8%AE%B0/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Just My Socks: https://justmysocks.net/members/aff.php?aff=6159
.. _Linode: https://www.linode.com/?r=8e45c4effcbbafe17fd593b5064d219bfab37e7d

.. comments
    comment something out blow

