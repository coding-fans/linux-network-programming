.. 用Go语言写RESTful API
    FileName:   go-restful-api.rst
    Author:     Fasion Chan
    Created:    2019-03-12 14:35:45
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        本文先介绍一些RESTful理念，并通过一个KVS服务演示RESTfulAPI的行为。
        最后以KVS服务实作为例，介绍如何使用go-restful框架编写RESTfulAPI。
    :keywords: restful, go, go-restful, api, golang, Go语言

=====================
用Go语言写RESTful API
=====================

本文先介绍一些 :ref:`restful-philosophy` ，
并通过一个 :ref:`kvs-service` 演示 `RESTful API` 的行为。
最后以 :ref:`kvs-implementation` 为例，
介绍如何使用 `go-restful` 框架编写 `RESTful API` 。

.. _restful-philosophy:

RESTful 理念
============

`REST <https://en.wikipedia.org/wiki/Representational_state_transfer>`_ 是
`Representational State Transfer <https://en.wikipedia.org/wiki/Representational_state_transfer>`_ 的缩写，
中文翻译是 `表现层状态转换 <https://zh.wikipedia.org/wiki/%E8%A1%A8%E7%8E%B0%E5%B1%82%E7%8A%B6%E6%80%81%E8%BD%AC%E6%8D%A2>`_ 。
这种软件构建风格，通过基于 `HTTP` 之上的一组 **约束** 和 **属性** ，提供万维网网络服务。
其主要特点包括：

- **统一接口** ( `Uniform Interface` )，资源通过一致、可预见的 `URI` 及请求方法来操作；
- **无状态** ( `Stateless` )，请求状态由客户端维护，服务端不做保存;
- **可缓存** ( `Cacheable` )，可以通过缓存提升服务性能；
- **分层系统** ( `Layered System` )

**统一接口** 是 `RESTful` 服务最大的特点。
统一接口的核心思想是，将服务抽象成各种 **资源** ，
并通过一套一致、可预见的 `URI` 以及 **请求方法** ( `Request Method` )来操作这些资源。
这样一来，掌握了一种资源的使用方法，便可延伸到其他资源上，达到举一反三的效果。

资源可以是 **单个资源** ，也可以是同种类资源组成的集合，即 **资源组** 。
资源、资源组以及对应的操作定义如下：

.. csv-table:: 表格-1 RESTful 统一接口
    :header: "资源", "GET", "PUT", "POST", "DELETE"

    "一组资源，URI形如：https://example.com/resources/", "列出每个资源及其详细信息(可选)", "以给定资源组替换当前资源组(较少用)", "创建资源并追加到资源组", "删除整个资源组(较少用)"
    "单个资源，URI形如：https://example.com/resources/142", "获取指定资源的详细信息", "替换或创建指定资源", "创建资源并作为子资源添加到当前资源组", "删除指定的资源"

注意到，创建资源有两种不同的方式，即：

#. 针对资源组的 `POST` 操作；
#. 针对资源 `PUT` 操作；

这两种操作是有区别的，主要体现在 **幂等性** 上。

针对资源组的 `POST` 操作，数据键(或 `ID` )一般由服务端分配。
因此，同个请求执行两次将创建两个相同的资源。
换句话讲，这个操作一般 **不具有幂等性** 。

针对资源 `PUT` 操作，由于客户端显式指定数据键，幂等性是可以保证的。
实际上，这个操作在资源已经存在的情况下替换原有资源，在资源还未存在的情况下创建资源。


最后，我们还可以将 `RESTful` 与传统的 **增删改查** 一一对应起来：

.. csv-table:: 表格-2 RESTful 增删改查
    :header: "操作", "对象", "URI", "请求方法"

    "创建资源( **增** )", "资源组", "/resources", "POST"
    "删除资源( **删** )", "资源", "/resources/142", "DELETE"
    "更新资源( **改** )", "资源", "/resources/142", "PUT"
    "查询资源( **查** )", "资源", "/resources/142", "GET"
    "列举资源( **列** )", "资源组", "/resources", "GET"

.. _kvs-service:

KVS 服务
========

为了演示 `RESTful` 服务接口的特性，我们特地实现了一个简单的服务—— `KVS` 服务。

