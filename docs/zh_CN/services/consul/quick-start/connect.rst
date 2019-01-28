.. 互连服务
    FileName:   connect.rst
    Author:     Fasion Chan
    Created:    2019-01-23 17:55:28
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        Consul还提供互连(Connect)功能，支持TLS加密以及权限控制。
        应用无需任何修改便可使用互连(Connect)功能，
        Consul提供挎斗代理(sidecarproxy)，自动为入站或出站连接开启TLS，
        因此互连可以做到对应用完全透明。
        当然了，应用也可以与互连原生集成，以追求更高的性能和安全性。
    :keywords: consul, connect, 互连, TLS, 加密, 权限控制

========
互连服务
========

前面我们成功往 `Consul`_ 注册服务，并使用 `DNS` 和 `HTTP` 接口查询服务地址，据此可直接连接服务。
`Consul` 还提供 **互连** ( `Connect` )功能，支持 `TLS` **加密** 以及 **权限控制** 。

应用无需任何修改便可使用 **互连** ( `Connect` )功能，
`Consul` 提供 **挎斗代理** ( `sidecar proxy` )，自动为 **入站** 或 **出站** 连接开启 `TLS` ，
因此 **互连** 可以做到对应用 **完全透明** 。
当然了，应用也可以与 **互连** 原生集成，以追求更高的 **性能** 和 **安全性** 。

启动互连无感服务
================

我们先从启动一个 **互连无感** ( `Connect-unaware` )服务开始。
快速起见，就用 :doc:`socat </toolkit/socat>` 命令启动一个简单的 **回显** ( `echo` )服务。
这个服务接收 `TCP` 连接请求，接到数据后原样回传。
如果系统未安装 *socat* 命令，可通过 **包管理工具** 进行安装。

.. code-block:: shell-session

    $ socat -v tcp-l:8181,fork exec:"/bin/cat"

然后，使用 :doc:`nc </toolkit/nc>` 命令进行连接，以验证服务功能。
连上之后，输入一些文本并按回车，输入的内容将被传送回来：

.. code-block:: shell-session

    $ nc 127.0.0.1 8181
    hello
    hello
    echo
    echo

:doc:`socat </toolkit/socat>` 是一个非常古老的 `Unix` 工具，只能处理原始的 `TCP` 连接。
它没有加密的概念，也不支持 `TLS` 协议，
刚好类比一个非常典型的历史应用，如 **数据库** 、 `Web` 服务等。

注册服务及互连
==============

接着，我们将 ``socat`` 服务注册到 `Consul`_ 。
与先前一样，只需编写一个服务定义文件，只不过这次需要额外提供 **互连** 配置( `connect` 字段 )：

.. code-block:: shell-session

    $ cat <<EOF | sudo tee ./consul.d/socat.json
    {
      "service": {
        "name": "socat",
        "port": 8181,
        "connect": { "sidecar_service": {} }
      }
    }
    EOF

配置保存后，执行 ``consul reload`` 或者向 `Consul` 发送 `SIGHUP` 信号，加载新配置。

注意到，唯一不同的地方是 ``"connect"`` 开头那行。
这行空配置通知 `Consul` 为服务注册一个 **挎斗代理** 。
代理作为 ``socat`` 服务的代表，动态分配监听端口，接收入站连接，
检查 `TLS` 连接授权，最后通过标准的 `TCP` 连回服务。

**挎斗代理** 注册只是通知 `Consul` 服务需要代理， `Consul` 并不会自动运行代理进程。
我们需要在另一个终端启动代理进程：

.. code-block:: shell-session

    $ consul connect proxy -sidecar-for socat
    ==> Consul Connect proxy starting...
        Configuration mode: Agent API
            Sidecar for ID: socat
                  Proxy ID: socat-sidecar-proxy

    ...

注意到，代理输出的日志中有以下一行内容：

.. code-block:: text

    TLS Identity: spiffe://878e0a5e-842a-1acf-939b-805c3594ce8f.consul/ns/default/dc/dc1/svc/socat

这表明，代理使用 ``socat`` 服务的 **身份** ，检查入站连接是否得到 **授权** 。

.. figure:: /_images/services/consul/quick-start/connect/df8c118bfac75e09bb4e3defcbd07ae4.png

连接服务
========

现在，可以连接服务了，还是先直接使用 ``consul connect proxy`` 命令。
这次，我们使用该命令运行一个代表某个服务访问其他服务的本地代理。
这个工具在开发阶段很有用，可以伪装成任何服务，并通过 **互连** 功能连接其他服务。

下面这个命令，以 ``web`` 服务为身份启动一个代理，
监听 `9191` 端口，并将请求转发到前一小节注册的 ``socat`` 服务。

.. code-block:: shell-session

    $ consul connect proxy -service web -upstream socat:9191
    ==> Consul Connect proxy starting...
        Configuration mode: Flags
                   Service: web
                  Upstream: socat => :9191
           Public listener: Disabled

    ...

在这个配置下，代理从 `9191` 端口收到连接请求后，
通过 **服务发现** 找到 ``socat`` 服务的 **互连** 代理，
并以 ``web`` 服务为身份与其建立 `TLS` 连接。

注意到，代理输出的日志中有以下一行内容：

.. code-block:: text

    TLS Identity: spiffe://878e0a5e-842a-1acf-939b-805c3594ce8f.consul/ns/default/dc/dc1/svc/web

