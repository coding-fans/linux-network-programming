#!/bin/bash
# FileName:   setup.sh
# Author:     Fasion Chan
# @contact:   fasionchan@gmail.com
# @version:   $Id$
#
# Description:
#
# Changelog:
#
#

SELF_PATH=`realpath "$0"`
SCRIPT_DIR_PATH=`dirname "$SELF_PATH"`
DOC_ROOT_PATH=`dirname "$SCRIPT_DIR_PATH"`
DOCS_PATH=`dirname "$DOC_ROOT_PATH"`
REPO_PATH=`dirname "$DOCS_PATH"`
PYENV_PATH="$DOC_ROOT_PATH/opt/pyenv"
PIP_PATH="$PYENV_PATH/bin/pip"
REAL_PYENV_PATH="$HOME/opt/pythons/pyenv.sphinx"

if [ ! -e "$REAL_PYENV_PATH" ]; then
    mkdir -p "$REAL_PYENV_PATH"
    virtualenv -p python3 "$REAL_PYENV_PATH"
fi

if [ ! -e "$PYENV_PATH" ]; then
    mkdir -p `dirname "$PYENV_PATH"`
    ln -s "$REAL_PYENV_PATH" "$PYENV_PATH"
fi

"$PIP_PATH" install -r "$REPO_PATH/requirements/docs.txt"
"$PIP_PATH" install -r "$REPO_PATH/requirements/docs-dev.txt"
