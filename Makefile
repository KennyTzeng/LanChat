CC = gcc
CFLAGS = -g -Wall

.c.o:
	$(CC) -c $< $(CFLAGS)

link: lanchat.o sendPacket.o recvPacket.o
	$(CC) -o lanchat $^ -static
