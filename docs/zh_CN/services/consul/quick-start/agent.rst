.. 运行代理
    FileName:   run-agent.rst
    Author:     Fasion Chan
    Created:    2019-01-22 20:14:53
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        Consul安装完毕后，便可运行代理(agent)了。
        代理有两种不同的运行模式：服务器(server)模式以及客户端(client)模式。
        每个数据中心(datacenter)至少需要部署一个服务器，然而3节点或5节点的集群更为推荐。
        单机器部署强烈不推荐，因为一旦故障，数据丢失不可避免。
    :keywords: consul, agent, server, client, graceful leave, 优雅退出

========
运行代理
========

`Consul`_ 安装完毕后，便可运行 **代理** ( `agent` )了。
代理有两种不同的运行模式： **服务器** ( `server` )模式以及 **客户端** ( `client` )模式。
每个 **数据中心** ( `datacenter` )至少需要部署一个 **服务器** ，
然而 **3** 节点或 **5** 节点的集群更为推荐。
**单机器部署强烈不推荐** ，因为一旦故障，数据丢失不可避免。

其他节点以 **客户端** 模式运行。
**客户端** 进程非常轻量级，只负责 **注册服务** ，运行 **健康检查** 以及 **转发查询** 。
集群内每个节点均需要运行 **客户端** 模式 **代理** 。

更多数据中心部署细节，请查看文档 `deployment-guide <https://learn.hashicorp.com/consul/advanced/day-1-operations/deployment-guide>`_ 。

启动代理
========

快速起见，我们以 **开发** ( `development` )模式启动 `Consul`_ **代理** ，
这个模式可快速拉起一个单节点的 `Consul` 环境。
**千万不要在生产环境中使用该模式，因为数据没有持久化** 。

.. code-block:: shell-session

    $ consul agent -dev
    ==> Starting Consul agent...
    ==> Consul agent running!
               Version: 'v1.4.0'
               Node ID: 'c3714f95-4487-8862-17fd-68030f0c275d'
             Node name: 'FasionAir.local'
            Datacenter: 'dc1' (Segment: '<all>')
                Server: true (Bootstrap: false)
           Client Addr: [127.0.0.1] (HTTP: 8500, HTTPS: -1, gRPC: 8502, DNS: 8600)
          Cluster Addr: 127.0.0.1 (LAN: 8301, WAN: 8302)
               Encrypt: Gossip: false, TLS-Outgoing: false, TLS-Incoming: false

    ==> Log data will now stream in as it occurs:

        2019/01/23 14:41:10 [DEBUG] agent: Using random ID "c3714f95-4487-8862-17fd-68030f0c275d" as node ID
        2019/01/23 14:41:10 [WARN] agent: Node name "FasionAir.local" will not be discoverable via DNS due to invalid characters. Valid characters include all alpha-numerics and dashes.
        ......

可以看到， `Consul` 已经启动并输出一些日志信息。
从日志可知， **代理** 以 **服务器** 模式运行，并作为集群领袖。
注意到，当前节点被标记为健康，并成为集群唯一节点。

集群成员
========

在另一个终端会话执行 ``consul members`` ，可以查看 `Consul`_ 集群所有成员。
可以看到，当前集群中只有一个成员，:doc:`下节 <join>` 介绍如何添加新成员。

.. code-block:: shell-session

    $ consul members
    Node             Address         Status  Type    Build  Protocol  DC   Segment
    FasionAir.local  127.0.0.1:8301  alive   server  1.4.0  2         dc1  <all>

命令输出集群节点信息，包括 **地址** 、 **健康状态** 、 **节点角色** 以及一些 **版本信息** 。
如果指定 ``-detailed`` 参数，还可以看到额外的 **元数据** 。

``members`` 指令输出基于 `gossip协议 <https://consul.io/docs/internals/gossip.html>`_ ，只保证 **最终一致** ( `eventually consistent` )。
换句话讲，不管何时，当前 **代理** 所见可能并不是集群的全貌。
如果需要 **强一致** ( `strongly consistent` )的结果，可用使用 `HTTP` 接口进行查询(请求将转发至服务器)：

.. code-block:: shell-session

    $ curl localhost:8500/v1/catalog/nodes
    [
        {
            "ID": "c735ff68-f8ce-3011-9fcf-55ab81f7e21d",
            "Node": "FasionAir.local",
            "Address": "127.0.0.1",
            "Datacenter": "dc1",
            "TaggedAddresses": {
                "lan": "127.0.0.1",
                "wan": "127.0.0.1"
            },
            "Meta": {
                "consul-network-segment": ""
            },
            "CreateIndex": 9,
            "ModifyIndex": 10
        }
    ]

除了 `HTTP` 接口，还可以使用 `DNS` 接口查询节点信息。
注意，你需要将 `DNS` 查询指到 `Consul` 代理的 `DNS` 服务器，默认跑在 *8600* 端口。

.. code-block:: shell-session

    $ dig @127.0.0.1 -p 8600 FasionAir.local.node.consul

    ;; QUESTION SECTION:
    ;FasionAir.local.node.consul.   IN      A

    ;; ANSWER SECTION:
    FasionAir.local.node.consul. 0  IN      A       127.0.0.1

`DNS` 记录格式(如 ``FasionAir.local.node.consul`` )，将在 :doc:`下节 <services>` 详细介绍。

停止代理
========

可以通过 `Ctrl-C` (中断信号 `SIGINT` ) 让代理优雅地退出——先离开集群，然后退出。

**优雅退出** 要求 `Consul` 在退出前先通知集群其他节点。
如果 **强杀** 代理进程，集群其他节点将检测到该节点 **故障** 。
集群成员退出，其服务以及健康检查任务将从目录中移除。
集群成员故障，其健康状态将被标记为 **严重错误** ，但不会从目录中移除。
`Consul` 将自动重连故障节点，以应对网络波动，但不会重连已退出的节点。

如果 **代理** 为 **服务器** 模式，优雅退出则更为重要——避免影响 **共识协议** 而导致服务中断。
更多关于安全添加和删除服务节点，请查考文档 `advanced-operations/servers <https://learn.hashicorp.com/consul/day-2-operations/advanced-operations/servers>`_ 。

下一步
======

下一节，开始介绍： :doc:`如何向Consul注册服务 <services>` 。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Run the Consul Agent | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/agent>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

