LIN_OBJS = config.o connection.o encrypt.o main.o pack.o packet.o
CC=g++
DEBUG=-g
CFLAGS=-c -Wall $(DEBUG)
LFLAGS=$(DEBUG) --enable-stdcall-fixup -lws2_32 -lmswsock -ladvapi32
LDADD=ws2_32.dll mswsock.dll advapi32.dll

all: Lineage install

install:
	cp -f Lineage "C:\Program Files\NCsoft\Lineage"
	
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

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

pack.o: pack.cpp pack.h
	$(CC) $(CFLAGS) pack.cpp
