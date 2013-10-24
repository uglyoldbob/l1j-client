bin_PROGRAMS = Lineage Admin
if WINDOWS
Lineage_LDFLAGS = -lmingw32
endif
Lineage_SOURCES = \
src/client.cpp \
src/config.cpp \
src/connection.cpp \
src/funcptr.cpp \
src/global.cpp \
src/lindes.cpp \
src/main.cpp \
src/packet.cpp \
src/sdl_master.cpp \
src/sdl_user.cpp \
src/unsorted.cpp \
src/widgets/chat_window.cpp \
src/widgets/sdl_animate_button.cpp \
src/widgets/sdl_button.cpp \
src/widgets/sdl_char_info.cpp \
src/widgets/sdl_check_button.cpp \
src/widgets/sdl_lin_map.cpp \
src/widgets/sdl_radio_button.cpp \
src/widgets/sdl_input_box.cpp \
src/widgets/sdl_plain_button.cpp \
src/widgets/sdl_text_button.cpp \
src/widgets/sdl_widget.cpp \
src/widgets/sdl_window.cpp \
src/widgets/sprite.cpp \
src/widgets/text_box.cpp \
src/resources/briefcase.cpp \
src/resources/files.cpp \
src/resources/music.cpp \
src/resources/pack.cpp \
src/resources/partial_table.cpp \
src/resources/prepared_graphics.cpp \
src/resources/sdl_font.cpp \
src/resources/reg_font.cpp \
src/resources/small_font.cpp \
src/resources/map_coord.cpp \
src/resources/pixel_coord.cpp \
src/resources/coord.cpp \
src/resources/screen_coord.cpp \
src/resources/sdl_graphic.cpp \
src/resources/table.cpp \
src/resources/tile.cpp \
src/drawmode/sdl_drawmode.cpp \
src/drawmode/draw_game.cpp \
src/drawmode/draw_login.cpp \
src/drawmode/draw_loading.cpp \
src/drawmode/draw_char_sel.cpp \
src/drawmode/draw_new_char.cpp \
src/encryption/mains.c \
src/encryption/sha256deep.c \
src/encryption/hash.c \
src/encryption/helpers.c \
src/encryption/sha256.c
if MAC
Lineage_SOURCES += src/SDLMain.m
endif

if WINDOWS
Admin_LDFLAGS = -lmingw32
endif

Admin_SOURCES = \
src/client_admin.cpp \
src/config.cpp \
src/connection.cpp \
src/funcptr.cpp \
src/global.cpp \
src/lindes.cpp \
src/main.cpp \
src/sdl_master.cpp \
src/sdl_admin.cpp \
src/unsorted.cpp \
src/widgets/chat_window.cpp \
src/widgets/sdl_animate_button.cpp \
src/widgets/sdl_button.cpp \
src/widgets/sdl_char_info.cpp \
src/widgets/sdl_check_button.cpp \
src/widgets/sdl_lin_map.cpp \
src/widgets/sdl_radio_button.cpp \
src/widgets/sdl_input_box.cpp \
src/widgets/sdl_plain_button.cpp \
src/widgets/sdl_text_button.cpp \
src/widgets/sdl_widget.cpp \
src/widgets/sdl_window.cpp \
src/widgets/sprite.cpp \
src/widgets/text_box.cpp \
src/resources/briefcase.cpp \
src/resources/files.cpp \
src/resources/music.cpp \
src/resources/pack.cpp \
src/resources/partial_table.cpp \
src/resources/prepared_graphics.cpp \
src/resources/sdl_font.cpp \
src/resources/reg_font.cpp \
src/resources/small_font.cpp \
src/resources/map_coord.cpp \
src/resources/pixel_coord.cpp \
src/resources/coord.cpp \
src/resources/screen_coord.cpp \
src/resources/sdl_graphic.cpp \
src/resources/table.cpp \
src/resources/tile.cpp \
src/drawmode/sdl_drawmode.cpp \
src/drawmode/draw_admin_main.cpp \
src/drawmode/draw_maint_img.cpp \
src/drawmode/draw_maint_map.cpp \
src/drawmode/draw_maint_til.cpp \
src/encryption/mains.c \
src/encryption/sha256deep.c \
src/encryption/hash.c \
src/encryption/helpers.c \
src/encryption/sha256.c
if MAC
Admin_SOURCES += src/SDLMain.m
endif

AM_CPPFLAGS = -I${top_srcdir}/src -I${top_srcdir}/include

if LINUX
TEST_DEP = linux_test
INSTALLER_DEP = linux_installer
PACKAGE_FILES = Lineage${EXEEXT} ${top_srcdir}/src/README.txt
else

if WINDOWS
TEST_DEP = win_test
INSTALLER_DEP = win_installer
else

if MAC
TEST_DEP = mac_test
INSTALLER_DEP = mac_installer
else

INSTALLER_DEP = fail
endif
endif
endif

mac_test:
	echo "Making test folder for mac"
	
	false

mac_installer:
	echo "Cannot make installer for mac"
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to test folder"
	cp -f ${top_srcdir}/src/LineageMac.ini ./client/Lineage.ini
	cp -f $(PACKAGE_FILES) ./client/

win_test:
	echo "Making test folder for windows"
	false

win_installer:
	echo "Cannot make installer for windows"
	false

linux_installer:
	echo "making installer for linux"
	@if [ ! -d installer ]; then\
		echo mkdir installer;\
		mkdir installer;\
	fi
	@echo "Copying files to installer folder"
	cp -f ${top_srcdir}/src/LineageLinux.ini ./installer/Lineage.ini
	cp -f $(PACKAGE_FILES) ./installer/
	$(STRIP) -s ./installer/Lineage${EXEEXT}

linux_test:
	@echo "making test folder for linux"
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to test folder"
	cp -f ${top_srcdir}/src/LineageLinux.ini ./client/Lineage.ini
	cp -f $(PACKAGE_FILES) ./client/

installer: Lineage$(EXEEXT) $(INSTALLER_DEP)
	echo "Finished making installer package"

test: Lineage$(EXEEXT) $(TEST_DEP)
	echo "Finished making test install"

#no installers shall be made when debugging is enabled
fail:
	echo "Unknown system - cannot make an installer"
	false
