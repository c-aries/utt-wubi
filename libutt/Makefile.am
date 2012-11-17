AM_CFLAGS = -Werror -Wall

lib_LTLIBRARIES = libutt.la

libutt_la_SOURCES = \
	module.c keyboard.c
libutt_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-DUTT_MODULE_PATH=\"$(UTT_MODULE_PATH)\" \
	$(GTK_CFLAGS)
libutt_la_LDFLAGS = \
	--expot-dynamic \
	$(GTK_LIBS)
