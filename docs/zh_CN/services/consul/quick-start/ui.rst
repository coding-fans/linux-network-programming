.. Web UI
    FileName:   ui.rst
    Author:     Fasion Chan
    Created:    2019-01-24 18:19:49
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        Consul还提供了一个功能全面的WebUI，界面也非常友好。
        UI可以查看服务、节点信息，查看健康检查配置和状态，查询或者设置键值数据。
        UI自动支持多数据中心(multi-datacenter)环境。
    :keywords: consul, ui, web, CONSUL_UI_LEGACY, 界面

======
Web UI
======

`Consul`_ 还提供了一个功能全面的 `Web UI` ，界面也非常友好。
`UI` 可以查看 **服务** 、 **节点** 信息，查看 **健康检查** 配置和状态，
查询或者设置 **键值数据** 。
`UI` 自动支持 **多数据中心** ( `multi-datacenter` )环境。

**代理** 自带 `UI` 界面功能，指定 ``-ui`` 参数即可启用：

.. code-block:: shell-session

    $ consul agent -dev -ui

`UI` 服务端口与 `HTTP` 接口一样，路径为 ``/ui`` ，默认地址是 `http://localhost:8500/ui <http://localhost:8500/ui>`_ 。

`UI` 访问权限可以通过 `ACL` 进行控制，限制只对某些页面有 **读** 、 **写** 或 **更新** 权限。

另外，可以在 `这里 <http://demo.consul.io/>`_ 找到一个 `UI` 演示环境( `Demo` )。

历史版本
========

`Consul`_ `1.2.0` 版本之后，原来的 `UI` 界面废弃了。
但是，用户还是可以通过将环境变量 ``CONSUL_UI_LEGACY`` 设置为 ``true`` 来使用旧版 `UI` 。
如果未指定该环境变量，默认启用最新版 `UI` 。
想要使用最新版 `UI` ，将 ``CONSUL_UI_LEGACY`` 设置为 ``false`` 或者干脆就不提供该环境变量。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Web UI | Consul - HashiCorp Learn <https://learn.hashicorp.com/consul/getting-started/ui>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. _Consul: https://www.consul.io/

.. comments
    comment something out below

