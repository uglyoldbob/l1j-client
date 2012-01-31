EXTRA_FLAGS=-s -O3

all: Lineage package Admin

-include ../make.frag
	
package: release.tar.gz

release.tar.gz: $(PACKAGE_FILES)
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to package folder"
	rm -f ./client/std* ./client/*.exe
	cp -f $(PACKAGE_FILES) ./client/
	tar -zcvf release.tar.gz client
