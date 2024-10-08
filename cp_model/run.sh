# rm -rf main

# g++ *.cpp -o output

g++ -std=c++17 -I/home/chang.liu/systemcLib/include -L/home/chang.liu/systemcLib/lib-linux64 -lsystemc  *.cpp -o main -g 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/chang.liu/systemcLib/lib-linux64

./main 
# ./main >> logs.txt 2>&1
# gdb ./main ls