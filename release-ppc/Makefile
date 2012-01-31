EXTRA_FLAGS=-s -O3
CFLAGS += -arch ppc
LFLAGS += -arch ppc

all: Lineage package Admin

-include ../make.frag
	
package: release.tar.gz

release.tar.gz: $(PACKAGE_FILES)
	@if [ ! -d client.app ]; then\
		echo mkdir client.app;\
		mkdir client.app;\
	fi
	@if [ ! -d client.app/Contents ]; then\
		echo mkdir client.app/Contents;\
		mkdir client.app/Contents;\
	fi
	@if [ ! -d client.app/Contents/MacOS ]; then\
		echo mkdir client.app/Contents/MacOS;\
		mkdir client.app/Contents/MacOS;\
	fi
	@if [ ! -d client.app/Contents/Resources ]; then\
		echo mkdir client.app/Contents/Resources;\
		mkdir client.app/Contents/Resources;\
	fi
	@echo "Copying files to package folder"
	cp -f $(OUTPUT)Lineage.ini ./client.app/Contents/Resources
	cp -f Lineage ./client.app/Contents/MacOS
	cp -f Admin ./client.app/Contents/MacOS
	tar -zcvf release.tar.gz client.app
