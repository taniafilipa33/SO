CFLAGS= -Wall -Wextra -pedantic -Wno-long-long -ansi -O2 -std=c99
OBJS=$(patsubst %.c, %.o, $(wildcard *.c))
EXEC=main

$(EXEC): $(OBJS)
		$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

limpar:
		rm $(EXEC) *.o

install: main