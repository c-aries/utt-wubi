SUBDIRS = data po module libutt include src

ACLOCAL_AMFLAGS = -I m4

doc_DATA = COPYING AUTHORS

EXTRA_DIST = \
	$(doc_DATA) \
	libutt.pc.in
	HACKING BUG README TODO

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libutt.pc

distclean-local:
	find -name "*~" -print0 | xargs -0 rm -f

.PHONY: distclean-local
