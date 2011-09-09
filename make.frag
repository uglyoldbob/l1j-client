#main folder objects
Lineage_OBJS = client.o config.o connection.o funcptr.o global.o lindes.o main.o \
	packet.o sdl_master.o sdl_user.o unsorted.o
Admin_OBJS = client_admin.o config.o connection.o funcptr.o global.o lindes.o \
	main.o sdl_master.o sdl_admin.o unsorted.o


#widget objects
Lineage_OBJS += widgets/sdl_animate_button.o widgets/sdl_button.o \
	widgets/sdl_char_info.o widgets/sdl_check_button.o widgets/sdl_radio_button.o \
	widgets/sdl_input_box.o widgets/sdl_plain_button.o widgets/sdl_widget.o
Admin_OBJS += widgets/sdl_animate_button.o widgets/sdl_button.o \
	widgets/sdl_char_info.o widgets/sdl_check_button.o widgets/sdl_radio_button.o \
	widgets/sdl_input_box.o widgets/sdl_plain_button.o widgets/sdl_text_button.o \
	widgets/sdl_widget.o


#resource objects
Lineage_OBJS += resources/music.o resources/pack.o resources/partial_table.o \
	resources/prepared_graphics.o resources/sdl_font.o resources/table.o \
	resources/tile.o
Admin_OBJS += resources/lin_map.o resources/music.o resources/pack.o \
	resources/partial_table.o resources/prepared_graphics.o resources/sdl_font.o \
	resources/table.o resources/tile.o


#drawmode objects
Lineage_OBJS += drawmode/sdl_drawmode.o drawmode/draw_game.o \
	drawmode/draw_login.o drawmode/draw_loading.o drawmode/draw_char_sel.o \
	drawmode/draw_new_char.o
Admin_OBJS += drawmode/sdl_drawmode.o drawmode/draw_admin_main.o \
	drawmode/draw_maint_img.o drawmode/draw_maint_map.o drawmode/draw_maint_til.o


Lineage_DEPS := $(Lineage_OBJS:.o=.d)
Admin_DEPS := $(Admin_OBJS:.o=.d)

-include $(Lineage_DEPS)
-include $(Admin_DEPS)


CC=g++

VPATH = ../src

CFLAGS =-c -Wall -I../src
INCLUDE = -I../src/
OUTPUT = ../src/
LIB_LOC = ../libs/
LFLAGS=-lmswsock -lws2_32 --enable-stdcall-fixup -Wl,-subsystem,windows -lmingw32 \
	-lSDLmain
LDADD=$(LIB_LOC)ws2_32.dll $(LIB_LOC)mswsock.dll $(LIB_LOC)SDL.dll \
	$(LIB_LOC)SDL_image.dll $(LIB_LOC)SDL_mixer.dll $(LIB_LOC)smpeg.dll

PACKAGE_FILES = Lineage $(OUTPUT)Lineage.ini $(OUTPUT)README.txt \
	$(OUTPUT)README-SDL.txt $(OUTPUT)README-SDL-image.txt $(LIB_LOC)SDL.dll \
	$(LIB_LOC)SDL_image.dll $(LIB_LOC)SDL_mixer.dll $(LIB_LOC)smpeg.dll \
	$(LIB_LOC)zlib1.dll $(LIB_LOC)libpng12-0.dll \
	$(LIB_LOC)libstdc++-6.dll $(LIB_LOC)libgcc_s_dw2-1.dll

.PHONY : all clean

clean:
	rm -rf *.o *.a *.exe *.d
	rm -rf drawmode resources widgets

Admin: $(Admin_OBJS) package
	$(CC) $(EXTRA_FLAGS) $(LFLAGS) $(Admin_OBJS) $(LDADD) -o Admin
	cp Admin ./client

Lineage: $(Lineage_OBJS)
	$(CC) $(EXTRA_FLAGS) $(LFLAGS) $(Lineage_OBJS) $(LDADD) -o Lineage

.cpp.o:
	@if [ ! -d $(@D) ]; then\
		echo mkdir $(@D);\
		mkdir $(@D);\
	fi
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(INCLUDE)$(@D) $< -o $@
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(INCLUDE)$(@D) $< -MM -MF $(@D)/$(*F).d