CC = gcc
CPPC = g++

nodes.o: src/nodes.c
	${CC} $^ -Iinclude -c -o $@

all:
	${CPPC} src/main.cpp -Iinclude -Llib -llua54 -lm