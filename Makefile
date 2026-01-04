nodes.o: src/nodes.c
	gcc $^ -Iinclude -c -o $@

all:
	gcc src/main.c -Iinclude -Llib -llua54 -lm