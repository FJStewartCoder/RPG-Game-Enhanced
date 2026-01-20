CC = gcc
CPPC = g++

menus.o: src/menus/menus.c
	${CC} $^ -Iinclude/menus -c -o $@

extension.o: src/extension.cpp
	${CPPC} $^ -Iinclude -c -o $@

build_help.o: src/build_help.cpp
	${CPPC} $^ -Iinclude -c -o $@

build.o: src/build.cpp
	${CPPC} $^ -Iinclude -c -o $@

log.o:
	${CC} src/log/log.c -Iinclude/log -c -o $@

nodes.o: src/nodes.cpp
	${CPPC} $^ -Iinclude -c -o $@

all: src/main.cpp nodes.o log.o build.o build_help.o extension.o menus.o
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g

clean:
	rm *.o