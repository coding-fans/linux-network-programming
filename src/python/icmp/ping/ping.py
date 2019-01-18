#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   ping.py
Author:     Fasion Chan
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

import os
import socket
import struct
import sys
import threading
import time

def calculate_checksum(icmp):
    if len(icmp) % 2:
        icmp += b'\00'

    checksum = 0
    for i in range(len(icmp)//2):
        word, = struct.unpack('!H', icmp[2*i:2*i+2])
        checksum += word

    while True:
        carry = checksum >> 16
        if carry:
            checksum = (checksum & 0xffff) + carry
        else:
            break

    checksum = ~checksum & 0xffff

    return struct.pack('!H', checksum)

def calculate_checksum(icmp):
    highs = icmp[0::2]
    lows = icmp[1::2]

    checksum = ((sum(highs) << 8) + sum(lows))

    while True:
        carry = checksum >> 16
        if carry:
            checksum = (checksum & 0xffff) + carry
        else:
            break

    checksum = ~checksum & 0xffff

    return struct.pack('!H', checksum)

def pack_icmp_echo_request(ident, seq, payload):
    pseudo = struct.pack(
        '!BBHHH',
        8,
        0,
        0,
        ident,
        seq,
    ) + payload
    checksum = calculate_checksum(pseudo)
    return pseudo[:2] + checksum + pseudo[4:]

def unpack_icmp_echo_reply(icmp):
    _type, code, _, ident, seq, = struct.unpack(
        '!BBHHH',
        icmp[:8]
    )
    if _type != 0:
        return
    if code != 0:
        return

    payload = icmp[8:]

    return ident, seq, payload

def send_routine(sock, addr, ident, magic, stop):
    # first sequence no
    seq = 1

    while not stop:
        # currrent time
        sending_ts = time.time()

        # packet current time to payload
        # in order to calculate round trip time from reply
        payload = struct.pack('!d', sending_ts) + magic

        # pack icmp packet
        icmp = pack_icmp_echo_request(ident, seq, payload)

        # send it
        sock.sendto(icmp, 0, (addr, 0))

        seq += 1
        time.sleep(1)

def recv_routine(sock, ident, magic):
    while True:
        # wait for another icmp packet
        ip, (src_addr, _) = sock.recvfrom(1500)

        # unpack it
        result = unpack_icmp_echo_reply(ip[20:])
        if not result:
            continue

        # print info
        _ident, seq, payload = result
        if _ident != ident:
            continue

        sending_ts, = struct.unpack('!d', payload[:8])
        print('%s seq=%d %5.2fms' % (
            src_addr,
            seq,
            (time.time()-sending_ts) * 1000,
        ))

def ping(addr):
    # create socket for sending and receiving icmp packet
    sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)

    # id field
    ident = os.getpid()
    # magic string to pad
    magic = b'1234567890'

    # sender thread stop flag
    # append anything to stop
    sender_stop = []

    # start sender thread
    # call send_routine function to send icmp forever
    args = (sock, addr, ident, magic, sender_stop,)
    sender = threading.Thread(target=send_routine, args=args)
    sender.start()

    try:
        # receive icmp reply forever
        recv_routine(sock, ident, magic)
    except KeyboardInterrupt:
        pass

    # tell sender thread to stop
    sender_stop.append(True)

    # clean sender thread
    sender.join()

    print()

if __name__ == '__main__':
    ping(sys.argv[1])
