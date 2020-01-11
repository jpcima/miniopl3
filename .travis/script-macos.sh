#!/bin/bash

# ---------------------------------------------------------------------------------------------------------------------
# stop on error

set -e

# ---------------------------------------------------------------------------------------------------------------------
# print shell commands

set -x

# ---------------------------------------------------------------------------------------------------------------------
# setup

export PKG_CONFIG="pkg-config --static"
export PKG_CONFIG_PATH="/opt/local/lib/pkgconfig"

export PATH=/opt/local/bin:"$PATH"

export CFLAGS="-I/opt/local/include -arch i386 -arch x86_64"
export CXXFLAGS="-I/opt/local/include -arch i386 -arch x86_64"
export LDFLAGS="-L/opt/local/lib -arch i386 -arch x86_64"

# ---------------------------------------------------------------------------------------------------------------------
# build the plugin

MACOS=true make $MAKE_ARGS
