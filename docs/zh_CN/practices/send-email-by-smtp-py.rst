.. 使用SMTP协议发送邮件(Python语言)
    FileName:   send-email-by-smtp-py.rst
    Author:     Fasion Chan
    Created:    2018-05-23 16:55:15
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :keywords: smtp, smtplib, python, send email, 发送邮件

================================
使用SMTP协议发送邮件(Python语言)
================================

smtplib
=======

`Python` 标准库中， `smtplib <https://docs.python.org/3/library/smtplib.html>`_ 模块提供 `SMTP` 协议发送接口，使用起来十分方便。
接下来，写一个用于发送邮件的函数，一睹为快：

.. literalinclude:: /_src/python/smtp/send_email.py
    :caption:
    :name: c/send_ether/arguments
    :language: python
    :lines: 21-64
    :linenos:

函数通过参数指定了发送一封邮件所需要的全部要素，依次是：
邮件服务器地址、端口、登录用户、登录密码、发送人邮箱(一般与登录用户相同)、收件人邮箱，邮件标题，邮件内容。

函数第 `21` 行先初始化一个连接对象。
注意到，类连接类通过 ``smtp_cls`` 参数化，
灵活支持不同连接类，如 ``smtp.SMTP`` 、 ``smtp.SMTP_SSL`` 等。

第 `24` 行用指定的地址端口连接邮件服务器。
紧接着，第 `25` 行用指定的用户密码进行登录。
至此，程序编写已经成功了一半。

第 `28` - `40` 行组装消息体，消息体包括：发件人，收件人，邮件标题，发送时间，邮件内容等要素。
消息体的格式如下：

.. code-block:: text

    From: xxxx@qq.com
    To: yyyy@qq.com
    Subject: hello world
    Date: 23/5/2018 20:35

    first email sent by python

空行之后就是邮件内容，可以包括多行。

第 `43` 行将消息体发送到邮件服务器。这样，一封邮件就成功发送出去了，是不是很简单？

SMTP服务器
==========

在调用 ``send_email`` 函数发送邮件前，需要获得邮件服务器的登录信息。
登录信息包括四要素，分别是：服务器地址，端口，登录用户以及登录密码。

很不幸，不同的邮件厂商，做法颇有不同。
下面以 `QQ邮箱 <https://mail.qq.com/>`_ 为例，介绍如何获得登录信息：

登录 `QQ邮箱 <https://mail.qq.com/>`_ ，点击 **设置** > **账号** ，
找到 **POP3/IMAP/SMTP/Exchange/CardDAV/CalDAV服务** ，
按提示开启 `SMTP` 并取得授权码。

.. figure:: /_images/practices/1c50f1e9f0bf98f7f495b14377869f71.png
    :width: 342px

    *授权码*

点击 `如何使用 Foxmail 等软件收发邮件？ <http://service.mail.qq.com/cgi-bin/help?subtype=1&&id=28&&no=371>`_ ，
指引里可以找到 `SMTP` 服务器地址以及端口信息：

.. figure:: /_images/practices/75d2afbd89bb1516a5f6f12cfda33bad.png
    :width: 342px

    *地址端口*

至此，四要素集齐：服务器地址一般为 ``smtp.qq.com`` ，端口一般为 ``465`` ，
登录用户就邮箱地址，密码就是授权码。

.. warning::
    注意，SMTP服务器登录密码是生成的授权码，不是邮箱登录密码！

下一步
======

小手动起来，使用 `Python` 发出你的第一封邮件吧！

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `smtplib — SMTP protocol client <https://docs.python.org/3/library/smtplib.html>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

