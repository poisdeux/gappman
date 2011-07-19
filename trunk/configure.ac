#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman], [0.4.2-dev], [m.brekhof@gmail.com], [gappman], [http://code.google.com/p/gappman/])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for commandline options
AC_ARG_ENABLE([debug],
 [AS_HELP_STRING([--enable-debug], [enables debug mode @<:@default: no@:>@])],
 [enable_debug=no], [enable_debug=yes])

AC_ARG_ENABLE([listener],
 [AS_HELP_STRING([--disable-listener], [disable listener support @<:@default: no@:>@])],
 [disable_listener=yes], [disable_listener=no])

AC_ARG_ENABLE([dbus],
 [AS_HELP_STRING([--disable-dbus], [disable dbus support. This will enable the socket version.])],
 [without_dbus=yes], [without_dbus=no])

# Generic options
AC_ARG_ENABLE([doxygen-docs], 
	[AS_HELP_STRING([--disable-doxygen-docs],
		[disable building the doxygen documentation])],
	[doxygen_docs=no], [doxygen_docs=yes])

AC_ARG_ENABLE([building-applets], 
	[AS_HELP_STRING([--disable-building-applets],
		[disable building the applets])],
	[building_applets=no], [building_applets=yes])

AC_ARG_ENABLE([building-libs], 
	[AS_HELP_STRING([--disable-building-libs],
		[disable building the libs])],
	[building_libs=no], [building_libs=yes])

AC_ARG_ENABLE([building-gappman], 
	[AS_HELP_STRING([--disable-building-gappman],
		[disable building gappman])],
	[building_gappman=no], [building_gappman=yes])

AM_CONDITIONAL([BUILD_GAPPMAN], [test "x$building_gappman" == "xyes"])
AM_COND_IF([BUILD_GAPPMAN], AC_CONFIG_SUBDIRS([appmanager]))

AM_CONDITIONAL([BUILD_LIBS], [test "x$building_libs" == "xyes"])
AM_COND_IF([BUILD_LIBS], AC_CONFIG_SUBDIRS([libs]))

AM_CONDITIONAL([BUILD_APPLETS], [test "x$building_applets" == "xyes"])
AM_COND_IF([BUILD_APPLETS], AC_CONFIG_SUBDIRS([applets]))

# Check for programs if doxygen docs should be build
AS_IF([test "x$doxygen_docs" == "xyes"],
  [AC_PATH_PROG(DOXYGEN, doxygen)
    AS_IF([test -z "$DOXYGEN"], AC_MSG_WARN(Doxygen not found. Doxygen documentation will not be built))
    AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
    AM_COND_IF([HAVE_DOXYGEN], 
      AC_CONFIG_FILES([docs/doxygen/Makefile docs/doxygen/Doxyfile]))])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
