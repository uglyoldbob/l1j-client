LIN_OBJS = config.o connection.o encrypt.o lindes.o \
			main.o music.o pack.o packet.o unsorted.o \
			table.o
CC=g++
DEBUG=-g
CFLAGS=-c -Wall $(DEBUG)
LFLAGS=$(DEBUG)
LDADD=

all: Lineage install

install:
	cp -f ./Lineage ~/Lineage
	cp -f ./Lineage.ini ~/Lineage
	
clean:
	rm -f *.o Lineage

Lineage: $(LIN_OBJS)
	$(CC) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage
	
config.o: config.cpp config.h
	$(CC) $(CFLAGS) config.cpp

connection.o: connection.cpp connection.h
	$(CC) $(CFLAGS) connection.cpp

encrypt.o: encrypt.cpp
	$(CC) $(CFLAGS) encrypt.cpp
	
lindes.o: lindes.cpp lindes.h
	$(CC) $(CFLAGS) lindes.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp
	
music.o: music.cpp music.h
	$(CC) $(CFLAGS) music.cpp

pack.o: pack.cpp pack.h
	$(CC) $(CFLAGS) pack.cpp

packet.o: packet.cpp packet.h
	$(CC) $(CFLAGS) packet.cpp
	
table.o: table.cpp table.h
	$(CC) $(CFLAGS) table.cpp
	
unsorted.o: unsorted.cpp unsorted.h
	$(CC) $(CFLAGS) unsorted.cpp
