#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([idiot], [0.1], [jy.park@piolink.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_SILENT_RULES([yes])

AM_MAINTAINER_MODE

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Checks for programs.
LT_INIT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET

Checks for libraries.
GLIB_REQUIRED=2.38.0
AM_PATH_GLIB_2_0($GLIB_REQUIRED,,,gobject gmodule gio gio-unix)
if test "$GLIB_LIBS" = ""; then
  AC_MSG_ERROR(GLIB $GLIB_REQUIRED or later is required to build cbroker)
fi
GLIB_CFLAGS="$GLIB_CFLAGS -DGLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_38"

GLIB_MAKEFILE='$(top_srcdir)/Makefile.glib'
AC_SUBST(GLIB_MAKEFILE)

GLIB_TESTS

# Checks for libraries.
PKG_CHECK_MODULES([MOSQUITTO], [libmosquitto >= 2.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
