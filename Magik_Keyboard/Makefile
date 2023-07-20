# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -Wpedantic -std=c99 -O0 -g

# define targets
TARGETS= mk

#define object-files
OBJ=mk.o

$(TRIE).o: $(TRIE).c $(TRIE).h
	$(CC) $(CFLAGS) $^ -c

build: $(TARGETS)

mk: mk.o trie.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

pack:
	zip -FSr Magik_Keyboard.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJ)

.PHONY: pack clean
