#!/bin/bash

FILE=list

rm -rf $FILE
g++ -std=c++17 -o $FILE $FILE.cpp -g

./$FILE
# gdb ./$FILE
rm -rf $FILE
