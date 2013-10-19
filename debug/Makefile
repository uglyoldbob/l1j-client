EXTRA_FLAGS=-g
	
PACKAGE_FILES += $(OUTPUT)run.bat

all: Lineage.exe Admin.exe package

-include ../make.frag

package: $(PACKAGE_FILES)
	@if [ ! -d client ]; then\
		echo mkdir client;\
		mkdir client;\
	fi
	@echo "Copying files to package folder"
	cp -f $(PACKAGE_FILES) ./client/
	touch ./client/DEBUG_DO_NOT_RELEASE.txt
