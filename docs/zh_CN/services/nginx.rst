.. Nginx
    FileName:   nginx.rst
    Author:     Fasion Chan
    Created:    2018-07-09 20:17:32
    @contact:   fasionchan@gmail.com
    @version:   $Id$

    Description:

    Changelog:

=====
Nginx
=====

配置片段
========

静态资源缓存
------------

.. code-block:: nginx

    server {
        listen 80;
        server_name some-site.com;

        root /some/path/to/site;

        # 缓存图片
        location ~ \.(jpg|png|jpeg|bmp|gif|swf)$ {
            root /some/path/to/site/images;
            if (-f $request_filename) {
                expires 7d;
                break;
            }
        }

        # 缓存样式
        location ~ \.(css)$ {
            root /some/path/to/site/css;
            if (-f $request_filename) {
                expires 3d;
                break;
            }
        }

        # 缓存脚本
        location ~ \.(js)$ {
            root /some/path/to/site/js;
            if (-f $request_filename) {
                expires 1d;
                break;
            }
        }
    }

反向代理
--------

.. code-block:: nginx

    server {
        # 监听端口
        listen 80;
        # 代理对外域名
        server_name proxy-site.com;

        location / {
            # 转向服务器
            proxy_pass http://dest-site.com;
            proxy_redirect default;
        }
    }

    # 服务器集群及权重(可选)
    upstream dest-site.com {
        server 10.0.0.1:80 weight=1;
    }

Web Socket 转发
---------------

.. code-block:: nginx

    location /wsapp/ {
        proxy_pass http://wsbackend;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "Upgrade";
    }

下一步
======

.. include:: /_fragments/next-step-to-wechat-mp.rst

参考文献
========

#. `Using NGINX as a WebSocket Proxy <https://www.nginx.com/blog/websocket-nginx/>`_

.. include:: /_fragments/wechat-reward.rst

.. include:: /_fragments/disqus.rst


.. comments
    comment something out below

    .. meta::
        :description lang=zh:
        :keywords:

