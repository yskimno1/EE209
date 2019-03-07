CC = gcc209

%.o: %.c
	$(CC) -O3 -D NDEBUG -std=gnu99 -c $<

all: testheapmgrgnu testheapmgrkr testheapmgrbase testheapmgr1 testheapmgr2

clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f *~ testheapmgrgnu testheapmgrkr testheapmgrbase testheapmgr1 testheapmgr2 *.o


testheapmgrgnu : testheapmgr.o heapmgrgnu.o
	$(CC) -O3 -D NDEBUG -std=gnu99 $? -o $@

testheapmgrkr : testheapmgr.o heapmgrkr.o
	$(CC) -O3 -D NDEBUG -std=gnu99 $? -o $@

testheapmgrbase: testheapmgr.o heapmgrbase.o chunkbase.o
	$(CC) -O3 -D NDEBUG -std=gnu99 $? -o $@

testheapmgr1: testheapmgr.o heapmgr1.o chunk1.o
	$(CC) -O3 -D NDEBUG -std=gnu99 $? -o $@

testheapmgr2: testheapmgr.o heapmgr2.o chunk2.o
	$(CC) -O3 -D NDEBUG -std=gnu99 $? -o $@

testheapmgr.o: heapmgr.h
heapmgrgnu.o: heapmgr.h
heapmgrkr.o: heapmgr.h
heapmgrbase.o:
heapmgr1.o:
heapmgr2.o:

chunkbase.o: chunkbase.h
chunk1.o: chunkbase.h
chunk2.o: chunk2.h
