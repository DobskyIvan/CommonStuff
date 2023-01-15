#!/bin/sh
gcc -Wall -Wextra -o a.o $1
echo $'\n	GCC work is done!\n'
echo $1- programm run here:
./a.o
rm a.o
echo $1- programm is done!
echo $'\n'
