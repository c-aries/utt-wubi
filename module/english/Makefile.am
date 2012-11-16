AM_CFLAGS = -Werror -Wall
AM_CPPFLAGS = -I$(top_srcdir)/include

moduledir = $(libdir)/utt/module/english
module_LTLIBRARIES = \
	keyboard.la english.la

keyboard_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
keyboard_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/include
keyboard_la_SOURCES = \
	keyboard.c

english_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS)
english_la_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(top_srcdir)/include
english_la_SOURCES = \
	english.c


