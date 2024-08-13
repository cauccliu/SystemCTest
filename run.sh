#!/bin/bash

FILE=sc_spawn

rm -rf $FILE
g++ -std=c++17 -I/home/chang.liu/systemcLib/include -L/home/chang.liu/systemcLib/lib-linux64 -lsystemc -o $FILE $FILE.cpp -g
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/chang.liu/systemcLib/lib-linux64
./$FILE
# gdb ./$FILE
rm -rf $FILE