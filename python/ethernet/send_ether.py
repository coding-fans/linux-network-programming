#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   send_ether.py
Author:     Chen Yanfei
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

import base64
import struct

from argparse import (
    ArgumentParser,
)
from fcntl import (
    ioctl,
)
from socket import (
    AF_PACKET,
    AF_INET,
    SOCK_DGRAM,
    SOCK_RAW,
    IPPROTO_RAW,
    socket,
)


def mac_ntoa(n):
    '''
        将MAC地址转化成可读形式

        n:      MAC地址二进制形式(也是网络传输的形式)
    '''

    return ':'.join([
        base64.b16encode(bytes([b])).decode('utf8')
        for b in n
    ]).lower()


def mac_aton(a):
    '''
        将MAC地址由可读形式转成二进制

        a:      MAC地址可读形式(冒号分隔十六进制)
    '''

    return b''.join([
        base64.b16decode(b.encode('utf8'))
        for b in a.upper().split(':')
    ])


def get_mac_address(iface):
    '''
        获取网卡MAC地址

        iface:      网卡名
    '''

    # 创建套接字
    s = socket(AF_INET, SOCK_DGRAM)
    # 封装网卡名
    iface_buf = struct.pack('64s', iface.encode('utf8'))
    # 调用ioctl系统调用获得MAC地址
    # 从C头文件获悉SIOCGIFHWADDR宏的值为：0x8927
    mac = ioctl(s.fileno(), 0x8927, iface_buf)[18:24]
    return mac


def send_ether(iface, to, _type, data):
    '''
        发送以太网数据

        iface:      发送网卡
        to:         目的MAC地址
        _type:      协议类型
        data:    数据
    '''

    if isinstance(to, str):
        to = mac_aton(to)
    if isinstance(data, str):
        data = data.encode('utf8')

    # 创建套接字并绑定发送网卡
    s = socket(AF_PACKET, SOCK_RAW)
    s.bind((iface, 0))

    # 根据发送网卡取得源MAC地址
    fr = get_mac_address(iface)

    # 封装以太网包头
    header = struct.pack('!6s6sH', fr, to, _type)
    packet = header + data

    # 发送数据包
    s.send(packet)

def main():
    '''
        发送以太网数据

        iface:      发送网卡
        to:         目的MAC地址
        _type:      协议类型
        data:    数据
    '''

    # 命令行参数解析器
    parser = ArgumentParser(description='Send ethernet packet.')

    # 参数：发送网卡
    parser.add_argument(
        '-i',
        '--iface',
        dest='iface',
        required=True,
    )
    # 参数：目的MAC
    parser.add_argument(
        '-t',
        '--to',
        dest='to',
        required=True,
    )
    # 参数：发送数据
    parser.add_argument(
        '-d',
        '--data',
        dest='data',
        required=True,
    )
    # 参数：协议类型
    parser.add_argument(
        '-T',
        '--type',
        dest='_type',
        default='0x0900',
        required=False,
    )

    # 解析参数
    args = parser.parse_args()

    # 根据参数发送以太网数据包
    send_ether(
        iface=args.iface,
        to=args.to,
        _type=eval(args._type),
        data=args.data,
    )


if __name__ == '__main__':
    main()
