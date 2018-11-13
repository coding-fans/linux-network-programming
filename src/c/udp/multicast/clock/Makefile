# FileName:   Makefile
# Author:     Fasion Chan
# @contact:   fasionchan@gmail.com
# @version:   $Id$
#
# Description:
#
# Changelog:
#

.DEFAULT_GOAL := all

server: server.c
	gcc -o $@ $^

client: client.c
	gcc -o $@ $^

all: server client

clean:
	rm -rf server client
