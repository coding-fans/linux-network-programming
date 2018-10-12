.. 深入理解Linux TCP backlog
    FileName:   linux-tcp-backlog.rst
    Author:     Fasion Chan
    Created:    2018-10-12 19:13:15
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

.. meta::
    :description lang=zh:
        当应用程序调用listen系统调用让一个socket进入LISTEN状态时，需要指定一个参数：backlog。
        这个参数经常被描述为，新连接队列的长度限制。
        由于TCP建立连接需要进行3次握手，一个新连接在到达ESTABLISHED状态可以被accept系统调用返回给应用程序前，
        必须经过一个中间状态SYN RECEIVED(见上图)。
        这意味着，TCP/IP协议栈在实现backlog队列时，有两种不同的选择。
    :keywords: linux tcp backlog, tcp, backlog, linux

=========================
深入理解Linux TCP backlog
=========================

.. note::

    本文译自：`How TCP backlog works in Linux <http://veithen.github.io/2014/01/01/how-tcp-backlog-works-in-linux.html>`_ 。

当应用程序调用 `listen` 系统调用让一个 `socket` 进入 `LISTEN` 状态时，需要指定一个参数： `backlog` 。
这个参数经常被描述为，新连接队列的长度限制。

.. figure:: /_images/translations/linux-tcp-backlog/ba9dbb181b283f5c03d650146c8ab15c.png
    :width: 480px

由于 `TCP` 建立连接需要进行3次握手，一个新连接在到达 `ESTABLISHED` 状态可以被 `accept` 系统调用返回给应用程序前，
必须经过一个中间状态 `SYN RECEIVED` (见上图)。
这意味着， `TCP/IP` 协议栈在实现 `backlog` 队列时，有两种不同的选择：

#. 仅使用一个队列，队列规模由 `listen` 系统调用 `backlog` 参数指定。
   当协议栈收到一个 `SYN` 包时，响应 `SYN/ACK` 包并且将连接加进该队列。
   当相应的 `ACK` 响应包收到后，连接变为 `ESTABLISHED` 状态，可以向应用程序返回。
   这意味着队列里的连接可以有两种不同的状态： `SEND RECEIVED` 和 `ESTABLISHED` 。
   只有后一种连接才能被 `accept` 系统调用返回给应用程序。
#. 使用两个队列—— `SYN` 队列(待完成连接队列)和 `accept` 队列(已完成连接队列)。
   状态为 `SYN RECEIVED` 的连接进入 `SYN` 队列，
   后续当状态变更为 `ESTABLISHED` 时移到 `accept` 队列(即收到3次握手中最后一个 `ACK` 包)。
   顾名思义， `accept` 系统调用就只是简单地从 `accept` 队列消费新连接。
   在这种情况下， `listen` 系统调用 `backlog` 参数决定 `accept` 队列的最大规模。

历史上，起源于 `BSD` 的 `TCP` 实现使用第一种方法。
这个方案意味着，当 `backlog` 限制达到，系统将停止对 `SYN` 包响应 `SYN/ACK` 包。
通常，协议栈只是丢弃 `SYN` 包(而不是回一个 `RST` 包)以便客户端可以重试(而不是异常退出)。

`TCP/IP详解 卷3` 第 `14.5` 节中有提到这一点。
书中作者提到， `BSD` 实现虽然使用了两个独立的队列，但是行为跟使用一个队列并没什么区别。

在 `Linux` 上，情况有所不同，情况 `listen` 系统调用 `man` 文档页：

    `The  behavior  of  the  backlog  argument on TCP sockets changed with Linux 2.2.
    Now it specifies the queue length for completely established sockets waiting to be accepted, instead of the number of incomplete connection requests.
    The maximum length of the queue for incomplete sockets can be set using /proc/sys/net/ipv4/tcp_max_syn_backlog.
    When syncookies are enabled there is no logical maximum length and this setting is ignored.`

意思是， `backlog` 参数的行为在 `Linux` 2.2之后有所改变。
现在，它指定了等待 `accept` 系统调用的已建立连接队列的长度，而不是待完成连接请求数。
待完成连接队列长度由 `/proc/sys/net/ipv4/tcp_max_syn_backlog` 指定；
在 `syncookies` 启用的情况下，逻辑上没有最大值限制，这个设置便被忽略。

也就是说，当前版本的 `Linux` 实现了第二种方案，使用两个队列：

- `SYN` 队列，长度由系统统一指定；
- `accept` 队列，长度由应用程序单独指定。

现在，一个需要考虑的问题是在 `accept` 队列已满，
而一个已完成新连接需要由 `SYN` 队列移动到 `accept` 队列(收到3次握手中最后一个 `ACK` 包)，这个实现方案是什么行为。
这种情况下，由 `net/ipv4/tcp_minisocks.c` 中 `tcp_check_req` 函数处理：

.. code-block:: c

    child = inet_csk(sk)->icsk_af_ops->syn_recv_sock(sk, skb, req, NULL);
    if (child == NULL)
        goto listen_overflow;

对于 `IPv4` ，第一行代码实际上调用的是 `net/ipv4/tcp_ipv4.c` 中的 `tcp_v4_syn_recv_sock` 函数，代码如下：

.. code-block:: c

    if (sk_acceptq_is_full(sk))
        goto exit_overflow;