该服务只提供一种资源，名为 ``kv`` 。
根据资源名， ``kv`` 资源组的 `URI` 是 ``/kvs`` ，
``kv`` 资源的 `URI` 是 ``/kvs/<key>`` ， ``key`` 是资源的键。

进入源码目录 `go/restful/go-restful/kvs <https://github.com/fasionchan/linux-network-programming/tree/master/src/go/restful/go-restful/kvs>`_ ，
可以看到源码文件 `kvs.go <https://github.com/fasionchan/linux-network-programming/blob/master/src/go/restful/go-restful/kvs/kvs.go>`_ ：

.. code-block:: shell-session

    $ cd go/restful/go-restful/kvs
    $ ls
    kvs.go

在 `Go` 开发环境下，可直接启动 `KVS` 服务了：

.. code-block:: shell-session

    $ go run kvs.go

服务默认监听本地 `8080` 端口： `<http://127.0.0.1:8080/>`_ 。

服务启动后，资源池是空的，不信发起搜索请求看看：

.. code-block:: shell-session

    $ curl http://127.0.0.1:8080/kvs
    {
     "result": true,
     "data": [],
     "meta": {
      "total": 0,
      "skip": 0,
      "count": 0
     },
     "message": ""
    }

注意到，返回的数据中， `data` 字段是一个空的列表。
换句话讲，资源池中没有任何可用的键。

接着，我们随便用一个键(如 ``something`` )向服务器检索数据。
毫不意外，服务器返回一个错误，告诉我们资源不存在：

.. code-block:: shell-session

    $ curl http://127.0.0.1:8080/kvs/something
    {
     "result": false,
     "data": null,
     "meta": null,
     "message": "resource not exists"
    }

好吧，那我们创建一个资源呗：

.. code-block:: shell-session

    $ curl -X POST \
        -H 'Content-Type: application/json' \
        -d '{"key": "guangzhou", "name": "广州", "population": 14498400}' \
        http://127.0.0.1:8080/kvs
    {
     "result": true,
     "data": {
      "key": "guangzhou",
      "name": "广州",
      "population": 14498400
     },
     "meta": null,
     "message": ""
    }

我们新增了一条广州的人口数据，数据键为 ``guangzhou`` 。
注意到，我们使用 ``POST`` 方法，并通过 ``Content-Type`` 头部指定数据类型为 ``json`` 。

我们再接再厉，添加杭州的数据：

.. code-block:: shell-session

    $ curl -X POST \
        -H 'Content-Type: application/json' \
        -d '{"key": "hangzhou", "name": "杭州", "population": 946800}' \
        http://127.0.0.1:8080/kvs
    {
     "result": true,
     "data": {
      "key": "hangzhou",
      "name": "杭州",
      "population": 946800
     },
     "meta": null,
     "message": ""
    }

艾玛呀，人口少了个零咋整？——更新呗：

.. code-block:: shell-session

    $ curl -X PUT \
        -H 'Content-Type: application/json' \
        -d '{"population": 9468000}' \
        http://127.0.0.1:8080/kvs/hangzhou
    {
     "result": true,
     "data": {
      "key": "hangzhou",
      "name": "杭州",
      "population": 9468000
     },
     "meta": null,
     "message": ""
    }

注意到，这里我们采用 ``PUT`` 方法，数据只包括需要修正的人口字段( ``population`` )。

好了，现在在资源池可以查询到这两个数据记录了：

.. code-block:: shell-session

    $ curl http://127.0.0.1:8080/kvs
    {
     "result": true,
     "data": [
      {
       "key": "guangzhou",
       "name": "广州",
       "population": 14498400
      },
      {
       "key": "hangzhou",
       "name": "杭州",
       "population": 9468000
      }
     ],
     "meta": {
      "total": 2,
      "skip": 0,
      "count": 2
     },
     "message": ""
    }

根据数据键，我们可以查询出对应的数据记录：

.. code-block:: shell-session

    $ curl http://localhost:8080/kvs/guangzhou
    {
     "result": true,
     "data": {
      "key": "guangzhou",
      "name": "广州",
      "population": 14498400
     },
     "meta": null,
     "message": ""
    }

数据记录不断增长，当存储资源不足时，服务将返回一个错误：

