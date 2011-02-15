# GMLIB_SEARCH
# ------------
# Define user variabel LIBGM which allows the user to specify the location 
# for libgm. If not specified the following locations will be searched:
#   1. ../libs/
#   2. Installed location of libgm on the system
# If found in 1. GM_INCLUDES and GM_OBJS will be defined pointing to the
# header and object (.la) files. If found in 2. the libs are included in LIBS.
AC_DEFUN([GMLIB_SEARCH],
[
AC_MSG_CHECKING([for location gmlib])
# Configure variables
AC_ARG_VAR([LIBGM], [The PATH wherein the gm libraries can be found])
#Search for libgm
if test -z "$LIBGM"
then
        #user did not specify path so we start
        #our search. First see if the libs are
        #available in the project
        if test -d "$srcdir/../libs/layout" && \
           test -d "$srcdir/../libs/parseconf" && \
           test -d "$srcdir/../libs/connect" && \
           test -d "$srcdir/../libs/generic"
        then
                LIBGM='$(top_srcdir)/../libs'
        fi
fi

#If LIBGM is defined we need to make sure the compiler knows
#where to find the header and library files.
if test -n "$LIBGM"
then
        AC_SUBST([GM_INCLUDES], ["-I$LIBGM/generic -I$LIBGM/layout -I$LIBGM/parseconf -I$LIBGM/connect"])
        AC_SUBST([GM_OBJS], ["$LIBGM/layout/libgm_layout.la $LIBGM/parseconf/libgm_parseconf.la $LIBGM/connect/libgm_connect.la"])
fi

#If LIBGM is still empty we check for installed libgm
if test -z "$LIBGM"
then
        AC_CHECK_LIB([gm_connect], [gm_connect_to_gappman],
                [AC_CHECK_HEADERS([gm_connect.h])
		  LIBGM="-lgm_connect"
                  LIBS="$LIBGM $LIBS"],
                [AC_MSG_ERROR([No libgm_connect found])])

        AC_CHECK_LIB([gm_layout], [gm_create_button],
                [AC_CHECK_HEADERS([gm_layout.h gm_changeresolution.h])
                  LIBGM="-lgm_layout",
                  LIBS="$LIBGM $LIBS"],
                [AC_MSG_ERROR([No libgm_layout found])])

        AC_CHECK_LIB([gm_parseconf], [gm_load_conf],
                [AC_CHECK_HEADERS([gm_parseconf.h])
                  LIBGM="-lgm_parseconf"
                  LIBS="$LIBGM $LIBS"],
                [AC_MSG_ERROR([No libgm_parseconf found])])
fi
AC_MSG_RESULT($LIBGM)
])
