
#!/bin/sh
name=$1

gcc -Wall -Wextra -o ${name%.c}.o $1
echo $'\nGCC work is done!\n'
