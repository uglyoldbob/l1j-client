#include "pack.h"
#include "table.h"

#include "SDL_endian.h"

//these are used so that the correct byte order is used
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif