#!/bin/bash
gcc -ggdb -o main main.cpp
gcc -ggdb -o writebuf writebuf.cpp
gcc -ggdb -o readbuf readbuf.cpp
./main input.txt  ouput.txt
