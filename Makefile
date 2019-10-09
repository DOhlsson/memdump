CC = gcc
CFLAGS = -Wall -Werror -pedantic

TARGETS = memdump memshove memwatch counter

all: $(TARGETS)

memdump: src/memdump.c src/memlib.c src/memlib.h
	$(CC) $(CFLAGS) src/memlib.c -o $@ $<

memshove: src/memshove.c
	$(CC) $(CFLAGS) -o $@ $<

memwatch: src/memwatch.c
	$(CC) $(CFLAGS) -o $@ $<

counter: src/counter.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
