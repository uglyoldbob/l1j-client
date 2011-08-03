LIN_OBJS = client.o config.o connection.o lindes.o main.o music.o pack.o \
	packet.o partial_table.o prepared_graphics.o sdl_master.o sdl_user.o \
	sdl_widget.o unsorted.o table.o
CC=g++

TEST=-g
RELEASE=-s -O3
CFLAGS=-c -Wall
LFLAGS=
LDADD=

.PHONY : debug release

all: debug_check release_check Lineage install

install:
	cp -f ./Lineage "somewhere"
	@if [ -e release_made ]; then\
		if [ ! -d deploy ]; then\
			echo mkdir deploy;\
			mkdir deploy;\
		fi;\
		echo "Copying files to deploy folder";\
		cp -f "deploy_files" ./deploy/;\
		echo tar -zcvf release.tar.gz deploy;\
		tar -zcvf release.tar.gz deploy;\
	fi
	
clean:
	rm -f *.o Lineage* debug_made release_made
	@if [ -e debug ] && [ -e release ]; then\
		echo "Making DEBUG version";\
		rm -f release;\
	fi
	
release:
	@if [ -e debug ]; then\
		echo "make clean";\
		make clean;\
	fi
	rm -f debug
	touch release
	make Lineage
	
debug:
	@if [ -e release ]; then\
		echo "make clean";\
		make clean;\
	fi
	rm -f release
	touch debug
	make Lineage

release_check:
	@if [ -e release ] && [ ! -e Lineage_rls ]; then\
		echo "Making RELEASE version";\
		rm -f debug;\
		make clean;\
	fi
	
debug_check:
	@if [ -e debug ] && [ ! -e Lineage_dbg ]; then\
		echo "Making DEBUG version";\
		rm -f release;\
		make clean;\
	fi

Lineage: $(LIN_OBJS)
	@if [ -e debug_made ] && [ -e release_made ]; then\
		echo Make clean because of mixed debug and release objects;\
		make clean;\
	fi
	@if [ -e debug_made ]; then\
		echo $(CC) $(TEST) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_dbg;\
		$(CC) $(TEST) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_dbg;\
		cp -f Lineage_dbg Lineage;\
	fi
	@if [ -e release_made ]; then\
		echo $(CC) $(RELEASE) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_rls;\
		$(CC) $(RELEASE) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_rls;\
		cp -f Lineage_rls Lineage;\
	fi

.cpp.o:
	@if [ -e release ]; then\
		echo $(CC) $(RELEASE) $(CFLAGS) $<;\
		$(CC) $(RELEASE) $(CFLAGS) $< && touch release_made;\
	else\
		echo $(CC) $(TEST) $(CFLAGS) $<;\
		$(CC) $(TEST) $(CFLAGS) $< && touch debug_made;\
	fi
