#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([utt-wubi], [0.2], [babyaries2@gmail.com])
AC_CONFIG_SRCDIR([src/utt_article.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# gettext
IT_PROG_INTLTOOL([0.40.6])
GETTEXT_PACKAGE=${PACKAGE_NAME}
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
                                    [The prefix for our gettext translation domains.])

# libtool
AC_PROG_LIBTOOL
LT_PREREQ([2.2.10])
LT_INIT()

# Checks for libraries.
# gtk+, Debian 6.0
#GTK_MODULES="gtk+-2.0 >= 2.20.1 glib-2.0 >= 2.24.2"
# port for Slackware 13.1
#GTK_MODULES="gtk+-2.0 >= 2.18.9 glib-2.0 >= 2.22.5"
AM_PATH_GTK_2_0([2.18.9],,[AC_MSG_ERROR([${PACKAGE_NAME} require gtk+-2.0])])
AM_PATH_GLIB_2_0([2.22.5],,[AC_MSG_ERROR([${PACKAGE_NAME} require glib-2.0])])
AM_PATH_XML2([2.7.6],,[AC_MSG_ERROR([${PACKAGE_NAME} require libxml-2.0])])

UUID_MODULES="uuid >= 2.17.0"
PKG_CHECK_MODULES(UUID,$UUID_MODULES)
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)

GCONF2_MODULES="gconf-2.0 >= 2.28.1"
PKG_CHECK_MODULES(GCONF2,$GCONF2_MODULES)
AC_SUBST(GCONF2_CFLAGS)
AC_SUBST(GCONF2_LIBS)

# custom utt parameter
UTT_MODULE_PATH=${libdir}/utt/module
AC_SUBST(UTT_MODULE_PATH)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
		libutt.pc
		po/Makefile.in
		data/Makefile
		data/utt.desktop.in
		data/utt-wubi.desktop.in
		data/zigen/Makefile
		data/zigen/png/Makefile
		data/icons/Makefile
		data/icons/48x48/Makefile
		data/icons/48x48/apps/Makefile
		data/article/Makefile
		data/artwork/Makefile
		data/artwork/keyboard/Makefile
		module/Makefile
		module/wubi/Makefile
		module/english/Makefile
		module/pinyin/Makefile
		libutt/Makefile
		include/Makefile
		include/utt/Makefile
		src/Makefile])
AC_OUTPUT
