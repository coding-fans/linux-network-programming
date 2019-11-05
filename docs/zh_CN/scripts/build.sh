#!/bin/sh

# Author: fasion
# Created time: 2019-11-04 08:38:35
# Last Modified by: fasion
# Last Modified time: 2019-11-04 08:44:15

SELF_PATH=`realpath "$0"`
SCRIPT_DIR_PATH=`dirname "$SELF_PATH"`
DOC_ROOT_PATH=`dirname "$SCRIPT_DIR_PATH"`
DOCS_PATH=`dirname "$DOC_ROOT_PATH"`
REPO_PATH=`dirname "$DOCS_PATH"`
PYENV_PATH="$DOC_ROOT_PATH/opt/pyenv"
PIP_PATH="$PYENV_PATH/bin/pip"
REAL_PYENV_PATH="$HOME/opt/pythons/pyenv.sphinx"
PYENV_BIN_PATH="$PYENV_PATH/bin"

(
    cd "$DOC_ROOT_PATH"
    PATH="$PATH:$PYENV_BIN_PATH" make html
)
