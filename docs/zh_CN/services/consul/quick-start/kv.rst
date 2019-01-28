.. 键值数据
    FileName:   kv.rst
    Author:     Fasion Chan
    Created:    2019-01-24 18:15:19
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        除了服务发现和健康检查，Consul还提供一个易于使用的KV存储。
        这可以用来存储动态配置、辅助服务协调、构建领袖选举，几乎可以做任何开发人员能够想到的事情。
    :keywords: consul, kv store, dynamic configuration, KV存储, 键值存储, 动态配置

========
键值数据
========

除了 :doc:`服务发现 <services>` 和 :doc:`健康检查 <checks>` ，`Consul`_ 还提供一个易于使用的 `KV` 存储。
这可以用来存储动态配置、辅助服务协调、构建领袖选举等，甚至做任何开发人员能够想到的事情。

简明用法
========

`KV` 存储更容易入门，接下来我们创建几个键值来演示一下。
与 `Consul`_ **键值存储** 打交道有两种不同方式： `HTTP` 接口或者 **命令行接口** ( `CLI` )。
下面的例子使用 **命令行接口** 进行，因为这样更为方便。
更高级的集成，可以参考 `Consul KV HTTP API文档 <https://www.consul.io/api/kv.html>`_ 。

先 **查询** 给定 **键** ( `key` ) ``redis/config/minconns`` 对应的 **值** ( `value` )：

.. code-block:: shell-session

    $ consul kv get redis/config/minconns
    Error! No key exists at: redis/config/minconns

可以看到，我们没有得到任何结果，因为该键根本没有存储任何值。
接着，我们往存储 **插入** 一些键值：

.. code-block:: shell-session

    $ consul kv put redis/config/minconns 1
    Success! Data written to: redis/config/minconns

.. code-block:: shell-session

    $ consul kv put redis/config/maxconns 25
    Success! Data written to: redis/config/maxconns

.. code-block:: shell-session

    $ consul kv put -flags=42 redis/config/users/admin abcd1234
    Success! Data written to: redis/config/users/admin

现在存储已经有数据了，可以根据键将值 **查询** 出来：

.. code-block:: shell-session

    $ consul kv get redis/config/minconns
    1

`Consul`_ 也可以返回额外的 **元数据** ，指定 ``-detailed`` 参数即可：

.. code-block:: shell-session

    $ consul kv get -detailed redis/config/minconns
    CreateIndex      207
    Flags            0
    Key              redis/config/minconns
    LockIndex        0
    ModifyIndex      207
    Session          -
    Value            1

在 ``redis/config/users/admin`` 键上，我们存储了一个值为 `42` 的 **标识** ( `flag` )。
`Consul`_ 支持 `64` 位的标识值。
标识值不是 `Consul` 内部使用的，而是客户端用来为 **键值** 添加带语义的元数据。

通过 ``-recurse`` 参数，可以 **列举** 存储内所有的键，返回结果以字母排序：

.. code-block:: shell-session

    $ consul kv get -recurse
    redis/config/maxconns:25
    redis/config/minconns:1
    redis/config/users/admin:abcd1234

从存储中 **删除** 某个键，需要调用 ``delete`` 指令：

.. code-block:: shell-session

    $ consul kv delete redis/config/minconns
    Success! Deleted key: redis/config/minconns

**递归删除** 某个前缀下所有键也是支持的，同样是指定 ``-recurse`` 参数：

.. code-block:: shell-session

    $ consul kv delete -recurse redis
    Success! Deleted keys with prefix: redis

在同样的键上再次执行 ``put`` 指令即可 **更新** 对应的值：

.. code-block:: shell-session

    $ consul kv put foo bar

    $ consul kv get foo
    bar

    $ consul kv put foo zip

    $ consul kv get foo
    zip

`Consul`_ 也支持原子的 **检查并设置** ( `Check-And-Set` )型更新操作，指定 ``-cas`` 参数即可：

.. code-block:: shell-session

    $ consul kv put -cas -modify-index=123 foo bar
    Success! Data written to: foo

    $ consul kv put -cas -modify-index=123 foo bar
    Error! Did not write to foo: CAS failed

第一个更新操作成功因为记录索引刚好是 `123` ，第二次则失败因为索引不再是 `123` 了。

下一步
======

由于篇幅关系，我们只介绍了若干个操作接口，完整接口可以查看文档：

- `Consul KV HTTP API <https://www.consul.io/api/kv.html>`_
- `Consul KV CLI <https://www.consul.io/docs/commands/kv.html>`_

下一节，我们介绍： :doc:`ui` 。

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `KV Data | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/kv>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

