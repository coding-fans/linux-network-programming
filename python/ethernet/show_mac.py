#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   show_mac.py
Author:     Fasion Chan
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

import base64
import struct
import sys

from fcntl import (
    ioctl,
)
from socket import (
    AF_INET,
    SOCK_DGRAM,
    socket,
)


def mac_ntoa(n):
    '''
        Convert binary MAC address to readable format.

        Arguments
            n: binary format, must be bytes type of size 6.

        Returns
            A str type which is the readable format, like '08:00:27:c8:04:83'.
    '''

    return '%02x:%02x:%02x:%02x:%02x:%02x' % tuple(n)


def fetch_iface_mac(iface, s=None):
    '''
        Fetch MAC address of given iface.

        Arguments
            iface: iface name, str type.

        Returns
            A bytes type of size 6, which is the MAC address in binary format.
    '''

    # create socket if not given, any type is ok
    if not s:
        s = socket(AF_INET, SOCK_DGRAM)

    # pack iface name to struct ifreq
    iface_buf = struct.pack('64s', iface.encode('utf8'))

    # call ioctl to get hardware address
    # according to C header file, SIOCGIFHWADDR is 0x8927
    mac = ioctl(s.fileno(), 0x8927, iface_buf)[18:24]

    return mac


def main():
    # name of iface
    iface = sys.argv[1]

    # Fetch mac address
    mac = fetch_iface_mac(iface)

    # output result
    print('iFace: {iface}'.format(iface=iface))
    print('MAC: {mac}'.format(mac=mac_ntoa(mac)))


if __name__ == '__main__':
    main()
