AM_CFLAGS = -Werror -Wall

bin_PROGRAMS = utt-wubi

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
	zigen_images.c zigen_images.h

utt_wubi_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(UUID_CFLAGS) \
	$(LIBXML2_CFLAGS) \
	$(GCONF2_CFLAGS) \
	-DDATAROOTDIR=\"$(datarootdir)\" \
	-DPKGDATADIR=\"$(pkgdatadir)\"

utt_wubi_LDFLAGS = \
	$(GTK_LIBS) \
	$(GCONF2_LIBS) \
	$(UUID_LIBS) \
	$(LIBXML2_LIBS)
