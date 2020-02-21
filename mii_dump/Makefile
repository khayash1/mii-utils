#
# mii_dump - MII register dump utility
#

CC=$(CROSS_COMPILE)gcc

TARGET=mii_dump
OBJS=mii_dump.o

CFLAGS=-O -g

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