.. code-block:: shell-session

    $ curl -X POST \
        -H 'Content-Type: application/json' \
        -d '{"key": "suzhou", "name": "苏州", "population": 10684000}' \
        http://127.0.0.1:8080/kvs
    {
     "result": false,
     "data": null,
     "meta": null,
     "message": "out of resources"
    }

.. _kvs-implementation:

KVS 服务实作
============

`KVS` 是一个非常简单的服务，对于有一些编程基础的童鞋，理解起来应该毫无难度。
完整源码在这查看： `kvs.go`_ 。

先定义数据结构体，通过标签定义用于 *json* 序列化的字段名：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :name: go/restful/go-restful/kvs/kvs.go/struct
    :language: go
    :lines: 28-45
    :linenos:

*DataItem* 是存储 **城市信息** 的结构体；
*ResultMeta* 是存储 **结果元数据** 的结构体；
而 *Result* 则是存储 **请求结果** 的结构体。

在 `go-restful`_ 框架中，服务定义由 *restful.WebService* 结构维护，初始化示例如下：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :name: go/restful/go-restful/kvs/kvs.go/service
    :language: go
    :lines: 47-61
    :linenos:

如上述代码，构建 *restful.WebService* 结构，需指定 **路径** ( `path` )以及 **路由规则** 。
路由规则包括 *3* 个组成部分， **路径** ( `path` )、 **方法** ( `method` )以及 **处理函数** 。

处理函数有两个参数： **请求对象** ( *restful.Request* )以及 **响应对象** ( *restful.Response* )。
其中，请求对象用于获取请求信息；响应对象用于返回数据。

增
--

创建资源需在资源组 `URI` ( ``/kvs`` )之上实现 `POST` 方法处理函数：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :name: go/restful/go-restful/kvs/kvs.go/create
    :language: go
    :lines: 62-109
    :linenos:

创建资源的处理逻辑如下：

#. 检查资源使用情况，在资源不足时返回错误；
#. 获取请求数据，即反序列化请求数据；
#. 检查数据，当数据不符合要求时下返回错误；
#. 检查资源，在资源已存在时返回错误；
#. 创建资源并加入资源组；

注意到，我们通过在请求数据中包含数据键( ``key`` 字段 )来保证该操作的 **幂等性** 。

删
--

删除资源需要为资源 `URI` 实现 `DELETE` 方法处理函数。
资源 `URI` 形如 ``/kvs/<key>`` ，其中 ``kvs`` 是资源名， ``<key>`` 是用于唯一标识一个资源的键(或者 `ID` )。

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :caption:
    :name: go/restful/go-restful/kvs/kvs.go/delete
    :language: go
    :lines: 111-130
    :linenos:

删除资源前，先检查资源是否存在。
当资源不存在时，直接返回错误；当资源存在时，删除该资源并将其返回。

改
--

更新资源需要为资源 `URI` 实现 `PUT` 方法处理函数：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :caption:
    :name: go/restful/go-restful/kvs/kvs.go/update
    :language: go
    :lines: 132-159
    :linenos:

更新方式由两种， **部分更新** 以及 **整体替换** ，该例子实现的是前者。

查
--

查询操作需要为资源 `URI` 实现 `GET` 方法处理函数：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :caption:
    :name: go/restful/go-restful/kvs/kvs.go/retrieve
    :language: go
    :lines: 161-177
    :linenos:

列
--

列举给定资源组内的资源，需要为资源组 `URI` 实现 `GET` 方法：

.. literalinclude:: /_src/go/restful/go-restful/kvs/kvs.go
    :caption:
    :name: go/restful/go-restful/kvs/kvs.go/search
    :language: go
    :lines: 179-232
    :linenos:

可以通过 `URI` 参数指定列举条件，则只返回符合条件的资源。
此外，还可以为列举操作实现分页逻辑，这在资源数很多的情况下非常有用。

注意到，我们在返回数据中，用一个名为 ``meta`` 字段存放分页信息。
分页信息包括三部分：总资源数( ``total`` )、当前页资源数( ``count`` )以及当前页起点( ``skip`` )。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Welcome to Flask — Flask 1.0.2 documentation <http://flask.pocoo.org/docs/1.0/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _go-restful: https://github.com/emicklei/go-restful
.. _kvs.go: https://github.com/fasionchan/linux-network-programming/blob/master/src/go/restful/go-restful/kvs/kvs.go

.. comments
    comment something out below
