#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([gappman-main], [TRUNKMUSTNOTGENERATEPACKAGE], [m.brekhof@gmail.com], [gappman-main], [http://code.google.com/p/gappman/])
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
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, "yes", "no",)
if test "$HAVE_PKG_CONFIG" = "no" ; then
  AC_MSG_ERROR([Could not find required program pkg-config.])
fi

# Configure variables
AC_ARG_VAR([LIBGM], [The PATH wherein the gm libraries can be found])

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

AC_SEARCH_LIBS([g_thread_init], [gthread-2.0], [
AC_SUBST([GTHREAD_INCLUDES],[`pkg-config --cflags gthread-2.0`])
AC_SUBST([GTHREAD_OBJS],[`pkg-config --libs gthread-2.0`])
], [
AC_MSG_ERROR([Error: gtkthread-2.0 not found!])
], [])

#Search for libgm
if test -z "$LIBGM"
then
	#user did not specify path so we start
	#our search. First see if the libs are
	#available in the project
	if test	-d "$srcdir/../libs/layout" && \
	   test	-d "$srcdir/../libs/parseconf"
	then
		LIBGM='$(top_srcdir)/../libs'
	fi
fi

#If LIBGM is defined we need to make sure the compiler knows
#where to find the header and library files.
if test -n "$LIBGM"
then
	AC_SUBST([GM_INCLUDES], ["-I$LIBGM/generic -I$LIBGM/layout -I$LIBGM/parseconf"])
	AC_SUBST([GM_OBJS], ["$LIBGM/layout/libgm_layout.la $LIBGM/parseconf/libgm_parseconf.la"])
fi

#If LIBGM is still empty we check for installed libgm
if test -z "$LIBGM"
then
	AC_CHECK_LIB([gm_connect], [gm_connect_to_gappman],
		[AC_CHECK_HEADERS([gm_connect.h])
		  LIBS="-lgm_connect $LIBS"],
		[AC_MSG_ERROR([No libgm_connect found])])

	AC_CHECK_LIB([gm_layout], [gm_create_button],
		[AC_CHECK_HEADERS([gm_layout.h gm_changeresolution.h])
		  LIBS="-lgm_layout $LIBS"],
		[AC_MSG_ERROR([No libgm_layout found])])

	AC_CHECK_LIB([gm_parseconf], [gm_load_conf],
		[AC_CHECK_HEADERS([gm_parseconf.h])
		  LIBS="-lgm_parseconf $LIBS"],
		[AC_MSG_ERROR([No libgm_parseconf found])])
fi

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
Package features:
  listener disabled: ${disable_listener}

Now type 'make @<:@<target>@:>@'
  where the optional <target> can be:
    all         - build all binaries
    install     - install everything
    check       - create and perform tests

"

