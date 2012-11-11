AM_CFLAGS = -Werror -Wall

lib_LTLIBRARIES = libutt.la

libutt_la_SOURCES = \
	module.c
libutt_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-DUTT_MODULE_PATH=\"$(UTT_MODULE_PATH)\"
libutt_la_LDFLAGS = --expot-dynamic
