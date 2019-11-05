#!/bin/sh

# Author: fasion
# Created time: 2019-11-04 09:34:59
# Last Modified by: fasion
# Last Modified time: 2019-11-05 08:19:42

SELF_PATH=`realpath "$0"`
SCRIPT_DIR_PATH=`dirname "$SELF_PATH"`
DOC_ROOT_PATH=`dirname "$SCRIPT_DIR_PATH"`
DOCS_PATH=`dirname "$DOC_ROOT_PATH"`
REPO_PATH=`dirname "$DOCS_PATH"`
VAR_PATH="$DOC_ROOT_PATH/var"
CLONE_PATH="$VAR_PATH/clone"

if [ -e "$CLONE_PATH" ]; then
    read -p "delete clone repo $CLONE_PATH (y or n)? " value
    if [ "$value" == "y" ]; then
        rm -rf "$CLONE_PATH"
    else
        exit 1
    fi
fi

git clone $REPO_PATH $CLONE_PATH

(
    cd "$CLONE_PATH" && \
        "$CLONE_PATH/docs/zh_CN/scripts/setup.sh" && \
        "$CLONE_PATH/docs/zh_CN/scripts/build.sh" && \
        ossutil -e oss-cn-shanghai.aliyuncs.com cp -f -r "$CLONE_PATH/docs/zh_CN/_build/html" oss://works-network/zh_CN/latest
)
