AC_INIT([l1j_client], [0.1])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

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

case $host_os in
  darwin* )
	#mac
	BUILD_FOR="MAC"
	AC_MSG_NOTICE("Building for mac")
	;;
  linux* )
	BUILD_FOR="LINUX"
	AC_MSG_NOTICE("Building for linux")
	;;
  mingw32* )
	BUILD_FOR="WINDOWS"
	USE_DLLS="mingwm10.dll"	
#	AC_MSG_ERROR("Library specs for wxwidgets unknown")
	AC_MSG_NOTICE("Building for windows")
	;;
  mingw32msvc* )
	BUILD_FOR="WINDOWS"
	USE_DLLS="mingwm10.dll"	
#	AC_MSG_ERROR("Library specs for wxwidgets unknown")
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
AC_MSG_NOTICE("Testing $BUILD_FOR")

AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
