CC=gcc
CFLAGS=-c -Wall -Wextra -Werror -fPIC
LDLFAGS=-shared
INCLUDES=-I/./headers
HDIR=headers
SDIR=sources
OBJECTS=so_scheduler.o priority_queue.o task_chunk.o list.o
TARGET=libscheduler.so

build: $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDLFAGS) $(OBJECTS) -o $@

so_scheduler.o: $(SDIR)/so_scheduler.c $(HDIR)/so_scheduler.h $(HDIR)/utils.h $(HDIR)/debug.h
	$(CC) $(CFLAGS) $< -o $@

priority_queue.o: $(SDIR)/priority_queue.c $(HDIR)/priority_queue.h $(HDIR)/utils.h $(HDIR)/debug.h
	$(CC) $(CFLAGS) $< -o $@

task_chunk.o: $(SDIR)/task_chunk.c $(HDIR)/task_chunk.h $(HDIR)/so_scheduler.h
	$(CC) $(CFLAGS) $< -o $@

list.o: $(SDIR)/list.c $(HDIR)/list.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(TARGET) *.o

pack:
	zip -r tema4.zip Makefile Readme $(HDIR) $(SDIR)

.PHONY: clean build
