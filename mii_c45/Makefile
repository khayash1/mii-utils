#
# mii_c45 - MII register read/write utility
#

CC=$(CROSS_COMPILE)gcc

TARGET=mii_c45
OBJS=mii_c45.o

CFLAGS=-O -g

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
