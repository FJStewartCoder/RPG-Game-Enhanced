CC = gcc
CPPC = g++

nodes.o: src/nodes.cpp
	${CC} $^ -Iinclude -c -o $@

all: src/main.cpp nodes.o
	${CPPC} $^ -Iinclude -Llib -llua54 -lm