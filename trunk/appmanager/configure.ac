#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman-main], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman-main], [http://code.google.com/p/gappman/])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM
AC_CONFIG_SRCDIR([appmanager.c])
AC_CONFIG_HEADERS([config.h])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_USE_SYSTEM_EXTENSIONS

# Checks for commandline options
AC_ARG_ENABLE([listener],
 [AS_HELP_STRING([--disable-listener], [disable listener support @<:@default: no@:>@])],
 [disable_listener=yes], [disable_listener=no])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.22])

# Checks for libraries.
PKG_CHECK_MODULES([LIBXML],[libxml-2.0 >= 2.6.32])
PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.12.12])
PKG_CHECK_MODULES([GLIB],[gthread-2.0 >= 2.16.6])

#Search for gmlib and configure GM_INCLUDES, GM_OBJS or LIBS 
GMLIB_SEARCH

# Checks for header files.
AC_PATH_X
if test "${disable_listener}" = no; then
	AC_CHECK_HEADERS([netdb.h netinet/in.h sys/socket.h], [], [disable_listener=yes])
fi

if test "${disable_listener}" = yes; then
	AC_MSG_WARN([
------------------------------------------
Building gappman without listener support.
Some applets may not work correctly. 
------------------------------------------
])
AC_DEFINE([NO_LISTENER], 1, [listener support disabled])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
#AC_CHECK_FUNCS([gethostbyname localtime_r memset pow socket sqrt strndup strspn strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo \
"
Configured ${PACKAGE_NAME} ${PACKAGE_VERSION}

prefix:       ${prefix}
compiler:     ${CC} ${CFLAGS} ${CPPFLAGS}
GM Libraries: ${GM_OBJS}
GM Includes:  ${GM_INCLUDES}
GTK Libraries: ${GTK_LIBS}
GTK Includes: ${GTK_CFLAGS}
GLIB Libraries: ${GLIB_LDFLAGS}
GLIB Includes: ${GLIB_CFLAGS}
XML Libraries: ${LIBXML_LDFLAGS}
XML Includes: ${LIBXML_CFLAGS}

Package features:
  listener disabled: ${disable_listener}

Now type 'make @<:@<target>@:>@'
  where the optional <target> can be:
    all         - build all binaries
    install     - install everything
    check       - create and perform tests

"

