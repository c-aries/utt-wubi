CFLAGS:=$(shell pkg-config --cflags gtk+-2.0) -Werror -Wall -g
LDFLAGS:=$(shell pkg-config --libs gtk+-2.0)
TARGET:= utt-wubi

all: $(TARGET)

utt-wubi: utt_wubi.h \
	keyboard.o wubi_class.o wubi_article.o zigen_images.o wubi_table.o \
	wubi_zigen.o wubi_jianma.o wubi_wenzhang.o \
	utttextarea.o utt_dashboard.o uttclassrecord.o \
	utt_plugin.o

utt_class.o: utt_class.c utt_class.h
wubi_zigen.o: wubi_zigen.c
wubi_jianma.o: wubi_jianma.c
wubi_wenzhang.o: wubi_wenzhang.c
class.o: class.c class.h
zigen_images.o: zigen_images.c zigen_images.h
wubi_class.o: wubi_class.c wubi_class.h
keyboard.o: keyboard.c keyboard.h
uttclassrecord.o: uttclassrecord.c uttclassrecord.h
wubi_table.o: wubi_table.c wubi_table.h
utt_plugin.o: utt_plugin.c utt_plugin.h

debug: test.o utttextarea.o class.o
	gcc -DDEBUG_MAIN $(CFLAGS) $(LDFLAGS) $^

clean:
	rm -f *~ *.o $(TARGET) a.out TAGS

TAGS:
	find -name "*.[ch]" | xargs ctags -e

.PHONY: all clean debug TAGS
