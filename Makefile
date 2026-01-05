CC = gcc
CPPC = g++

log.o:
	${CC} src/log/log.c -Iinclude/log -c -o $@

player.o: src/player.cpp
	${CPPC} $^ -Iinclude -c -o $@

nodes.o: src/nodes.cpp
	${CPPC} $^ -Iinclude -c -o $@

all: src/main.cpp nodes.o log.o
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g