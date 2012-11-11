AM_CFLAGS = -Werror -Wall
AM_CPPFLAGS = -I$(top_srcdir)/include

moduledir = $(libdir)/utt/module/wubi
module_LTLIBRARIES = \
	jianma.la wubi.la zigen.la

# learn from pidgin, FIXME: how to export .so only?
jianma_la_LDFLAGS = \
	-module -avoid-version \
	$(GLIB_LDFLAGS)
jianma_la_CPPFLAGS = \
	$(GLIB_CFLAGS)
jianma_la_SOURCES = \
	jianma.c

wubi_la_LDFLAGS = -module -avoid-version
wubi_la_SOURCES = \
	wubi.c

zigen_la_LDFLAGS = \
	-module -avoid-version \
	$(GLIB_LDFLAGS)
zigen_la_CPPFLAGS = \
	$(GLIB_CFLAGS)
zigen_la_SOURCES = \
	zigen.c

