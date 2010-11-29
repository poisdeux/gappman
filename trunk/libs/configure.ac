#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman-libs], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman-libs], [http://code.google.com/p/gappman/])
AC_CONFIG_SRCDIR([layout/gm_layout.c])
AC_CANONICAL_SYSTEM
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_USE_SYSTEM_EXTENSIONS

# Checks for commandline options
AC_ARG_ENABLE([listener],
 [AS_HELP_STRING([--disable-listener], [disable listener support @<:@default: no@:>@])],
 [disable_listener=yes], [disable_listener=no])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, "yes", "no",)
if test "$HAVE_PKG_CONFIG" = "no" ; then
  AC_MSG_ERROR([Could not find required program pkg-config.])
fi


# Checks for libraries.
AC_SEARCH_LIBS(xmlTextReaderRead, xml2, [
AC_SUBST([LIBXML_INCLUDES],[`pkg-config --cflags libxml-2.0`])
AC_SUBST([LIBXML_OBJS],[`pkg-config --libs libxml-2.0`])
], [
AC_MSG_ERROR([Error: libxml2 not found!])
], [])

AC_SEARCH_LIBS([gtk_main], [gtk-x11-2.0], [
AC_SUBST([GTK_INCLUDES],[`pkg-config --cflags gtk+-2.0`])
AC_SUBST([GTK_OBJS],[`pkg-config --libs gtk+-2.0`])
], [
AC_MSG_ERROR([Error: gtk+-2.0 not found!])
], [])

AC_SEARCH_LIBS([XRRSetScreenConfigAndRate], [Xrandr], [
AC_SUBST([XRANDR_INCLUDES], [`pkg-config --cflags xrandr`])
AC_SUBST([XRANDR_OBJS],[`pkg-config --libs xrandr`])
], [
AC_MSG_ERROR([Error: libXrandr not found!])
], [])

# Checks for header files.
AC_PATH_X
if test "${disable_listener}" = no; then
	AC_CHECK_HEADERS([netdb.h netinet/in.h sys/socket.h], [], [disable_listener=yes])
fi

if test "${disable_listener}" = yes; then
	AC_MSG_WARN([
------------------------------------------
Building gappman-libs without listener support.
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
AC_CHECK_FUNCS([gethostbyname localtime_r memset pow socket sqrt strndup strspn strstr])

AC_CONFIG_FILES([Makefile
                 connect/Makefile
                 generic/Makefile
                 layout/Makefile
                 parseconf/Makefile])
AC_OUTPUT

echo \
"
Configured ${PACKAGE_NAME} ${PACKAGE_VERSION}

prefix: ${prefix}
compiler: ${CC} ${CFLAGS} ${CPPFLAGS}

Package features:
  listener disabled: ${disable_listener}

Now type 'make @<:@<target>@:>@'
  where the optional <target> can be:
    all         - build all binaries
    install     - install everything
    check       - create and perform tests

"

