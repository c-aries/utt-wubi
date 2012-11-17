AM_CFLAGS = -Werror -Wall
AM_CPPFLAGS = -I$(top_srcdir)/include

moduledir = $(libdir)/utt/module/wubi
module_LTLIBRARIES = \
	jianma.la wubi.la zigen.la

# learn from pidgin, FIXME: how to export .so only?
jianma_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
jianma_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/include
jianma_la_SOURCES = \
	jianma.c

wubi_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
wubi_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/include
wubi_la_SOURCES = \
	wubi.c

zigen_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
zigen_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-I$(top_srcdir)/include
zigen_la_SOURCES = \
	zigen.c
