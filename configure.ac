#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([utt-wubi], [0.1], [babyaries2@gmail.com])
AC_CONFIG_SRCDIR([src/utt_article.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# gettext
IT_PROG_INTLTOOL([0.41.1])
GETTEXT_PACKAGE=${PACKAGE_NAME}
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
                                    [The prefix for our gettext translation domains.])

# libtool
AC_PROG_LIBTOOL
LT_PREREQ([2.2.6])
LT_INIT()

# Checks for libraries.
# gtk+, Debian 6.0
GTK_MODULES="gtk+-2.0 >= 2.20.1 glib-2.0 >= 2.24.2"
PKG_CHECK_MODULES(GTK,$GTK_MODULES)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

UUID_MODULES="uuid >= 2.17.0"
PKG_CHECK_MODULES(UUID,$UUID_MODULES)
AC_SUBST(UUID_CFLAGS)
AC_SUBST(UUID_LIBS)

LIBXML2_MODULES="libxml-2.0 >= 2.7.8"
PKG_CHECK_MODULES(LIBXML2,$LIBXML2_MODULES)
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

GCONF2_MODULES="gconf-2.0 >= 2.28.1"
PKG_CHECK_MODULES(GCONF2,$GCONF2_MODULES)
AC_SUBST(GCONF2_CFLAGS)
AC_SUBST(GCONF2_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
		src/Makefile
		po/Makefile.in
		data/Makefile
		data/utt-wubi.desktop.in
		data/zigen/Makefile
		data/zigen/png/Makefile
		data/icons/Makefile
		data/icons/48x48/Makefile
		data/icons/48x48/apps/Makefile])
AC_OUTPUT