CC=gcc
CFLAGS=-c -Wall -g -fPIC
LDFLAGS=-shared
LIBNAME=libvmsim.so
SOURCES=vmsim.c common_lin.c list.c
OBJECTS=$(SOURCES:.c=.o)


build: $(SOURCES) $(LIBNAME)

$(LIBNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(LIBNAME) *.o

.PHONY: clean build
