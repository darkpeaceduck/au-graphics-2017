#!/bin/bash

BUILD_DIR=build
PROJ_NAME=deferred

function build() {
    set -e
    if [ -n "$1" ]; then
        BUILD_DIR=$1
    fi
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake ../
    make 
}

function subuild() {
    set -e
    if [ -n "$1" ]; then
        BUILD_DIR=$1
    fi
    cd $BUILD_DIR
    make 
}

function clean() {
    set -e
    if [ -n "$1" ]; then
        BUILD_DIR=$1
    fi
    rm -rf  $BUILD_DIR
}

function run() {
    set -e
    if [ -n "$1" ]; then
        BUILD_DIR=$1
    fi
    $BUILD_DIR/bin/$PROJ_NAME
}

function deps() {
    apt-get install libglfw3-dev libassimp-dev freeglut3-dev
}


case $1 in
build):
    build $2
    ;;
subuild):
    subuild $2
    ;;
clean):
    clean $2
    ;;
run):
    run $2
    ;;
deps):
    deps 
    ;;
*):
    echo "NO CMD"
    ;;
esac