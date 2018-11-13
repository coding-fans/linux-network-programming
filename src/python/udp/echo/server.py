#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   server.py
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
    parser = ArgumentParser(description='server.py: udp echo server')

    parser.add_argument(
        '-a',
        '--bind-address',
        dest='bind_addr',
        required=False,
        default='0.0.0.0',
    )

    parser.add_argument(
        '-p',
        '--bind-port',
        dest='bind_port',
        required=False,
        default=55555,
    )

    args = parser.parse_args()

    return args

def main():
    args = parse_arguments()
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind((args.bind_addr, args.bind_port))
    while True:
        msg, peer = s.recvfrom(BUFFER_SIZE)
        print('Recieved %4d bytes from %s' % (len(msg), peer))
        s.sendto(msg, peer)

if __name__ == '__main__':
    main()
