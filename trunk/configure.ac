#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman], [http://code.google.com/p/gappman/])
AC_CONFIG_MACRO_DIR([m4])

# Pass generic make options to all makefiles
AM_INIT_AUTOMAKE([-Wall -Werror])

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
AC_SUBST(GTK_INCLUDES)
AC_SUBST(GTK_OBJS)
AC_SUBST(GTHREAD_INCLUDES)
AC_SUBST(GTHREAD_OBJS)
AC_SUBST(LIBXML_INCLUDES)
AC_SUBST(LIBXML_OBJS)
AC_SUBST(DBUS_INCLUDES)
AC_SUBST(DBUS_OBJS)
AC_SUBST(XRANDR_INCLUDES)
AC_SUBST(XRANDR_OBJS)

AC_SEARCH_LIBS(dbus_g_bus_get, dbus-glib-1, [
DBUS_INCLUDES=`pkg-config --cflags dbus-glib-1`
DBUS_OBJS=`pkg-config --libs dbus-glib-1`
], [
AC_MSG_ERROR([Error: dbus-glib-1 not found!])
], [])

AC_SEARCH_LIBS(xmlTextReaderRead, xml2, [
LIBXML_INCLUDES=`pkg-config --cflags libxml-2.0`
LIBXML_OBJS=`pkg-config --libs libxml-2.0`
], [
AC_MSG_ERROR([Error: libxml2 not found!])
], [])

AC_SEARCH_LIBS([gtk_main], [gtk-x11-2.0], [
GTK_INCLUDES=`pkg-config --cflags gtk+-2.0`
GTK_OBJS=`pkg-config --libs gtk+-2.0`
], [
AC_MSG_ERROR([Error: gtk+-2.0 not found!])
], [])

AC_SEARCH_LIBS([g_thread_init], [gthread-2.0], [
GTHREAD_INCLUDES=`pkg-config --cflags gthread-2.0`
GTHREAD_OBJS=`pkg-config --libs gthread-2.0`
], [
AC_MSG_ERROR([Error: gtkthread-2.0 not found!])
], [])

AC_SEARCH_LIBS([XRRSetScreenConfigAndRate], [Xrandr], [
XRANDR_INCLUDES=`pkg-config --cflags xrandr`
XRANDR_OBJS=`pkg-config --libs xrandr`
], [
AC_MSG_ERROR([Error: libXrandr not found!])
], [])

# Checks for header files.
AC_PATH_X
if test "${disable_listener}" = no; then
	AC_CHECK_HEADERS([netdb.h netinet/in.h sys/socket.h], [], [disable_listener=yes])
fi

AS_IF([test "x${disable_listener}" = xyes],
  [AC_DEFINE([NO_LISTENER], [1], [listener support disabled])],
  [AC_MSG_WARN([
------------------------------------------
Building gappman without listener support.
Some applets may not work correctly.
------------------------------------------
])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname localtime_r memset pow socket sqrt strndup strspn strstr])

AC_CONFIG_FILES([Makefile
                 applets/Makefile
                 applets/changeresolution/Makefile
                 applets/netman/Makefile
                 applets/netman/applet/Makefile
                 applets/netman/daemon/Makefile
                 applets/netman/daemon/introspection/Makefile
                 applets/processmanager/Makefile
                 applets/shutdown/Makefile
                 libs/Makefile
                 libs/connect/Makefile
                 libs/generic/Makefile
                 libs/layout/Makefile
                 libs/parseconf/Makefile
                 appmanager/Makefile])
AC_OUTPUT
