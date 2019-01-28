.. 快速开始
    FileName:   index.rst
    Author:     Fasion Chan
    Created:    2019-01-21 19:38:30
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
    :keywords:

========
快速开始
========

`Consul`_ 是一个 **微服务管理** 解决方案，提供 **服务发现** 、 **配置管理** 、 以及 **服务划分** 等全特性功能。
通过本系列学习，你可在一天之内掌握 `Consul`_ ！

.. toctree::
    :titlesonly:

    安装Consul <install>
    运行代理 <agent>
    注册服务 <services>
    互连服务 <connect>
    Consul集群 <join>
    健康检查 <checks>
    键值数据 <kv>
    Web UI <ui>

简要
====

#. :doc:`install` 。
   `Consul`_ 集群每个节点均需要安装 `Consul`_ `代理` ，本节介绍安装方法。
#. :doc:`agent` 。
   `Consul`_ **代理** ( `agent` )可运行于 **服务器** ( `server` )模式或 **客户端** ( `client` )模式。
   本节讨论这两种模式，并演示如何以 **开发** ( `development` )模式启动 **代理** 。
#. :doc:`services` 。
   往 `Consul`_ 注册服务，提供服务配置或者调用 `HTTP` 接口均可。
   本节演示通过配置文件注册服务。
#. :doc:`connect` 。
   `Consul`_ **互连** ( `Connect` )提供服务间连接 **鉴权** 及 **加密** ，基于 `TLS` 。
#. :doc:`join` 。本节通过 `Vagrant` 虚拟机演示组件双节点集群。
#. :doc:`checks` 。本节演示为 **节点** ( `node` )和 **服务** ( `service` )配置 **健康检查** 。
#. :doc:`kv` 。本节演示使用 **命令行接口** ( `CLI` )添加、管理 **键值** ( `key-value` )数据。
#. :doc:`ui` 。 `Consul`_ 自带 `Web` 界面，功能全面，操作友好，本节我们一起来探索。


.. _Consul: https://www.consul.io/

.. comments
    comment something out below

