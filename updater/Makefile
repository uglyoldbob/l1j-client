LIN_OBJS = config.o connection.o encrypt.o main.o pack.o packet.o
CC=g++
DEBUG=-g
CFLAGS=-c -Wall $(DEBUG)
LFLAGS=$(DEBUG)
LDADD=

all: Lineage install

install:
	cp -f ./Lineage "/Applications/Lineage.app/Contents/MacOS"
	cp -f ./Lineage.ini "/Applications/Lineage.app/Contents/MacOS"

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
