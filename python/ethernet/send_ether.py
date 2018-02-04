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


def mac_aton(a):
    '''
        Convert readable MAC address to binary format.

        Arguments
            a: readable format, str type of Colon-Separated Hexadecimal.

        Returns
            A bytes type of size 6, which is the binary format.
    '''

    return base64.b16decode(a.upper().replace(':', ''))


def fetch_iface_mac(iface, s=None):
    '''
        Fetch MAC address of given iface.

        Arguments
            iface: iface name, str type.

        Returns
            A bytes type of size 6, which is the MAC address in binary format.
    '''

    # create socket if given, any type is ok
    if not s:
        s = socket(AF_INET, SOCK_DGRAM)

    # pack iface name to struct ifreq
    iface_buf = struct.pack('64s', iface.encode('utf8'))

    # call ioctl to get hardware address
    # according to C header file, SIOCGIFHWADDR is 0x8927
    mac = ioctl(s.fileno(), 0x8927, iface_buf)[18:24]

    return mac


def send_ether(iface, to, _type, data, s=None):
    '''
        Send data through ethernet protocol, using raw socket.

        Arguments
            iface: name of iface for sending, str type.

            to: destination MAC addres, str type for readable or
                bytes type for binary.

            _type: protocol type, int type.

            data: data to send, str type or bytes type.
    '''

    # if destination address is readable format, convert first
    if isinstance(to, str):
        to = mac_aton(to)

    # if data is str type, encode it first
    if isinstance(data, str):
        data = data.encode('utf8')

    # create raw socket if not given
    if s is None:
        s = socket(AF_PACKET, SOCK_RAW)

    # bind to the sending iface
    s.bind((iface, 0))

    # get MAC address of sending iface, which is the source address
    fr = fetch_iface_mac(iface, s)

    # pack ethernet header
    header = struct.pack('!6s6sH', to, fr, _type)
    # pack ethernet frame
    frame = header + data

    # send the ethernet frame
    s.send(frame)


def parse_arguments():
    '''
        Parse command line arguments.

        Arguments

        Returns
            An argparse.Namespace is return, in which command options and
            arguments are stored.
    '''

    # parser for command line arguments
    parser = ArgumentParser(description='Send ethernet frame.')

    # Argument: name of iface for sending
    parser.add_argument(
        '-i',
        '--iface',
        dest='iface',
        required=True,
    )
    # Argument: destination MAC address
    parser.add_argument(
        '-t',
        '--to',
        dest='to',
        required=True,
    )
    # Argument: data to send
    parser.add_argument(
        '-d',
        '--data',
        dest='data',
        default='a' * 46,
        required=False,
    )
    # Argument: protocol type
    parser.add_argument(
        '-T',
        '--type',
        dest='_type',
        default='0x0900',
        required=False,
    )

    # parse arguments
    args = parser.parse_args()

    return args


def main():
    '''
        Entrance for the program.
    '''

    # parse command line arguments
    args = parse_arguments()

    # send ethernet frame according to given arguments
    send_ether(
        iface=args.iface,
        to=args.to,
        _type=eval(args._type),
        data=args.data,
    )

if __name__ == '__main__':
    main()
