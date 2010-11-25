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
           test -d "$srcdir/../libs/parseconf"
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
        AC_CHECK_LIB([gm_layout], [gm_create_button],
                [AC_CHECK_HEADERS([gm_layout.h gm_changeresolution.h])
		  LIBGM="-lgm_layout"
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
