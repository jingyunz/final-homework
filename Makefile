all: jysh


CC=gcc
SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRC))
CFLAGS += -Wall -I. -std=gnu99 -lreadline -O0 -g

jysh: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.c.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm *.o jysh

run:
	./jysh

