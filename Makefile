CC=gcc
CFLAGS=-std=c11 -O2

all: myshell

myshell: myshell.o lex.yy.o
	$(CC) $(CFLAGS) -Wall -o myshell myshell.o lex.yy.o -lfl -lreadline

# Only turn on warning for my own code
myshell.o: myshell.c
	$(CC) $(CFLAGS) -Wall -c myshell.c

# Ignore all warnings for lexer
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -w -c lex.yy.c

lex.yy.c: parse_line.lex
	flex parse_line.lex

.PHONY:clean
clean:
	rm -f *.o *~ myshell lex.yy.c
