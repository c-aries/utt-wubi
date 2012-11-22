noinst_PROGRAMS = gen_keyboard

gen_keyboard_SOURCES = \
	gen_keyboard.c

gen_keyboard_CPPFLAGS = \
	$(GTK_CFLAGS)
gen_keyboard_LDFLAGS = \
	$(GTK_LIBS)

EXTRA_DIST = \
	backspace.jpeg \
	backspace.xcf \
	button.jpeg \
	button.xcf \
	capslock.jpeg \
	capslock.xcf \
	cursor.png \
	cursor_white.png \
	enter.jpeg \
	enter.xcf \
	gnu.png \
	keyboard_common.xcf \
	pop_white.png \
	pop.xcf \
	shift.jpeg \
	shift.xcf \
	space.jpeg \
	space.xcf \
	tab.jpeg \
	tab.xcf

distclean-local:
	rm -f keyboard.png

.PHONY: distclean-local
