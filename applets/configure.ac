#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman-applets], [0.5.0], [m.brekhof@gmail.com], [gappman-applets], [http://code.google.com/p/gappman/])
AC_CONFIG_SRCDIR([netman/main.c])
AC_CANONICAL_SYSTEM
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_USE_SYSTEM_EXTENSIONS

# Checks for commandline options
AC_ARG_ENABLE([debug],
 [AS_HELP_STRING([--enable-debug], [enables debug mode @<:@default: no@:>@])],
 [enable_debug=yes], [enable_debug=no])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, "yes", "no",)
if test "$HAVE_PKG_CONFIG" = "no" ; then
  AC_MSG_ERROR([Could not find required program pkg-config.])
fi

# Checks for libraries.
PKG_CHECK_MODULES([DBUS],[dbus-glib-1 >= 0.76])
PKG_CHECK_MODULES([LIBXML],[libxml-2.0 >= 2.6.32])
PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.12.12])
PKG_CHECK_MODULES([GLIB],[glib-2.0 >= 2.16.6])

GMLIB_SEARCH

# Checks for header files.
AC_PATH_X

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname localtime_r memset pow socket sqrt strndup strspn strstr])

AS_IF([test "x${enable_debug}" = "xyes"],
  AC_MSG_WARN([
------------------------------------------
Building debug version.
This adds additional code.
------------------------------------------
])
AC_DEFINE([DEBUG], 1, [debug code enabled]))

AC_CONFIG_FILES([Makefile
                 changeresolution/Makefile
                 netman/Makefile
                 netman/xml-config-files/Makefile
                 processmanager/Makefile
                 digitalclock/Makefile
                 shutdown/Makefile
								 shutdown/xml-config-files/Makefile])
AC_OUTPUT

echo \
"
Configured ${PACKAGE_NAME} ${PACKAGE_VERSION}

prefix: ${prefix}
compiler: ${CC} ${CFLAGS} ${CPPFLAGS}
GTK Libraries: ${GTK_LIBS}
GTK Includes: ${GTK_CFLAGS}
GLIB Libraries: ${GLIB_LIBS}
GLIB Includes: ${GLIB_CFLAGS}
XML Libraries: ${LIBXML_LIBS}
XML Includes: ${LIBXML_CFLAGS}
DBUS Libraries: ${DBUS_LIBS}
DBUS Includes: ${DBUS_CFLAGS}

Package features:
  debugging code enabled: ${enable_debug}

Now type 'make @<:@<target>@:>@'
  where the optional <target> can be:
    all         - build all binaries
    install     - install everything

"

