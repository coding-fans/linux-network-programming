.. man
    FileName:   man.rst
    Author:     Fasion Chan
    Created:    2018-01-28 14:35:22
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

===
man
===

需要使用某个命令，却忘记了用法？
代码写了一半，想不起某个系统调用的用法？
很不幸，这种囧事几乎每天都有……

没事，`Google <https://www.google.com/>`_ 一下呗。

除此之外，还有更便捷的方式：运行 ``man`` 命令查看 `手册页 <https://zh.wikipedia.org/wiki/%E6%89%8B%E5%86%8C%E9%A1%B5>`_ ( ``Manual pages`` )。
手册页是 ``Linux/Unix`` 软件文档的一种常用形式，涵盖：命令、系统调用、库函数等方方面面。

以查看 ``socket`` 系统调用手册为例，命令行下运行：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ man socket

回车按下后，手册来了：

.. figure:: /_images/toolkit/man/5dfc3c544562c605f94c8949f87ff6b6.png

如上图，系统调用需要引入的 **头文件** ， **参数** 类似及含义， **返回值** 等等一目了然。

注意到， ``man`` 提供了 ``less`` 终端分页器，在文档较长时非常有用。
按 ``j`` 往下滚动，按 ``k`` 往上滚动；
按 ``<ctrl>-f`` 向下翻页，按 ``<ctrl>-b`` 向上翻页；
按 ``gg`` 回到开头，按 ``G`` 跳到结尾处。
``vim`` 用户应该非常熟悉！

如果忘记了 ``man`` 命令的用法，还可以用 ``man`` 查自己的手册：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ man man

章节
====

``Linux`` 下，手册通常分为 ``8`` 个章节( ``Section`` )，见表格-1：

.. csv-table:: 表格-1 手册页章节
    :header: "章节", "说明"
    :widths: 10 40

    "1", "一般命令"
    "2", "系统调用"
    "3", "库函数，涵盖C标准函数库"
    "4", "特殊文件(通常是/dev中的设备文件)和驱动程序"
    "5", "文件格式和约定"
    "6", "游戏和屏保"
    "7", "杂项"
    "8", "系统管理命令和守护进程"
    "9", "内核例程"

不同的章节，可能有同名的手册。
``printf`` 就是一例，章节 ``1`` 提供 ``printf`` 命令的手册；章节 ``3`` 提供 ``printf`` 库函数的手册。
这种情况下，如果直接运行 ``man printf`` ，显示的是命令 ``printf`` 的手册。
对于 ``C`` 库函数，需要指定章节：

.. code-block:: shell-session
    :linenos:

    fasion@ubuntu:~/lnp$ man 3 printf

.. comments
    comment something out blow

