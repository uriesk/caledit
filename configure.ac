AC_INIT([caledit], [0.3], [uriesk@posteo.de])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CXX
AC_ARG_ENABLE([gui],[AS_HELP_STRING([--disable-gui],[Disable GUI support])],[:],[enable_gui=check])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  scripts/Makefile
])

# Teste ob GUI deaktiviert werden soll:
AS_IF([test "$enable_gui" != "no"],
  [PKG_CHECK_MODULES([GTKMM],[gtkmm-3.0],[enable_gui=yes],
    [AS_IF([test "$enable_gui" = "yes"],[AC_MSG_ERROR([gtk+-3.0 required, but not found.])],[enable_gui=no])])])
# Weitergabe an Automake und config.h:
AM_CONDITIONAL([ENABLE_GUI],[test "$enable_gui" = "yes"])
AS_IF([test "$enable_gui" = "yes"],[AC_DEFINE([ENABLE_GUI],[],[Enable GUI])])

AC_OUTPUT
