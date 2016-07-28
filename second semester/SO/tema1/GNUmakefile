CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

build: tema1

tema1: main.o hashtable.o hash.o list.o nodes.o utils.o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $<

hashtable.o: hashtable.c
	$(CC) $(CFLAGS) -c $<

hash.o: hash.c
	$(CC) $(CFLAGS) -c $<

list.o: list.c
	$(CC) $(CFLAGS) -c $<

nodes.o: nodes.c
	$(CC) $(CFLAGS) -c $<

utils.o: utils.c
	$(CC) $(CFLAGS) -c $<

pack:
	zip tema_roty *

.PHONY: clean
clean:
	rm -f *.o *~ tema1
