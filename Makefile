CC = gcc
CFLAGS = -g

src = $(wildcard lib/*.c) \
			$(wildcard covert_channel/*.c) \

obj = $(src:.c=.o)

all:	covert-channel

covert-channel:	sender receiver

sender:	covert_channel/sender.o lib/util.o
	$(CC) $^ -o $@

receiver: covert_channel/receiver.o lib/util.o
	$(CC)  $^ -o $@


.PHONY: clean
clean:
	rm -f sender receiver $(obj)
