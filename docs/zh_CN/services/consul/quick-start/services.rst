.. 注册服务
    FileName:   services.rst
    Author:     Fasion Chan
    Created:    2019-01-22 20:03:09
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

    .. meta::
        :description lang=zh:
            上节，我们成功运行Consul代理，查看集群成员，并通过HTTP接口查询该节点。
            本节，我们再接再厉，尝试注册一个服务(service)并查询服务信息。
        :keywords: consul, service, registering, service catalog, 服务, 服务注册, 服务发现, 服务目录

========
注册服务
========

上节，我们成功运行 `Consul`_ 代理，查看集群成员节点，并试用 `HTTP` 查询接口。
本节，我们再接再厉，尝试 **注册** 一个 **服务** ( `service` )并 **查询** 服务信息。

定义服务
========

`Consul` 有两种不同方式可以 **注册服务** ：提供 **服务配置** 文件，或者调用 `HTTP`  **接口** 。

**服务配置** 文件是最常用的服务注册方式，因此我们先采用这种方式。
接下来，为 :doc:`上节 <agent>` 运行的代理准备配置。

首先，为 `Consul`_ 配置 **创建目录** 。
`Consul` 将加载该目录下所有配置文件，因此可按 `Unix` 惯例为目录命名，
例如 ``/etc/consul.d`` ( ``.d`` 后缀表示存放配置文件的目录 )。

.. code-block:: shell-session

    $ mkdir ./consul.d

接着，写一个配置文件来定义服务。
假设有一个名为 ``web`` 的服务，运行在 *80* 端口。
除了基本信息，还可以定义额外的 **标签** ( `tag` )用于过滤查询：

.. code-block:: shell-session

    $ echo '{"service": {"name": "web", "tags": ["rails"], "port": 80}}' \
        > ./consul.d/web.json

现在，可以重启代理并指定配置目录：

.. code-block:: shell-session

    $ consul agent -dev -config-dir=./consul.d

    ==> Starting Consul agent...
    ...
        [INFO] agent: Synced service 'web'
    ...

从代理输出可以看到， `Consul` 已经同步了 ``web`` 服务。
这意味着，代理已经成功从配置文件 **加载** 服务定义，并且注册到 **服务目录** ( `service catalog` )。

如需注册多个服务，可在配置目录下创建多个配置文件分别定义。

.. note::

    在生产环境，你需要为 *80* 端口部署一个可用的服务，并为其配置 :doc:`健康检查 <checks>` 。
    本文由于篇幅关系，省略了这个步骤。

查询服务
========

一旦代理重启完毕且服务已同步，用户便可通过 `DNS` 接口或者 `HTTP` 接口查询服务。

DNS API
-------

先通过 `DNS` 接口查询服务，服务域名为 ``NAME.service.consul`` 。
默认，所有域名均在 ``consul`` 名字空间内，但可通过 `-domain <https://consul.io/docs/agent/options.html#domain>`_ 参数调整。
``service`` **子域名** ( `subdomain` )表示查询 **服务** 信息， ``NAME`` 则是服务名。

对于我们注册的 ``web`` 服务，域名为： ``web.services.consul`` ：

.. code-block:: shell-session

    $ dig @127.0.0.1 -p 8600 web.service.consul

    ;; QUESTION SECTION:
    ;web.service.consul.        IN  A

    ;; ANSWER SECTION:
    web.service.consul. 0   IN  A   127.0.0.1

可以看到， `DNS` 接口返回一条 `A记录` ，包含服务可用节点的 `IP` 地址。
注意到， `A记录` 只能返回 `IP` 地址。

用户可以让 `DNS` 接口返回 `SRV记录` ，以获取完整的 **地址端口对** ( `address/port pair` )：

.. code-block:: shell-session

    $ dig @127.0.0.1 -p 8600 web.service.consul SRV

    ;; QUESTION SECTION:
    ;web.service.consul.            IN      SRV

    ;; ANSWER SECTION:
    web.service.consul.     0       IN      SRV     1 1 80 FasionAir.local.node.dc1.consul.

    ;; ADDITIONAL SECTION:
    FasionAir.local.node.dc1.consul. 0 IN   A       127.0.0.1
    FasionAir.local.node.dc1.consul. 0 IN   TXT     "consul-network-segment="

`SRV记录` 表明， ``web`` 服务跑在 ``FasionAir.local.node.dc1.consul.`` 节点 *80* 端口。
附加节额外返回该节点的 `A记录` 。

此外，使用 `DNS` 接口时，还可以通过 **标签** ( `tag` ) **过滤服务** 。
基于标签的查询格式是： ``TAG.NAME.service.consul`` 。
接下里，查询所有带 ``rails`` 标签的 ``web`` 服务：

.. code-block:: shell-session

    $ dig @127.0.0.1 -p 8600 rails.web.service.consul

    ;; QUESTION SECTION:
    ;rails.web.service.consul.      IN  A

    ;; ANSWER SECTION:
    rails.web.service.consul.   0   IN  A   127.0.0.1

我们注册服务时确实带上 ``rails`` 标签，因此接口可以正确响应。

HTTP API
--------

除了 `DNS` 接口，还可以通过 `HTTP` 接口查询服务：

.. code-block:: shell-session

    $ curl http://localhost:8500/v1/catalog/service/web
    [
        {
            "ID": "4116333b-bf68-36d9-25ed-192022bf9a68",
            "Node": "FasionAir.local",
            "Address": "127.0.0.1",
            "Datacenter": "dc1",
            "TaggedAddresses": {
                "lan": "127.0.0.1",
                "wan": "127.0.0.1"
            },
            "NodeMeta": {
                "consul-network-segment": ""
            },
            "ServiceKind": "",
            "ServiceID": "web",
            "ServiceName": "web",
            "ServiceTags": [
                "rails"
            ],
            "ServiceAddress": "",
            "ServiceWeights": {
                "Passing": 1,
                "Warning": 1
            },
            "ServiceMeta": {},
            "ServicePort": 80,
            "ServiceEnableTagOverride": false,
            "ServiceProxyDestination": "",
            "ServiceProxy": {},
            "ServiceConnect": {},
            "CreateIndex": 10,
            "ModifyIndex": 10
        }
    ]

**服务目录** `API` 返回运行指定服务的所有节点。
通常，我们只想要那些通过 :doc:`健康检查 <checks>` 的服务实例，可以这样查询：

.. code-block:: shell-session

    $ curl 'http://localhost:8500/v1/health/service/web?passing'

更新服务
========

服务定义更新步骤如下：

#. 修改配置文件；
#. 向代理发送 `SIGHUP` 信号；

更新过程无需停止服务，因此查询接口不会中断。

此外， `HTTP` 接口也可以用来动态 **添加** 、 **删除** 以及 **修改** 服务定义。

下一步
======

到目前为止，我们已经成功配置一个单节点代理并注册了一个服务。
其他服务定义字段可以在 `API文档 <https://www.consul.io/api/agent/service.html>`_ 中查阅。

接下来，我们进一步探索 `Consul`_ 其他功能——如何通过 :doc:`connect` 自动为服务间通讯提供 **加密** ( `encrypt` )及 **鉴权** ( `authorize` )功能。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Registering Services | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/services>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

