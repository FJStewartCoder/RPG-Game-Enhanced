CC = gcc
CPPC = g++

player.o: src/player.cpp
	${CPPC} $^ -Iinclude -c -o $@

nodes.o: src/nodes.cpp
	${CPPC} $^ -Iinclude -c -o $@

all: src/main.cpp nodes.o
	${CPPC} $^ -Iinclude -Llib -llua54 -lm