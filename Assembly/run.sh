#!/bin/bash

FILE=add

g++ -S -o add.s add.cpp # 包含调试信息

./clean_asm.sh add.s  # 去除不必要的信息 


# rm -rf $FILE
# g++ -std=c++17 -o $FILE $FILE.cpp -g

# ./$FILE
# # gdb ./$FILE
# rm -rf $FILE
