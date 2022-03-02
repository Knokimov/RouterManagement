CC = gcc
CFLAGS = -Wall -Wextra -lm
BINARIES = RouterManagement
all: $(BINARIES)

ruterdrift: RouterManagement.c Commands.c
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(BINARIES) *.dat