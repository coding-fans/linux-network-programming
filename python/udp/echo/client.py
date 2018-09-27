#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   client.py
Author:     Fasion Chan
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

from argparse import (
    ArgumentParser,
)
from socket import (
    socket,
    AF_INET,
    SOCK_DGRAM,
)

BUFFER_SIZE = 1024


def parse_arguments():
    parser = ArgumentParser(description='client.py: talk with udp echo server')

    parser.add_argument(
        '-a',
        '--server-address',
        dest='server_addr',
        required=False,
        default='127.0.0.1',
    )

    parser.add_argument(
        '-p',
        '--server-port',
        dest='server_port',
        required=False,
        default=55555,
    )

    parser.add_argument(
        '-d',
        '--data',
        dest='data',
        required=False,
        default='Hello, world!',
    )

    args = parser.parse_args()

    return args


def main():
    args = parse_arguments()
    s = socket(AF_INET, SOCK_DGRAM)
    s.sendto(args.data.encode('utf8'), (args.server_addr, args.server_port))
    msg, peer = s.recvfrom(BUFFER_SIZE)
    print(msg.decode('utf8'))

if __name__ == '__main__':
    main()
