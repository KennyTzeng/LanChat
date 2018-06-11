CC = gcc
CFLAGS = -g -Wall

.c.o:
	$(CC) -c $< $(CFLAGS)

link: lanchat.o
	$(CC) -o lanchat $^ -static
