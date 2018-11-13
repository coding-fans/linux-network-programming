.. 用 Flask 框架写 RESTful API
    FileName:   restful-api-by-flask.rst
    Author:     Fasion Chan
    Created:    2018-06-13 10:40:19
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :keywords: restful, flask, python

===========================
用 Flask 框架写 RESTful API
===========================

本文先介绍一些 :ref:`restful-philosophy` ，
并通过一个 :ref:`kvs-service` 演示 `RESTful API` 的行为。
最后以 :ref:`kvs-implementation` 为例，
介绍如何使用 `Flask` 框架编写 `RESTful API` 。

.. _restful-philosophy:

RESTful 理念
============

`REST <https://en.wikipedia.org/wiki/Representational_state_transfer>`_ 是
`Representational State Transfer <https://en.wikipedia.org/wiki/Representational_state_transfer>`_ 的缩写，
中文翻译是 `表现层状态转换 <https://zh.wikipedia.org/wiki/%E8%A1%A8%E7%8E%B0%E5%B1%82%E7%8A%B6%E6%80%81%E8%BD%AC%E6%8D%A2>`_ 。
这种软件构建风格，通过基于 `HTTP` 之上的一组 **约束** 和 **属性** ，提供万维网网络服务。
其主要特点包括：

- **统一接口** ( `Uniform Interface` )，资源通过一致、可预见的 `URI` 以及请求方法来操作；
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
根据资源名， ``kv`` 资源组的 `URI` 是 ``/kvs`` ， ``kv`` 资源的 `URI` 是 ``/kvs/<key>`` ， ``key`` 是资源的键。

进入源码目录 `python/restful/flask/kvs <https://github.com/fasionchan/linux-network-programming/tree/master/python/restful/flask/kvs>`_ ，
可以看到两个文件：

.. code-block:: shell-session

    $ cd python/restful/flask/kvs
    $ ls
    kvs.py  requirements.txt

其中， `kvs.py`_ 是源码文件，
`requirements.txt`_ 是 `Python` 依赖。

如果你的 `Python` 执行环境还未安装 `Flask` 框架，请使用 `pip` 命令安装：

.. code-block:: shell-session

    $ pip install -r requirements.txt

依赖安装完毕后，就可以启动 `KVS` 服务了：

.. code-block:: shell-session

    $ FLASK_APP=kvs.py flask run
    * Serving Flask app "kvs.py"
    * Environment: production
      WARNING: Do not use the development server in a production environment.
      Use a production WSGI server instead.
    * Debug mode: off
    * Running on http://127.0.0.1:5000/ (Press CTRL+C to quit)

服务默认监听本地 `5000` 端口： `<http://127.0.0.1:5000/>`_ 。

可以给 `flask` 指定监听地址和端口：

.. code-block:: shell-session

    $ FLASK_APP=kvs.py flask run --host 0.0.0.0 --port 80

服务启动后，资源池是空的，不信发起搜索请求看看：

.. code-block:: shell-session

    $ curl http://127.0.0.1:5000/kvs
    {"data":[],"meta":{"count":0,"skip":0,"total":0},"result":true}

注意到，返回的数据中， `data` 字段是一个空的列表。
换句话讲，资源池中没有任何可用的键。

接着，我们随便用一个键(如 ``something`` )向服务器检索数据。
毫不意外，服务器返回一个错误，告诉我们资源不存在：

.. code-block:: shell-session

    $ curl http://127.0.0.1:5000/kvs/something
    {"message":"resource not exists","result":false}

好吧，那我们创建一个资源呗：

.. code-block:: shell-session

    $ curl -X post \
        -H 'Content-Type: application/json' \
        -d '{"key": "guangzhou", "name": "广州", "population": 14498400}' \
        http://127.0.0.1:5000/kvs
    {"data":{"key":"guangzhou","name":"广州","population":14498400},"result":true}

我们新增了一条广州的人口数据，数据键为 ``guangzhou`` 。
注意到，我们使用 ``POST`` 方法，并通过 ``Content-Type`` 头部指定数据类型为 ``json`` 。

我们再接再厉，添加杭州的数据：

.. code-block:: shell-session

    $ curl -X post \
        -H 'Content-Type: application/json' \
        -d '{"key": "hangzhou", "name": "杭州", "population": 946800}' \
        http://127.0.0.1:5000/kvs
    {"data":{"key":"hangzhou","name":"杭州","population":946800},"result":true}

