dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([gappman], [0.4a], [m.brekhof@gmail.com])

AC_SUBST(GTK_INCLUDES)
AC_SUBST(GTK_OBJS)
AC_SUBST(GTHREAD_INCLUDES)
AC_SUBST(GTHREAD_OBJS)
AC_SUBST(LIBXML_INCLUDES)
AC_SUBST(LIBXML_OBJS)
AC_SUBST(XRANDR_INCLUDES)
AC_SUBST(XRANDR_OBJS)

AC_USE_SYSTEM_EXTENSIONS
AC_CHECK_FUNCS([localtime_r pow sqrt])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_STRUCT_TM
AC_PROG_CC_STDC

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
  test -f $top_builddir/configure && break
done
AC_PROG_LIBTOOL


AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, "yes", "no",)
if test "$HAVE_PKG_CONFIG" = "no" ; then
  AC_MSG_ERROR(== Could not find required program pkg-config.)
fi


AC_SEARCH_LIBS(xmlTextReaderRead, xml2, [
LIBXML_INCLUDES=`pkg-config --cflags libxml-2.0`
LIBXML_OBJS=`pkg-config --libs libxml-2.0`
], [
echo "Error: libxml2 not found!"; exit 1
], [])

AC_SEARCH_LIBS([gtk_main], [gtk-x11-2.0], [
GTK_INCLUDES=`pkg-config --cflags gtk+-2.0`
GTK_OBJS=`pkg-config --libs gtk+-2.0`
], [
echo "Error: gtk+-2.0 not found!"; exit 1
], [])

AC_SEARCH_LIBS([g_thread_init], [gthread-2.0], [
GTHREAD_INCLUDES=`pkg-config --cflags gthread-2.0`
GTHREAD_OBJS=`pkg-config --libs gthread-2.0`
], [
echo "Error: gtk+-2.0 not found!"; exit 1
], [])

AC_SEARCH_LIBS([XRRSetScreenConfigAndRate], [Xrandr], [
XRANDR_INCLUDES=`pkg-config --cflags --libs libxml-2.0`
XRANDR_OBJS=`pkg-config --libs --libs libxml-2.0`
], [
echo "Error: libXrandr not found!"; exit 1
], [])


AC_CONFIG_SRCDIR(src)

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC

AC_CONFIG_FILES([ 
Makefile
src/Makefile
libs/Makefile
libs/parseconf/Makefile
libs/layout/Makefile
libs/connect/Makefile
libs/generic/Makefile
applets/Makefile
applets/shutdown/Makefile
applets/processmanager/Makefile
applets/changeresolution/Makefile
applets/netman/Makefile
])

AC_OUTPUT
