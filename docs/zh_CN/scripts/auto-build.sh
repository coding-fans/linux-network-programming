#!/bin/bash
# FileName:   auto-build.sh
# Author:     Fasion Chan
# @contact:   fasionchan@gmail.com
# @version:   $Id$
#
# Description:
#
# Changelog:
#
#

BIND_ADDR=lnp.local
sudo ifconfig lo0 alias $BIND_ADDR up
sphinx-autobuild -H $BIND_ADDR . _build/html
