AM_CFLAGS = -Werror -Wall
AM_CPPFLAGS = -I$(top_srcdir)/include

moduledir = $(libdir)/utt/module/wubi
module_LTLIBRARIES = \
	jianma.la

# learn from pidgin, FIXME: how to export .so only?
jianma_la_LDFLAGS = -module -avoid-version
jianma_la_SOURCES = \
	jianma.c
