EXTRA_FLAGS=-g
CFLAGS = -arch i386
LFLAGS = -arch i386
	
PACKAGE_FILES += $(OUTPUT)run.bat

all: Lineage package Admin

-include ../make.frag

package: $(PACKAGE_FILES)
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to package folder"
	cp -f $(PACKAGE_FILES) ./client/
	touch ./client/DEBUG_DO_NOT_RELEASE.txt
