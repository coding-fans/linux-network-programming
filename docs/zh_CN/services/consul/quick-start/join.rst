.. Consul集群
    FileName:   join.rst
    Author:     Fasion Chan
    Created:    2019-01-24 17:55:02
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        本文讨论可扩展的生产级架构，但这是构建服务管理基础设施的关键。
        现在，我们要搭建一个真实的集群，由多个成员节点组成。
    :keywords: consul, cluster, 集群, scalable, 可扩展, 生产级架构

==========
Consul集群
==========

我们成功运行一个 **代理** ( `agent` )，在其上注册了 **服务** ( `service` )，并发起查询。
此外，我们也配置了 `Consul` **互连** ( `Connect` )，实现服务间 **连接加密** 和 **权限控制** 。
为了尽量简洁地演示 `Consul` 的使用方法，我们没有讨论 **可扩展** 的 **生产级架构** ，
但这是构建服务管理基础设施的关键。
现在，我们要搭建一个真实的 **集群** ，由多个成员节点组成。

`Consul`_ **代理** 启动后，刚开始对其他节点一无所知。
换句话讲，它是一个孤立的单节点集群。
**代理** 需要加入一个现有的集群，以便发现其他集群成员。
只要知道集群中任一成员，即可加入该集群。
加入之后， **代理** 将与该成员通讯，发现其他集群成员。
**代理** 可以加入其他任何 **代理** ，不仅仅是 **服务器** 模式的那些。

启动代理
========

为了模拟更真实的集群环境，我们借助 `Vagrant`_ 工具创建两个 **节点** ( `node` )。
`Vagrantfile` 可以在 `这里 <https://github.com/hashicorp/consul/tree/master/demo/vagrant-cluster>`_ 找到。

先启动实验所需的两个节点：

.. code-block:: shell-session

    $ ls
    Vagrantfile
    $ vagrant up

一旦系统就绪，我们便可 `ssh` 登录上去，开始配置 `Consul`_ 集群。
先登录第一个节点：

.. code-block:: shell-session

    $ vagrant ssh n1

前面，我们使用了 ``-dev`` 参数，以便快速启动服务，方便开发调试。
然而，这在集群环境中是不适合的。
之后，我们将抛弃 ``-dev`` 参数，并用其他集群参数来替换。

集群内每个节点名字必须唯一。
默认， `Consul` 使用 `机器名`_ 作为节点名，但我们将通过 ``-node`` 参数显示指定。

另外，我们通过 ``-bind`` 参数指定 **绑定地址** ( `bind address` )，
这是 `Consul` 的 **监听地址** ，必须能被其他节点访问到。
严格上讲， **绑定地址** 不是必要的，但提供一下会更好。
默认， `Consul` 会监听所有 `IPv4` 接口，但可能会提示因 **私有地址不唯一** 而启动失败。
由于生产服务器一般是 **多网卡** 的，指定 **绑定地址** 可避免 `Consul` 错误监听其他网卡。

第一个节点作为集群唯一一个 **服务器模式** 节点，需要指定 ``-server`` 参数。

``-bootstrap-expect`` 参数指定集群 **节点数** ，
可让 **日志复制** ( `replicated log` )推迟到所有节点均成功加入之后才开始。
`这个文档 <https://consul.io/docs/guides/bootstrapping.html>`_ 有更详细的介绍。

还有，我们将 ``-enable-script-checks`` 设置为 ``true`` 以便可以执行外部脚本进行 :doc:`checks` 。
这个功能我们会在 :doc:`下节 <checks>` 中用到。

最后， ``-config-dir`` 参数指定服务以及健康检查 **配置** 所在的目录。

将以上命令行参数合在一起，便得到以下 ``consul agent`` 命令：

.. code-block:: shell-session

    vagrant@n1:~$ consul agent -server -bootstrap-expect=1 \
        -data-dir=/tmp/consul -node=agent-one -bind=172.20.20.10 \
        -enable-script-checks=true -config-dir=/etc/consul.d

接着，新开一个终端，登上另一个节点：

.. code-block:: shell-session

    $ vagrant ssh n2

这次，我们通过 ``-bind`` 参数绑定第二个节点的地址，并通过 ``-node`` 参数指定节点名为 ``agent-two`` 。
由于第二个节点不以 **服务器** 模式运行，我们去掉了 ``-server`` 参数。

