#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <glib.h>

struct keyboard_layout {
  GHashTable *name_ht;
  gint button_width;
  gint button_height;
};

struct button_pos {
  gint x, y;
};

void load_keyboard (struct keyboard_layout *layout);
void free_keyboard (struct keyboard_layout *layout);

#endif
