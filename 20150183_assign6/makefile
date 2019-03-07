CC = gcc209

%.o: %.c
	$(CC) -D_BSD_SOURCE -D_GNU_SOURCE -c $<

all : ish

ish: ish.o dynarray.o
	$(CC) -D_BSD_SOURCE -D_GNU_SOURCE $? -o $@

ish.o: dynarray.h
dynarray.o: dynarray.h

clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f *.o