可以看到，这里会检查 `accept` 队列的长度。
如果队列已满，跳到 `exit_overflow` 标签执行一些清理工作、更新 `/proc/net/netstat` 中的统计项 `ListenOverflows` 和 `ListenDrops` ，最后返回 `NULL` 。
这会触发 `tcp_check_req` 函数跳到 `listen_overflow` 标签执行代码。

.. code-block:: c

    listen_overflow:
        if (!sysctl_tcp_abort_on_overflow) {
            inet_rsk(req)->acked = 1;
            return NULL;
        }

很显然，除非 `/proc/sys/net/ipv4/tcp_abort_on_overflow` 被设置为 `1` (这种情况下发送一个 `RST` 包)，实现什么都没做。

总结一下： `Linux` 内核协议栈在收到3次握手最后一个 `ACK` 包，确认一个新连接已完成。
在 `accept` 队列已满的情况下，会忽略这个包。
一开始您可能会对此感到奇怪——别忘了 `SYN RECEIVED` 状态下有一个计时器实现：
如果 `ACK` 包没有收到(或者是我们讨论的忽略)，协议栈会重发 `SYN/ACK` 包(重试次数由 `/proc/sys/net/ipv4/tcp_synack_retries` 决定)。

看以下抓包结果就非常明显——一个客户正尝试连接一个已经达到其最大 `backlog` 的 `socket` ：

.. code-block:: text

      0.000  127.0.0.1 -> 127.0.0.1  TCP 74 53302 > 9999 [SYN] Seq=0 Len=0
      0.000  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
      0.000  127.0.0.1 -> 127.0.0.1  TCP 66 53302 > 9999 [ACK] Seq=1 Ack=1 Len=0
      0.000  127.0.0.1 -> 127.0.0.1  TCP 71 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      0.207  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      0.623  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      1.199  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
      1.199  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 6#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
      1.455  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      3.123  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      3.399  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
      3.399  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 10#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
      6.459  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
      7.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
      7.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 13#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
     13.131  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
     15.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
     15.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 16#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
     26.491  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
     31.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
     31.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 19#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
     53.179  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
    106.491  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
    106.491  127.0.0.1 -> 127.0.0.1  TCP 54 9999 > 53302 [RST] Seq=1 Len=0

由于客户端的 `TCP` 实现在收到多个 `SYN/ACK` 包时，认为 `ACK` 包已经丢失了并且重传它。
如果在 `SYN/ACK` 重试次数达到限制前，服务端应用从 `accept` 队列接收连接，使得 `backlog` 减少，
那么协议栈会处理这些重传的 `ACK` 包，将连接状态从 `SYN RECEIVED` 变更到 `ESTABLISHED` 并且将其加入 `accept` 队列。
否则，正如以上包跟踪所示，客户端会收到一个 `RST` 包宣告连接失败。

在客户端看来，第一次收到 `SYN/ACK` 包之后，连接就会进入 `ESTABLISHED` 状态。
如果这时客户端首先开始发送数据，那么数据也会被重传。
好在 `TCP` 有慢启动机制，在服务端还没进入 `ESTABLISHED` 之前，客户端能发送的数据非常有限。

相反，如果客户端一开始就在等待服务端，而服务端 `backlog` 没能减少，
那么最后的结果是连接在客户端看来是 `ESTABLISHED` 状态，但在服务端看来是 `CLOSED` 状态。
这也就是所谓的半开连接。

有一点还没讨论的是： `listen` 系统调用 `man` 手册页中提到每次收到新 `SYN` 包，内核往 `SYN` 队列追加一个新连接(除非该队列已满)。
事实并非如此， `net/ipv4/tcp_ipv4.c`中`tcp_v4_conn_request` 函数负责处理 `SYN` 包，请看以下代码：

.. code-block:: c

    if (sk_acceptq_is_full(sk) && inet_csk_reqsk_queue_young(sk) > 1) {
        NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_LISTENOVERFLOWS);
        goto drop;
    }

可以看到，在 `accept` 队列已满的情况下，内核会强制限制 `SYN` 包的接收速率。
如果有大量 `SYN` 包待处理，它们其中的一些会被丢弃。
这样看来，就完全依靠客户端重传 `SYN` 包了，这种行为跟 `BSD` 实现一样。

下结论前，需要再研究以下 `Linux` 这种实现方式跟 `BSD` 相比有什么优势。
`Stevens` 是这样说的：

    在 `accept` 队列已满或者 `SYN` 队列已满的情况下， `backlog` 会达到限制。
    第一种情况经常发生在服务器或者服务器进程非常繁忙的情况下，进程没法足够快地调用 `accept` 系统调用从中取出已完成连接。
    后者是 `HTTP` 服务器经常面临的问题，在服务端客户端往返时间非常长的时候(相对于连接到达速率)，因为新 `SYN` 包在往返时间内都会占据一个连接对象。

    大多数情况下 `accept` 队列都是空的，因为一旦有一个新连接进入队列，阻塞等待的 `accept` 系统调用将返回，然后连接从队列中取出。

`Stevens` 建议的解决方案是简单地调大 `backlog` 。
但有个问题是，应用程序在调优 `backlog` 参数时，不仅需要考虑自身对新连接的处理逻辑，还需要考虑网络状况，包括往返时间等。

`Linux` 实现实际上分成两部分：应用程序只负责调解 `backlog` 参数，确保 `accept` 调用足够快以免 `accept` 队列被塞满；
系统管理员则根据网络状况调节 `/proc/sys/net/ipv4/tcp_max_syn_backlog` ，各司其职。

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst

.. comments
    comment something out below

