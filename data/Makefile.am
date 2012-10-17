SUBDIRS = zigen icons article

desktopdir = $(datadir)/applications
desktop_in_files = utt-wubi.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

imagesdir = $(pkgdatadir)
images_DATA = \
	button.png \
	dashboard.png \
	keyboard_wubi.png \
	keyboard_yiji.png

tablesdir = $(pkgdatadir)
tables_DATA = \
	Wubi.txt

EXTRA_DIST = \
	$(images_DATA) \
	$(tables_DATA) \
        $(desktop_in_files)

CLEANFILES = \
	$(desktop_DATA)

DISTCLEANFILES = \
        $(desktop_DATA)