这表明，代理使用 ``web`` 服务的 **身份** ，外出访问 ``socat`` 服务。

.. figure:: /_images/services/consul/quick-start/connect/8ddad7d692e93846d55018e8ee727116.png

代理跑起来后，连上去验证是否正常：

.. code-block:: shell-session

    $ nc 127.0.0.1 9191
    hello
    hello

注意到，代理之间的连接现在是 **加密** 而且有 **权限认证** 的。
我们成功通过 `TLS` 连接访问 ``socat`` 服务。
与代理之间的本地连接还是非加密的，就算在生产环境也无妨，因为本地连接只会走 **环回网卡** 。
任何跨机器的网络访问一定都是加密的。

此外，如果 ``web`` 和 ``socat`` 应用原生集成 **互连** 功能，则可以省略代理直接访问：

.. figure:: /_images/services/consul/quick-start/connect/1615e82037d8b9c312f12c9428e37482.png
    :width: 356px

代理的引入，让服务能够 **非侵入地** 对接 **互连** 功能，但也加大了网络开销。

注册依赖型服务
==============

上一小节，我们以 **开发模式** 临时运行代理，即直接执行： ``consul connect proxy`` 。
实际上，我们需要在 **互连** 体系内注册 **连接依赖** 。
我们重新注册 ``web`` 服务，添加依赖 ``socat`` 服务的 **挎斗代理** 配置。

.. code-block:: shell-session

    $ cat <<EOF | sudo tee ./consul.d/web.json
    {
      "service": {
        "name": "web",
        "port": 8080,
        "connect": {
          "sidecar_service": {
            "proxy": {
              "upstreams": [{
                 "destination_name": "socat",
                 "local_bind_port": 9191
              }]
            }
          }
        }
      }
    }
    EOF

这表明， ``web`` 服务的挎斗代理必须监听本地 `9191` 端口，并以 ``web`` 为身份将连接转发到 ``socat`` 服务。
后续 ``web`` 服务必须通过本地端口与 ``socat`` 通讯，而不是直接连接。

配置文件准备就绪，同样通过 `consul reload` 或 `SIGHUP` 信号重新加载。
如果上节为 ``web`` 启动的 **挎斗代理** 还在，先用 `Ctrl-C` 将其停止。
现在，可以用新配置为 ``web`` 服务启动代理了，方法与 ``socat`` 一样：

.. code-block:: shell-session

    $ consul connect proxy -sidecar-for web
    ==> Consul Connect proxy starting...
        Configuration mode: Agent API
            Sidecar for ID: web
                  Proxy ID: web-sidecar-proxy

    ==> Log data will now stream in as it occurs:

        2019/01/24 14:39:05 [INFO] 127.0.0.1:9191->service:default/socat starting on 127.0.0.1:9191
        2019/01/24 14:39:05 [INFO] Proxy loaded config and ready to serve
        2019/01/24 14:39:05 [INFO] TLS Identity: spiffe://5fe1d51b-0f62-acc9-17bf-dff2a7caea18.consul/ns/default/dc/dc1/svc/web
        2019/01/24 14:39:05 [INFO] TLS Roots   : [Consul CA 7]

注意到，第一行日志显示代理从 `Consul` 获得配置，并开始监听本地 `9191` 端口，
连接被代理到 ``socat`` 服务，符合预期。

此外，还可以看到代理从 `Consul` 那加载了代表 ``web`` 服务的证书，以及对应的根证书。

最后，发起新连接验证效果：

.. code-block:: shell-session

    $ nc 127.0.0.1 9191
    hello
    hello

控制访问
========

`Consul` 还实现了 **意图** ( `intention` )这个概念，用来控制哪些服务可以被访问。
在上面的实验中，可以成功连接，这是因为 `ACL` 系统默认策略是 **全部放行** 。

接下来，插入一条规则，禁止从 ``web`` 到 ``socat`` 的访问：

.. code-block:: shell-session

    $ consul intention create -deny web socat
    Created: web => socat (deny)

再次访问 ``web`` 服务的 **挎斗代理** ，发现已经无法连接了：

.. code-block:: shell-session

    $ nc 127.0.0.1 9191
    $

现在尝试将规则删除，并再次尝试连接服务：

.. code-block:: shell-session

    $ consul intention delete web socat
    Intention deleted.
    $ nc 127.0.0.1 9191
    hello
    hello

顺便提一下，对于当前版本 `Consul` ，修改 **意图** 规则不会影响已经建立的连接。
因此，规则修改之后，需要发起新连接，确认调整已经生效。

互连意义
========

`Consul`_ **互连** 这个特性最大的意义在于， **将访问控制与服务IP解耦** 。

传统服务访问控制，最典型的要数 `IP` 黑/白名单。
但在 **服务容器化** 的时代，服务 `IP` 频繁变动，不太好组织 `IP` 名单？
好在 `Consul`_ 为每个服务提供 `TLS` 认证证书，服务访问可以通过 `TLS` 协议进行 **身份证认证** ，
完全剥离对服务 `IP` 的依赖。

- 访问控制与 `IP` 解耦；
- 加密流量；

下一步
======

下一节，我们介绍： :doc:`如何搭建Consul集群 <join>` 。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Connect | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/connect>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

