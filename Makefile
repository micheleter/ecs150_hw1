all: sshell

sshell: main.o
	gcc -Wall -Wextra -Werror -o sshell main.o
main.o: sshell.c
	gcc -Wall -Wextra -Werror -c -o main.o sshell.c
	
clean:
	rm -f sshell main.o