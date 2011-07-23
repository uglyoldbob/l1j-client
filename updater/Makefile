LIN_OBJS = config.o connection.o encrypt.o main.o pack.o
CC=g++
DEBUG=-g
CFLAGS=-c $(DEBUG)
LFLAGS=$(DEBUG)
LDADD=

Lineage: $(LIN_OBJS)
	$(CC) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage
	
