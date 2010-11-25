#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman-applets], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman-applets], [http://code.google.com/p/gappman/])
AC_CONFIG_SRCDIR([netman/applet/nm_main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, "yes", "no",)
if test "$HAVE_PKG_CONFIG" = "no" ; then
  AC_MSG_ERROR([Could not find required program pkg-config.])
fi

# Checks for libraries.

AC_SEARCH_LIBS(dbus_g_bus_get, dbus-glib-1, [
AC_SUBST([DBUS_INCLUDES],[`pkg-config --cflags dbus-glib-1`])
AC_SUBST([DBUS_OBJS],[`pkg-config --libs dbus-glib-1`])
], [
AC_MSG_ERROR([Error: dbus-glib-1 not found!])
], [])

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

AC_CONFIG_FILES([Makefile
                 changeresolution/Makefile
                 netman/Makefile
                 netman/applet/Makefile
                 netman/daemon/Makefile
                 netman/daemon/introspection/Makefile
                 processmanager/Makefile
                 shutdown/Makefile])
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

