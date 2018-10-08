#!/bin/bash

ROOT_PATH=$(pwd)
LIB=$ROOT_PATH/lib

export LD_LIBRARY_PATH=$LIB; ./httpd 8080
