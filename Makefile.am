SUBDIRS = src data po

ACLOCAL_AMFLAGS = -I m4

doc_DATA = COPYING AUTHORS

EXTRA_DIST = \
	$(doc_DATA) \
	HACKING BUG README TODO

distclean-local:
	find -name "*~" -print0 | xargs -0 rm

.PHONY: distclean-local