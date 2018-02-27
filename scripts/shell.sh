#!/bin/bash -i

# Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
#
# This file is part of Quicken.
#
# Quicken is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; version 3.
#
# Quicken is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with Quicken. If not, see <http://www.gnu.org/licenses/>.

# This script spawns a new Bash shell with all the environment variables needed
# to run apps based on the local source tree uninstalled. Note that qmake must
# be run before in order to have the .build_dir.inc file generated:

#  $ qmake
#  $ ./scripts/shell.sh
#  [Quicken] $ make
#  [Quicken] $ ./bin/qmlscene-quicken <file>

NAME='Quicken'

BUILD_PATHS_FILENAME=`dirname ${BASH_SOURCE[0]}`/../.build_paths.inc
if ! [ -f $BUILD_DIR_FILENAME ]
then
    echo "Generate makefiles with 'qmake' first!"
    exit 1
fi
. $BUILD_PATHS_FILENAME || exit 1

export QML2_IMPORT_PATH=$BUILD_DIR/qml
export LD_LIBRARY_PATH=$BUILD_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export PKG_CONFIG_PATH=$BUILD_DIR/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}
export PATH=$BUILD_DIR/bin${PATH:+:$PATH}

TEMP_FILE=`mktemp -t bashrc.XXXXXXXX`
echo source $HOME/.bashrc >> $TEMP_FILE
echo PS1=\'[$NAME] $PS1\' >> $TEMP_FILE
SHELL_OPTIONS="--init-file $TEMP_FILE"

echo Entering $NAME shell.
$SHELL $SHELL_OPTIONS
echo Leaving $NAME shell. Have a nice day!

if test ! -z "$TEMP_FILE"
then
    rm $TEMP_FILE
fi
