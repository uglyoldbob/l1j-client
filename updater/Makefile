LIN_OBJS = client.o config.o connection.o global.o lindes.o main.o music.o \
	pack.o packet.o partial_table.o prepared_graphics.o sdl_animate_button.o \
	sdl_button.o sdl_font.o sdl_input_box.o sdl_master.o sdl_plain_button.o \
	sdl_user.o sdl_widget.o unsorted.o table.o

DIST_FILES = Lineage.exe Lineage.ini README.txt README-SDL.txt README-SDL-image.txt \
	SDL.dll SDL_image.dll zlib1.dll libpng12-0.dll

CC=g++

TEST=-g
RELEASE=-s -O3
CFLAGS=-c -Wall
LFLAGS=-lmswsock -lws2_32 --enable-stdcall-fixup -Wl,-subsystem,windows -lmingw32 -lSDLmain
LDADD=ws2_32.dll mswsock.dll SDL.dll SDL_image.dll

.PHONY : debug release dist

all: debug_check release_check Lineage dist

dist:
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to dist folder"
	@rm -f ./client/*
	cp -f $(DIST_FILES) ./client/
	@if [ -e debug_made ]; then\
		echo touch ./client/DEBUG_DO_NOT_RELEASE.txt;\
		touch ./client/DEBUG_DO_NOT_RELEASE.txt;\
		rm -f release.tar.gz;\
	fi
	@if [ -e release_made ]; then\
		echo tar -zcvf release.tar.gz client;\
		tar -zcvf release.tar.gz client;\
	fi

	
clean:
	rm -f *.o Lineage.exe Lineage_dbg.exe Lineage_rls.exe Lineage debug_made release_made
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
		cp -f Lineage_dbg.exe Lineage.exe;\
	fi
	@if [ -e release_made ]; then\
		echo $(CC) $(RELEASE) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_rls;\
		$(CC) $(RELEASE) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage_rls;\
		cp -f Lineage_rls.exe Lineage.exe;\
	fi

.cpp.o:
	@if [ -e release ]; then\
		echo $(CC) $(RELEASE) $(CFLAGS) $<;\
		$(CC) $(RELEASE) $(CFLAGS) $< && touch release_made;\
	else\
		echo $(CC) $(TEST) $(CFLAGS) $<;\
		$(CC) $(TEST) $(CFLAGS) $< && touch debug_made;\
	fi
