#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman], [http://code.google.com/p/gappman/])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for programs
AC_PATH_PROG(DOXYGEN, doxygen)
AS_IF([ test -n $DOXYGEN ],
	AC_CONFIG_FILES([docs/doxygen/Makefile]))

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

AS_IF([ test "x$building_gappman" == "xyes" ],
	AC_CONFIG_SUBDIRS([appmanager]))

AS_IF([ test "x$building_applets" == "xyes" ],
	AC_CONFIG_SUBDIRS([applets]))

AS_IF([ test "x$building_libs" == "xyes" ],
	AC_CONFIG_SUBDIRS([libs]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