艾玛呀，人口少了个零咋整？——更新呗：

.. code-block:: shell-session

    $ curl -X put \
        -H 'Content-Type: application/json' \
        -d '{"population": 9468000}' \
        http://127.0.0.1:5000/kvs/hangzhou
    {"data":{"key":"hangzhou","name":"杭州","population":9468000},"result":true}

注意到，这里我们采用 ``PUT`` 方法，数据只包括需要修正的人口字段( ``population`` )。

好了，现在在资源池可以查询到这两个数据记录了：

.. code-block:: shell-session

    $ curl http://127.0.0.1:5000/kvs
    {"data":[{"key":"guangzhou","name":"广州","population":14498400},{"key":"hangzhou","name":"杭州","population":9468000}],"meta":{"count":2,"skip":0,"total":2},"result":true}

根据数据键，我们可以查询出对应的数据记录：

.. code-block:: shell-session

    $ curl http://localhost:5000/kvs/guangzhou
    {"data":{"key":"guangzhou","name":"广州","population":14498400},"result":true}

数据记录不断增长，当存储资源不足时，服务将返回一个错误：

.. code-block:: shell-session

    $ curl -X post \
        -H 'Content-Type: application/json' \
        -d '{"key": "suzhou", "name": "苏州", "population": 10684000}' \
        http://127.0.0.1:5000/kvs
    {"message":"out of resources","result":false}

.. _kvs-implementation:

KVS 服务实作
============

`KVS` 是一个非常简单的服务，对于有一些编程基础的童鞋，理解起来应该毫无难度。
完整源码在这查看： `kvs.py`_ 。

增
--

创建资源需在资源组 `URI` ( ``/kvs`` )之上实现 `POST` 方法：

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/create
    :language: python
    :lines: 56-100
    :linenos:

创建资源的处理逻辑如下：

#. 检查资源使用情况，在资源不足时返回错误；
#. 获取请求数据，即反序列化请求数据；
#. 检查数据，当数据不符合要求时下返回错误；
#. 检查资源，在资源已存在时返回错误；
#. 创建资源并加入资源组；

注意到，我们通过在请求数据中包含数据键( ``key`` 字段 )来保证该操作的幂等性。

**获取请求数据** 的逻辑在其他接口也会用到，因此需要对其进行封装，以便代码复用。
为此，我们实现了一个名为 ``get_request_data`` 的函数：

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/get_request_data
    :language: python
    :lines: 34-53
    :linenos:

该函数先检查 `Content-Type` **头部** ，然后根据头部指定数据类型，对 **请求体** ( `Request Body` )进行 **反序列化** 。

删
--

删除资源需要为资源 `URI` 实现 `DELETE` 方法。
资源 `URI` 形如 ``/kvs/<key>`` ，其中 ``kvs`` 是资源名， ``<key>`` 是用于唯一标识一个资源的键(或者 `ID` )。

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/delete
    :language: python
    :lines: 103-122
    :linenos:

删除资源前，先检查资源是否存在。
当资源不存在时，直接返回错误；当资源存在时，删除该资源并将其返回。

改
--

更新资源需要为资源 `URI` 实现 `PUT` 方法：

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/update
    :language: python
    :lines: 125-157
    :linenos:

更新方式由两种， **部分更新** 以及 **整体替换** ，该例子实现的是前者。

查
--

查询操作需要为资源 `URI` 实现 `GET` 方法：

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/retrieve
    :language: python
    :lines: 160-179
    :linenos:

列
--

列举给定资源组内的资源，需要为资源组 `URI` 实现 `GET` 方法：

.. literalinclude:: /_src/python/restful/flask/kvs/kvs.py
    :caption:
    :name: python/restful/flask/kvs/kvs/search
    :language: python
    :lines: 182-
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

.. _kvs: https://github.com/fasionchan/linux-network-programming/tree/master/python/restful/flask/kvs
.. _kvs.py: https://github.com/fasionchan/linux-network-programming/blob/master/python/restful/flask/kvs/kvs.py
.. _requirements.txt: https://github.com/fasionchan/linux-network-programming/blob/master/python/restful/flask/kvs/requirements.txt

.. comments
    comment something out below

