LIN_OBJS = client.o config.o connection.o lindes.o main.o \
			music.o pack.o packet.o unsorted.o table.o
CC=g++
DEBUG=-g
CFLAGS=-c -Wall $(DEBUG)
LFLAGS=$(DEBUG) --enable-stdcall-fixup -lws2_32 -lmswsock# -ladvapi32
LDADD=ws2_32.dll mswsock.dll# advapi32.dll

all: Lineage install

install:
	cp -f ./Lineage "C:\Program Files\NCsoft\Lineage"
	cp -f ./run.bat "C:\Program Files\NCsoft\Lineage"

clean:
	rm -f *.o Lineage

Lineage: $(LIN_OBJS)
	$(CC) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage

client.o: client.cpp client.h
	$(CC) $(CFLAGS) client.cpp unsorted.cpp	

config.o: config.cpp config.h
	$(CC) $(CFLAGS) config.cpp

connection.o: connection.cpp connection.h
	$(CC) $(CFLAGS) connection.cpp

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
