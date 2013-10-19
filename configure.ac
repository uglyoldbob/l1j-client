AC_INIT([l1j_client], [0.1])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([ac_config.h:config.h.in])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

m4_include([m4/libs.m4])

#BUILD_FROM = system doing the compiling
#BUILD_FOR  = system the program will run on

AC_ARG_ENABLE([debug],
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

#detect os so the correct options can be setup for
#the expected build configuration
AC_CANONICAL_HOST
case $build_os in
  darwin* )
	#mac
	BUILD_FROM="MAC"
	AC_MSG_NOTICE("Building on mac")
	AC_MSG_ERROR("library specs for wxwidgets unknown")
	;;
  linux* )
	#linux
	BUILD_FROM="LINUX"
	AC_MSG_NOTICE("Building on linux")
	;;
  * )
	#unknown (probably broken)
	BUILD_FROM="BROKEN"
	AC_MSG_ERROR("Possibly invalid build system $build_os")
	;;
esac

AC_CHECK_LIB([SDL_mixer], [Mix_Init], [], [AC_MSG_ERROR("SDL_mixer Library not found")])
AC_CHECK_LIB([SDL_image], [IMG_Init], [], [AC_MSG_ERROR("SDL_image Library not found")])
AC_CHECK_LIB([SDL], [SDL_Init], [], [AC_MSG_ERROR("SDL_Init Library not found")])

case $host_os in
  darwin* )
	#mac
	BUILD_FOR="MAC"
	AC_DEFINE(MAC, [1], [Enable code specific to mac])
	AC_MSG_NOTICE("Building for mac")
	;;
  linux* )
	BUILD_FOR="LINUX"
	AC_DEFINE(LINUX, [1], [Enable code specific to linux])
	AC_MSG_NOTICE("Building for linux")
	;;
  mingw32* | mingw32msvc* )
	BUILD_FOR="WINDOWS"
	AC_DEFINE(WINDOWS, [1], [Enable code specific to windows])
	USE_DLLS="mingwm10.dll"	
	AC_CHECK_LIB([SDLmain], [main], [], [AC_MSG_ERROR("SDLmain Library not found")])
	AC_CHECK_LIB([ws2_32], [puts], [], [AC_MSG_ERROR("ws2_32 Library not found")])
	AC_MSG_NOTICE("Building for windows")
	;;
  * )
	BUILD_FOR="BROKEN"
	AC_MSG_ERROR("Possibly invalid build target $host_os")
	;;
esac

AC_SUBST(BUILD_FOR)
AC_SUBST(BUILD_FROM)

AM_CONDITIONAL([WINDOWS], [test \"$BUILD_FOR\" = \"WINDOWS\"])
AM_CONDITIONAL([LINUX], [test \"$BUILD_FOR\" = \"LINUX\"])

AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
