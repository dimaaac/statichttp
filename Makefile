CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lssl -lcrypto

all: server

server: server.c
        $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
        rm -f server

