.. Tomcat
    FileName:   tomcat.rst
    Author:     Fasion Chan
    Created:    2018-06-27 08:49:54
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :keywords: tomcat, 安装

======
Tomcat
======

安装
====

先准备好 `JDK` 包以及 `Tomcat` 二进制包，下载链接如下：

- `Oracle JDK 下载 <http://www.oracle.com/technetwork/java/javase/downloads/index.html>`_
- `Apache Tomcat 下载 <http://tomcat.apache.org/download-90.cgi>`_

接下来，我们将 `JDK` 部署在 ``/data/jdk`` 目录下；
将 `Tomcat` 部署在 ``/data/tomcat/clusters/default`` 目录下。

.. note::

    `Tomcat` 部署目录选择主要出于多实例考虑， `clusters` 目录下可以部署多个实例， `default` 是其中一个。

接下来，我们将 `JDK` 包解压至选定目录：

.. code-block:: shell-session

    $ mkdir /data/jdk
    $ tar -C /data/jdk -xf /media/home/Downloads/jdk-10.0.1_linux-x64_bin.tar.gz
    $ ls /data/jdk/
    jdk-10.0.1

跑一下 `java` 命令，确认安装正常无误：

.. code-block:: shell-session

    $ /data/jdk/jdk-10.0.1/bin/java --version
    java 10.0.1 2018-04-17
    Java(TM) SE Runtime Environment 18.3 (build 10.0.1+10)
    Java HotSpot(TM) 64-Bit Server VM 18.3 (build 10.0.1+10, mixed mode)

继续解压 `Tomcat` 二进制包至选定目录：

.. code-block:: shell-session

    $ mkdir -p /data/tomcat/clusters/default
    $ tar -C  /data/tomcat/clusters/default -xf apache-tomcat-9.0.8.tar.gz
    $ ls /data/tomcat/clusters/default
    apache-tomcat-9.0.8

进入 `Tomcat` 目录，编辑 ``bin/catalina.sh`` 文件：

.. code-block:: shell-session

    $ cd /data/tomcat/clusters/default/apache-tomcat-9.0.8
    $ vi bin/catalina.sh

在代码逻辑开始之前(一条非注释语句之前)，定义以下环境变量：

.. code-block:: shell

    export TOMCAT_HOME="/data/tomcat/clusters/default/apache-tomcat-9.0.8"
    export CATALINA_HOME="/data/tomcat/clusters/default/apache-tomcat-9.0.8"
    export JRE_HOME="/data/jdk/jdk-10.0.1"
    export JAVA_HOME="/data/jdk/jdk-10.0.1"

运行 `bin/startup.sh` 启动服务：

.. code-block:: shell-session

    $ bin/startup.sh
    Using CATALINA_BASE:   /data/tomcat/clusters/default/apache-tomcat-9.0.8
    Using CATALINA_HOME:   /data/tomcat/clusters/default/apache-tomcat-9.0.8
    Using CATALINA_TMPDIR: /data/tomcat/clusters/default/apache-tomcat-9.0.8/temp
    Using JRE_HOME:        /data/jdk/jdk-10.0.1
    Using CLASSPATH:       /data/tomcat/clusters/default/apache-tomcat-9.0.8/bin/bootstrap.jar:/data/tomcat/clusters/default/apache-tomcat-9.0.8/bin/tomcat-juli.jar
    Tomcat started.

确认 `Tomcat` 进程已经启动运行：

.. code-block:: shell-session

    $ ps aux | grep -v grep | grep tomcat
    fasion   24646  9.2 10.0 2395260 101156 pts/0  Sl   01:36   0:16 /data/jdk/jdk-10.0.1/bin/java -Djava.util.logging.config.file=/data/tomcat/clusters/default/apache-tomcat-9.0.8/conf/logging.properties -Djava.util.logging.manager=org.apache.juli.ClassLoaderLogManager -Djdk.tls.ephemeralDHKeySize=2048 -Djava.protocol.handler.pkgs=org.apache.catalina.webresources -Dorg.apache.catalina.security.SecurityListener.UMASK=0027 -Dignore.endorsed.dirs= -classpath /data/tomcat/clusters/default/apache-tomcat-9.0.8/bin/bootstrap.jar:/data/tomcat/clusters/default/apache-tomcat-9.0.8/bin/tomcat-juli.jar -Dcatalina.base=/data/tomcat/clusters/default/apache-tomcat-9.0.8 -Dcatalina.home=/data/tomcat/clusters/default/apache-tomcat-9.0.8 -Djava.io.tmpdir=/data/tomcat/clusters/default/apache-tomcat-9.0.8/temp org.apache.catalina.startup.Bootstrap start

最后，使用浏览器访问 `8080` 端口即可看到 `Tomcat` 默认页面：

.. figure:: /_images/services/tomcat/357b06b730ccb543ff3220f6fdae39a4.png
    :width: 508px

    *Tomcat 默认页面*

如果 `Tomcat` 部署在本地，则访问 `<http://localhost:8080>`_ 。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Apache Tomcat 9 (9.0.8) - Documentation Index <http://tomcat.apache.org/tomcat-9.0-doc/index.html>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

    :description lang=zh:

