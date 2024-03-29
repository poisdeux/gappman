#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman], [MASTER], [m.brekhof@gmail.com], [gappman], [http://code.google.com/p/gappman/])
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

#AC_ARG_ENABLE([building-tools], 
#	[AS_HELP_STRING([--disable-building-tools],
#		[disable building the tools])],
#	[building_tools=no], [building_tools=yes])

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

#AM_CONDITIONAL([BUILD_TOOLS], [test "x$building_tools" == "xyes"])
#AM_COND_IF([BUILD_TOOLS], AC_CONFIG_FILES([tools/Makefile]))

# Check for programs if doxygen docs should be build

AS_IF([test "x$doxygen_docs" == "xyes"],
  [AC_PATH_PROG(DOXYGEN, doxygen)
   AC_PATH_PROG(GRAPHVIZ, dot)
    AS_IF([test -z "$DOXYGEN"], AC_MSG_WARN(Doxygen not found. Doxygen documentation will not be built))
    AS_IF([test -z "$GRAPHVIZ"], AC_MSG_WARN(Graphviz not found. Doxygen documentation will not be built))
    AM_CONDITIONAL([BUILD_DOCS], [test -n "$DOXYGEN" -a -n "$GRAPHVIZ"])
    AM_COND_IF([BUILD_DOCS], 
      AC_CONFIG_FILES([docs/doxygen/Makefile docs/doxygen/Doxyfile]))])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
