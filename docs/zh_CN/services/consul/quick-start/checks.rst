.. 健康检查
    FileName:   checks.rst
    Author:     Fasion Chan
    Created:    2019-01-22 20:22:18
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        本节，我们进一步为节点和服务添加检查检查(healthcheck)。
        健康检查是服务发现(servicediscovery)的重要组成部分，可避免访问不健康的服务实例。
    :keywords: consul, health check, 健康检查

========
健康检查
========

前面我们将 `Consul`_ 跑起来，添加 **节点** ( `node` )和 **服务** ( `service` )，
并通过接口 **查询** 服务信息，差不多都是小菜一碟。

本节，我们进一步为 **节点** 和 **服务** 添加 **检查检查** ( `health check` )。
**健康检查** 是 **服务发现** ( `service discovery` )的重要组成部分，可避免访问不健康的服务实例。

我们在 :doc:`上节 <join>` 搭建的集群基础上进行试验，这是一个双节点集群。

定义检查
========

与 :doc:`服务 <services>` 类似，健康检查可通过 **配置文件** 或 `HTTP` 接口调用进行注册。

这里我们还是采用配置文件的方式，因为跟服务配置一样，这种方式 **最常用** 。

`Consul`_ *0.9.0* 以后， **代理** ( `agent` )需要将 ``enable_script_checks`` 配置设置为 ``true`` ，
以开启脚本检查。

在第二个节点 `Consul`_ 配置目录下创建两个配置文件：

.. code-block:: shell-session

    vagrant@n2:~$ echo '{"check": {"name": "ping", \
      "args": ["ping", "-c1", "baidu.com"], "interval": "30s"}}' \
      >/etc/consul.d/ping.json

.. code-block:: shell-session

    vagrant@n2:~$ echo '{"service": {"name": "web", "tags": ["rails"], \
      "port": 80, "check": {"args": ["curl", "localhost"], \
      "interval": "10s"}}}' >/etc/consul.d/web.json

第一个配置 ``ping`` 是 **节点检查** ( `host-level check` )，
每隔 *30* 秒调用 ``ping -c1 baidu.com`` 一次。
**脚本式** ( `script-based` )检查执行用户与启动 `Consul` 的用户一致。
如果命令返回值大于 **2** ，则认为检查 **失败** ，对应的服务也是 **不健康** 的。
如果返回值是 **1** ，则认为服务处于 **预警** ( `warning` )状态。
这个约定对于所有 **脚本式** 检查均适用。

第二个配置在 ``web`` 服务基础上，为其添加检查定义，每隔 *10* 秒钟调用 `curl` 判断服务是否可用。
与 **节点检测** 一样，如果命令返回值大于 **2** ，则认为检查 **失败** ，对应的服务将被标记为 **不健康** 。


现在，通过 ``consul reload`` 或者发送 ``SIGHUP`` 信号重启第二个 **代理** 。
接下来， **代理** 将出现以下日志：

.. code-block:: text

    ==> Starting Consul agent...
    ...
        [INFO] agent: Synced service 'web'
        [INFO] agent: Synced check 'service:web'
        [INFO] agent: Synced check 'ping'
        [WARN] Check 'service:web' is now critical

前几行表明， **代理** 已经同步了新配置。
最后一行表明，我们为 ``web`` 服务添加的健康检查发现严重错误。
这是因为，我们并没有运行 `Web` 服务， `curl` 检查一定会失败。

检查健康状态
============

我们已经添加了两个健康检查定义，现在可以通过 `HTTP` 查看结果。
首先，我们用以下命令查看所有 **不通过** 的检查(可以在任意节点运行)：

.. code-block:: shell-session

    vagrant@n1:~$ curl http://localhost:8500/v1/health/state/critical
    [{"Node":"agent-two","CheckID":"service:web","Name":"Service 'web' check","Status":"critical","Notes":"","ServiceID":"web","ServiceName":"web","ServiceTags":["rails"]}]

由此可见，只有一个检查处于严重错误状态，这就是我们的 ``web`` 服务。

此外，我们尝试通过 `DNS` 查询 ``web`` 服务， `Consul` 并不返回任何地址因为服务不健康。

.. code-block:: shell-session

    vagrant@n1:~$ dig @127.0.0.1 -p 8600 web.service.consul

    ;; QUESTION SECTION:
    ;web.service.consul.        IN  A

总结
====

本节，我们通过亲身试验发现，添加健康查也非常简单。
更新检查定义只需编辑配置文件，并向 **代理** 发送 `SIGHUP` 信号。
另外，也可以使用 `HTTP` 接口动态 **添加** 、 **删除** 、 以及 **修改** 检查定义。
`API` 还支持 **失知制动** ( `dead man's switch` )，一种基于 `TTL`_ 的检查。
`TTL` 检查让应用与 `Consul` 集成更紧密，业务逻辑判断可成为健康检查的一部分。

下一步
======

下一节，我们介绍： :doc:`kv` 。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Registering Health Checks | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/checks>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/
.. _TTL: https://consul.io/docs/agent/checks.html#TTL

.. comments
    comment something out below

