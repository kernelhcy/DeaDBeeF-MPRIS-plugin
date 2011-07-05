dnl Process this file with autoconf to produce a configure script.
AC_INIT([deadbeef], [1.5], [huangcongyu2006@gmail.com])
AC_CONFIG_HEADER(config.h)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([deadbeef], [1.5], [huangcongyu2006@gmail.com])

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_PROG_CXX
AC_STDC_HEADERS

AC_CONFIG_MACRO_DIR([m4])
LT_INIT

AC_C_BIGENDIAN

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(GLIB_DEPS, glib-2.0 gio-2.0 gthread-2.0)
MPRIS_LIBS="$GLIB_DEPS_LIBS"
MPRIS_CFLAGS="$GLIB_DEPS_CFLAGS"
AC_SUBST(MPRIS_LIBS)
AC_SUBST(MPRIS_CFLAGS)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
