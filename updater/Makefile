LIN_OBJS = main.o config.o connection.o encrypt.o pack.o
CC=g++
DEBUG=-g
CFLAGS=-c -Wall $(DEBUG)
LFLAGS=$(DEBUG)
LDADD=

all: Lineage install

install:
	cp -f ./Lineage "/media/E03CC5AA3CC57BD2/Program Files/NCsoft/Lineage/"

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

clean:
	rm -f *.o Lineage
