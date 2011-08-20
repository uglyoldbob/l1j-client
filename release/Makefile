LIN_OBJS = client.o config.o connection.o global.o lindes.o main.o packet.o \
	sdl_master.o sdl_user.o unsorted.o

SDL_WIDGET_OBJS = widgets/sdl_animate_button.o widgets/sdl_button.o \
	widgets/sdl_char_info.o widgets/sdl_input_box.o \
	widgets/sdl_plain_button.o widgets/sdl_widget.o

RESOURCE_OBJS = resources/music.o resources/pack.o resources/partial_table.o \
	resources/prepared_graphics.o resources/sdl_font.o resources/table.o
	
CC=g++

VPATH = ../src

RELEASE=-s -O3
CFLAGS =-c -Wall -I../src
INCLUDE = -I../src/
OUTPUT = ../src/
LIB_LOC = ../libs/
LFLAGS=
LDADD=sdl_widgets.a lin_res.a
	
PACKAGE_FILES = Lineage $(OUTPUT)Lineage.ini $(OUTPUT)README.txt

.PHONY : all clean

all: Lineage package

package: $(PACKAGE_FILES)
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to package folder"
	cp -f $(PACKAGE_FILES) ./client/
	rm -f ./client/std*
	tar -zcvf release.tar.gz client
		
clean:
	rm -rf *.o *.a *.exe
	
Lineage: $(LIN_OBJS) sdl_widgets.a lin_res.a
	$(CC) $(RELEASE) $(LFLAGS) $(LIN_OBJS) $(LDADD) -o Lineage
	
sdl_widgets.a: widgets/sdl_widgets.a
	cp -f widgets/sdl_widgets.a sdl_widgets.a

widgets/sdl_widgets.a: $(SDL_WIDGET_OBJS)
	ar rvs widgets/sdl_widgets.a $(SDL_WIDGET_OBJS)

lin_res.a: resources/lin_res.a
	cp -f resources/lin_res.a lin_res.a

resources/lin_res.a: $(RESOURCE_OBJS)
	ar rvs resources/lin_res.a $(RESOURCE_OBJS)
	
.cpp.o:
	@if [ ! -d $(@D) ]; then\
		echo mkdir $(@D);\
		mkdir $(@D);\
	fi
	$(CC) $(RELEASE) $(CFLAGS) $(INCLUDE)$(@D) $< -o $@