组合起来，便形成以下 ``consul agent`` 命令：

.. code-block:: shell-session

    vagrant@n2:~$ consul agent -data-dir=/tmp/consul -node=agent-two \
        -bind=172.20.20.11 -enable-script-checks=true \
        -config-dir=/etc/consul.d

至此，我们成功运行了两个 `Consul`_ 节点：一个是 **服务器** 模式，一个是 **客户端** 模式。
然而，两个 **代理** 均不知对方的存在，都以为自己处于单节点集群。
运行 ``consul members`` 便可确认这一点，你会发现集群成员只有一个。

加入集群
========

现在，通知第一个 **代理** 加入第二个，先登录 ``n1`` 节点：

.. code-block:: shell-session

    $ vagrant ssh n1

可以看到，目前集群还只有一个成员：

.. code-block:: shell-session

    vagrant@n1:~$ consul members
    Node       Address            Status  Type    Build  Protocol  DC   Segment
    agent-one  172.20.20.10:8301  alive   server  1.4.1  2         dc1  <all>

执行以下命令，让当前代理加入第二个代理， `IP` 为 ``172.20.20.11`` ：

.. code-block:: shell-session

    vagrant@n1:~$ consul join 172.20.20.11
    Successfully joined cluster by contacting 1 nodes.

这时，两个代理都会输出一些日志，从中可以看到加入集群的信息。
再次查看集群成员，可以看到两个 **代理** 都知道对方的存在了：

.. code-block:: shell-session

    vagrant@n1:~$ consul members
    Node       Address            Status  Type    Build  Protocol  DC   Segment
    agent-one  172.20.20.10:8301  alive   server  1.4.1  2         dc1  <all>
    agent-two  172.20.20.11:8301  alive   client  1.4.1  2         dc1  <default>

.. note::

    **代理** 新加入 `Consul`_ 集群，只需知道其中某个节点的地址即可。
    成功加入后， **代理** 间可借助通讯获得完整的成员信息。

自动加入集群
============

理想状态下，新节点启动后应该 **自动加入** `Consul` **集群** ，无需人工干预。
借助 `AWS` 、 `谷歌云` 或 `Azure` 的 **实例发现** 功能，
`Consul`_ 只需配置一个指定的 **键值对** ( `key/value` )便能实现 **自动组群** ( `auto-join` )。
这个方案无需任何 **硬编码** ( `hardcode` )配置，即可让新节点自动加入已有集群。
另外，指定 ``-join`` 命令行参数， **硬编码** 其他节点地址，也是可行的。

查询节点
========

与查询服务类似， `Consul` 也提供 `API` 用于查询节点自身， `HTTP` 与 `DNS` 均可。

`DNS` 接口，名字结构为 ``NAME.node.consul`` 或者 ``NAME.node.DATACENTER.consul`` 。
如果 **数据中心** ( `datacenter` )部分省略， `Consul`_ 则搜索本地数据中心。

例如，我们可以在节点一( ``agent-one`` )上查询节点二( ``agent-two`` )的地址：

.. code-block:: shell-session

    vagrant@n1:~$ dig @127.0.0.1 -p 8600 agent-two.node.consul

    ;; QUESTION SECTION:
    ;agent-two.node.consul.        IN    A

    ;; ANSWER SECTION:
    agent-two.node.consul.    0    IN    A      172.20.20.11

除了服务查询，具备节点查询能力对 **系统管理** ( `system administration` )任务意义非凡。
例如，需要通过 `SSH` 登录某节点时，通过接口便可查到地址信息。

退出集群
========

通过 `Ctrl-C` 或者强杀 **代理** 进程即可使其退出集群。
将节点转至 **离开状态** ( `left state` )则 **更优雅** ，避免其他节点检测到异常。
两种方式的区别 `官网 <https://learn.hashicorp.com/consul/getting-started/agent.html#stopping-the-agent>`_ 上有详细讨论。

下一步
======

本节，我们成功部署了一个 **多节点** 的 `Consul`_  **集群** 。
为了使服务更加 **健壮** ，还需要配置 :doc:`健康检查机制 <checks>` ， 下节见。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Consul Cluster | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/join>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/
.. _Vagrant: asdf
.. _机器名: https://learn-linux.readthedocs.io/zh_CN/latest/administration/network/hostname.html

.. comments
    comment something out below

