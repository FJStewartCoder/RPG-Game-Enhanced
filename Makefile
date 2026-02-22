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

input.o: src/input.cpp
	${CPPC} $^ -Iinclude -c -o $@

campaign.o: src/campaign.cpp
	${CPPC} $^ -Iinclude -c -o $@

table.o: src/table.cpp
	${CPPC} $^ -Iinclude -c -o $@

SOURCES = src/main.cpp nodes.o log.o build.o build_help.o extension.o menus.o save.o ${INJECT} input.o campaign.o table.o lua54.dll

# DEV mode can be enabled in settings.h
all: ${SOURCES}
	${CPPC} $^ -Iinclude -Llib -llua54 -lm -g -o RPenGine

clean:
	rm *.o