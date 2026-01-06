CC = gcc
CPPC = g++

extension.o: src/extension.cpp
	${CPPC} $^ -Iinclude -c -o $@

build_help.o: src/build_help.cpp
	${CPPC} $^ -Iinclude -c -o $@

build.o: src/build.cpp
	${CPPC} $^ -Iinclude -c -o $@

log.o:
	${CC} src/log/log.c -Iinclude/log -c -o $@

player.o: src/player.cpp
	${CPPC} $^ -Iinclude -c -o $@

nodes.o: src/nodes.cpp
	${CPPC} $^ -Iinclude -c -o $@

all: src/main.cpp nodes.o log.o build.o build_help.o extension.o
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g

clean:
	rm *.o