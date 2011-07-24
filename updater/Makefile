LIN_OBJS = config.o connection.o encrypt.o main.o pack.o
CC=g++
DEBUG=-g
CFLAGS=-c $(DEBUG)
LFLAGS=$(DEBUG) --enable-stdcall-fixup -lws2_32 -lmswsock -ladvapi32
LDADD=ws2_32.dll mswsock.dll advapi32.dll

all: Lineage install

install:
	cp -f Lineage "C:\Program Files\NCsoft\Lineage"
	
clean:
	rm -f *.o Lineage

Lineage: $(LIN_OBJS)
	$(CC) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage
	
