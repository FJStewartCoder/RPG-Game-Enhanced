CC = gcc
CPPC = g++


inject_api.o: src/inject/inject_api.cpp
	${CPPC} $^ -Iinclude/inject -Iinclude -c -o $@

inject_build.o: src/inject/inject_build.cpp
	${CPPC} $^ -Iinclude/inject -Iinclude -c -o $@

inject_core.o: src/inject/inject_core.cpp
	${CPPC} $^ -Iinclude/inject -Iinclude -c -o $@


INJECT = inject_api.o inject_build.o inject_core.o

menus.o: src/menus/menus.cpp
	${CPPC} $^ -Iinclude/menus -Iinclude -c -o $@

save.o: src/save.cpp
	${CPPC} $^ -Iinclude -c -o $@

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

all: src/main.cpp nodes.o log.o build.o build_help.o extension.o menus.o save.o ${INJECT}
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g

# make all but with the DEV macro defined
dev: src/main.cpp nodes.o log.o build.o build_help.o extension.o menus.o save.o ${INJECT}
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g -DDEV

clean:
	rm *.o