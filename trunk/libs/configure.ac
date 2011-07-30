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
AC_ARG_ENABLE([debug],
 [AS_HELP_STRING([--enable-debug], [enables debug mode @<:@default: no@:>@])],
 [enable_debug=yes], [enable_debug=no])

AC_ARG_ENABLE([listener],
 [AS_HELP_STRING([--disable-listener], [disable listener support @<:@default: no@:>@])],
 [disable_listener=yes], [disable_listener=no])

AC_ARG_ENABLE([dbus],
 [AS_HELP_STRING([--disable-dbus], [disable dbus support. This will enable the socket version.])],
 [without_dbus=yes], [without_dbus=no])

# Checks for libraries.
AS_IF([test "$without_dbus" != "yes" ],
[PKG_CHECK_MODULES([DBUS],[dbus-glib-1 >= 0.76], [without_dbus=no], [without_dbus=yes])],
[AC_MSG_WARN([Building gappman without dbus support])])

AS_IF([test "$without_dbus" != "yes" ],
[AC_DEFINE([WITH_DBUS_SUPPORT], [1], [Define if you want gappman compiled with dbus support])]
)
AM_CONDITIONAL([WITH_DBUS_SUPPORT], [test "$without_dbus" != "yes"])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.22])

# Checks for libraries.
PKG_CHECK_MODULES([LIBXML],[libxml-2.0 >= 2.6.32])
PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.12.12])
PKG_CHECK_MODULES([XRANDR],[xrandr >= 1.2.3])

# Checks for header files.
AC_PATH_X
AS_IF([test "x${disable_listener}" == "xno"],
	AC_CHECK_HEADERS([netdb.h netinet/in.h sys/socket.h], [], [disable_listener=yes]))

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyname localtime_r memset pow socket sqrt strndup strspn strstr])

AS_IF([test "x${disable_listener}" == "xyes"],
	AC_MSG_WARN([
------------------------------------------
Building gappman-libs without listener support.
Some applets may not work correctly. 
------------------------------------------
])
AC_DEFINE([NO_LISTENER], [1], [listener support disabled]))

AS_IF([test "x${enable_debug}" == "xyes"],
  AC_MSG_WARN([
------------------------------------------
Building debug version.
This adds additional code.
------------------------------------------
])
AC_DEFINE([DEBUG], [1], [debug code enabled]))

AC_CONFIG_FILES([Makefile
                 network/Makefile
                 network/libgm_network.pc
                 generic/Makefile
                 layout/Makefile
                 parseconf/Makefile])
AC_OUTPUT

echo \
"
Configured ${PACKAGE_NAME} ${PACKAGE_VERSION}

prefix: ${prefix}
compiler: ${CC} ${CFLAGS} ${CPPFLAGS}
GTK Libraries: ${GTK_LIBS}
GTK Includes: ${GTK_CFLAGS}
XML Libraries: ${LIBXML_LIBS}
XML Includes: ${LIBXML_CFLAGS}
XRANDR Libraries: ${XRANDR_LIBS}
XRANDR Includes: ${XRANDR_CFLAGS}

Package features:
  listener disabled: ${disable_listener}
  dbus support disabled: ${without_dbus}
  debugging code enabled: ${enable_debug}

Now type 'make @<:@<target>@:>@'
  where the optional <target> can be:
    all         - build all binaries
    install     - install everything
    check       - create and perform tests

"

