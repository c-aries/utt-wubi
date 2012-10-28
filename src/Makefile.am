AM_CFLAGS = -Werror -Wall

bin_PROGRAMS = utt-wubi utt

utt_wubi_SOURCES = \
	keyboard.c keyboard.h \
	utt_article.c utt_article.h \
	uttclassrecord.c uttclassrecord.h \
	utt_dashboard.c utt_dashboard.h \
	utt_plugin.c utt_plugin.h \
	utttextarea.c utttextarea.h \
	utt-wubi.c utt_wubi.h \
	utt_xml.c utt_xml.h \
	wubi_zigen.c wubi_jianma.c wubi_wenzhang.c \
	wubi_class.c wubi_class.h \
	wubi_table.c wubi_table.h \
	wubi_article.c \
	zigen_images.c zigen_images.h \
	utt_compat.h common.h

utt_wubi_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(UUID_CFLAGS) \
	$(XML_CPPFLAGS) \
	$(GCONF2_CFLAGS) \
	-DDATAROOTDIR=\"$(datarootdir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(localedir)\"

utt_wubi_LDFLAGS = \
	$(GTK_LIBS) \
	$(GCONF2_LIBS) \
	$(UUID_LIBS) \
	$(XML_LIBS)

utt_SOURCES = \
	utt.c

utt_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-DDATAROOTDIR=\"$(datarootdir)\" \
	-DLOCALEDIR=\"$(localedir)\"

utt_LDFLAGS = \
	$(GTK_LIBS)
