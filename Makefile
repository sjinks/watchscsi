all: watchscsi

CFLAGS=-Os -g

watchscsi: watchscsi.o
	$(CC) $(LDFLAGS) -o "$@" $^

watchscsi.o: watchscsi.c

.PHONY: clean

clean:
	-rm -f watchscsi.o watchscsi
