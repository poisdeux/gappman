#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman], [http://code.google.com/p/gappman/])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for programs
AC_PATH_PROG(DOXYGEN, doxygen)
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], AC_CONFIG_FILES([docs/doxygen/Makefile docs/doxygen/Doxyfile]))

# Generic options
AC_ARG_ENABLE([building-applets], 
	[AS_HELP_STRING([--enable-building-applets],
		[enable building the applets @<:@default: yes@:>@])],
	[building_applets=${enableval}], [building_applets=yes])

AC_ARG_ENABLE([building-libs], 
	[AS_HELP_STRING([--enable-building-libs],
		[enable building the libs @<:@default: yes@:>@])],
	[building_libs=${enableval}], [building_libs=yes])

AC_ARG_ENABLE([building-gappman], 
	[AS_HELP_STRING([--enable-building-gappman],
		[enable building gappman @<:@default: yes@:>@])],
	[building_gappman=${enableval}], [building_gappman=yes])

AM_CONDITIONAL([BUILD_GAPPMAN], [test "x$building_gappman" == "xyes"])
AM_COND_IF([BUILD_GAPPMAN], AC_CONFIG_SUBDIRS([appmanager]))

AM_CONDITIONAL([BUILD_LIBS], [test "x$building_libs" == "xyes"])
AM_COND_IF([BUILD_LIBS], AC_CONFIG_SUBDIRS([libs]))

AM_CONDITIONAL([BUILD_APPLETS], [test "x$building_applets" == "xyes"])
AM_COND_IF([BUILD_APPLETS], AC_CONFIG_SUBDIRS([applets]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
