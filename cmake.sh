#!/bin/bash

case $1 in
    "setup")
        cmake -H. -Bbuild
        ;;
    "debug")
        cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
        ;;
    "clean")
        echo "removing build/ and libboost.asio-wrapper.a"
        rm -rf build libboost.asio-wrapper.a
        ;;
    *)
        make -C build $1
        ;;
esac

if [ $# -gt 1 ]
    then
        shift
        ./cmake.sh $*
fi

