#!/usr/bin/env python
# -*- encoding=utf8 -*-

'''
FileName:   send_email.py
Author:     Fasion Chan
@contact:   fasionchan@gmail.com
@version:   $Id$

Description:

Changelog:

'''

import argparse
import smtplib
import datetime


def send_email(host, port, user, password, fr, to, subject, body,
        smtp_cls=smtplib.SMTP_SSL):

    '''
        Send an email by smtp protocol

        Arguments
            host: email server host
            port: email server port
            user: user to login
            password: password
            fr: sender email address, the same as login user usually
            to: receiver email address
            subject: email title
            body: email content

        Returns
            None
    '''

    smtp = smtp_cls()

    # connect and login
    smtp.connect(host, port)
    smtp.login(user, password)

    # format message
    msg = u'''From: {fr}
To: {to}
Subject: {subject}
Date: {date}

{body}
'''.format(
        fr=fr,
        to=to,
        subject=subject,
        date=datetime.datetime.now().strftime('%d/%m/%Y %H:%M'),
        body=body,
    )

    # do send
    smtp.sendmail(fr, to, msg.encode('utf8'))
    smtp.quit()


def main():
    parser = argparse.ArgumentParser(prog='sender')

    parser.add_argument(
        '-H',
        '--host',
        dest='host',
        metavar='host',
        default='',
        required=True,
        help='email server host',
    )

    parser.add_argument(
        '-p',
        '--port',
        dest='port',
        metavar='port',
        default='',
        required=True,
        help='email server port',
    )

    parser.add_argument(
        '-u',
        '--user',
        dest='user',
        metavar='user',
        default='',
        required=True,
        help='user',
    )

    parser.add_argument(
        '-P',
        '--password',
        dest='password',
        metavar='password',
        default='',
        required=True,
        help='password',
    )

    parser.add_argument(
        '-r',
        '--receiver',
        dest='receiver',
        metavar='receiver',
        default='',
        required=True,
        help='email receiver',
    )

    parser.add_argument(
        '-s',
        '--subject',
        dest='subject',
        metavar='subject',
        default='',
        required=True,
        help='email subject',
    )

    parser.add_argument(
        '-b',
        '--body',
        dest='body',
        metavar='body',
        default='',
        required=True,
        help='email body',
    )

    args = parser.parse_args()

    send_email(
        host=args.host,
        port=int(args.port),
        user=args.user,
        password=args.password,
        fr=args.user,
        to=args.receiver,
        subject=args.subject,
        body=args.body,
        smtp_cls=smtplib.SMTP_SSL,
    )


if __name__ == '__main__':
    main()
