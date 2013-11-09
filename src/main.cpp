#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <ac_config.h>
#endif

#ifdef MAC
#include <unistd.h>
#include "CoreFoundation/CoreFoundation.h"
#endif

#include "resources/music.h"
#include "sdl_master.h"
#include <SDL/SDL_mixer.h>

/**
This function is an osx specific function that changes the current directory
to the resources folder within the .app folder structure
This minimizes the changes required for the osx version when accessing resources
*/
void change_working_directory()
{
#ifdef MAC
	// ----------------------------------------------------------------------------
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		// error!
	}
	CFRelease(resourcesURL);
	chdir(path);
	printf("Current Path: %s\n", path);
#endif
}

/** Entry point of the program. Initializes SDL and starts the sdl_master object.
*/
int main(int argc, char* argv[])
{
	change_working_directory();
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Failed to start SDL\n");
		return 1;
	}
	
	SDL_EnableUNICODE(1);
	
	sdl_master graphics;
	graphics.create_client();
	graphics.process_events();
	SDL_VideoQuit();
	SDL_Quit();
	printf("Exiting now\n");
	return 0;
}
