AM_CFLAGS = -Werror -Wall
AM_CPPFLAGS = -I$(top_srcdir)/include

moduledir = $(libdir)/utt/module/pinyin
module_LTLIBRARIES = \
	pinyin.la

# learn from pidgin, FIXME: how to export .so only?
pinyin_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
pinyin_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/include
pinyin_la_SOURCES = \
	pinyin.c
