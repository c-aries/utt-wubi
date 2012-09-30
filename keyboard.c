#include <glib.h>
#include <string.h>
#include <ctype.h>
#include "keyboard.h"

#define PAD 7
#define REL 6
#define BUTTON_WIDTH 53
#define BUTTON_HEIGHT 53
#define TAB_WIDTH (BUTTON_WIDTH + REL + (BUTTON_WIDTH - REL) / 2)
#define PICTURE_WIDTH (TAB_WIDTH * 2 + BUTTON_WIDTH * 12 + PAD * 2 + REL * 13)
#define PICTURE_HEIGHT (PAD * 2 + BUTTON_HEIGHT * 5 + REL * 4)
#define BACKSPACE_WIDTH (PICTURE_WIDTH - PAD * 2 - BUTTON_WIDTH * 13 - REL * 13)
#define CAPSLOCK_WIDTH (TAB_WIDTH + REL + BUTTON_WIDTH / 4 - REL / 2)
#define ENTER_WIDTH (PICTURE_WIDTH - PAD * 2 - CAPSLOCK_WIDTH - BUTTON_WIDTH * 11 - REL * 12)
#define SHIFT_WIDTH ((PICTURE_WIDTH - PAD * 2 - BUTTON_WIDTH * 10 - REL * 11) / 2)
#define CTRL_WIDTH TAB_WIDTH
#define ALT_WIDTH BUTTON_WIDTH
#define GNU_WIDTH BUTTON_WIDTH
#define SPACE_WIDTH (PICTURE_WIDTH - PAD * 2 - CTRL_WIDTH * 2 - ALT_WIDTH * 2 - GNU_WIDTH * 2 - REL * 6)

void
load_keyboard (struct keyboard_layout *layout)
{
  gchar *row_q[] = { "q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
  gchar *row_Q[] = { "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"};
  gchar *row_a[] = { "a", "s", "d", "f", "g", "h", "j", "k", "l"};
  gchar *row_A[] = { "A", "S", "D", "F", "G", "H", "J", "K", "L"};
  gchar *row_z[] = { "z", "x", "c", "v", "b", "n", "m"};
  gchar *row_Z[] = { "Z", "X", "C", "V", "B", "N", "M"};
  struct button_pos *pos;
  gint i;

  layout->name_ht = g_hash_table_new_full (g_str_hash, g_str_equal,
					   NULL, g_free);
  for (i = 0; i < G_N_ELEMENTS (row_q); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + TAB_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + BUTTON_HEIGHT + REL;
    g_hash_table_insert (layout->name_ht, row_q[i], pos);
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + TAB_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + BUTTON_HEIGHT + REL;
    g_hash_table_insert (layout->name_ht, row_Q[i], pos);
  }
  for (i = 0; i < G_N_ELEMENTS (row_a); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + CAPSLOCK_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 2;
    g_hash_table_insert (layout->name_ht, row_a[i], pos);
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + CAPSLOCK_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 2;
    g_hash_table_insert (layout->name_ht, row_A[i], pos);
  }
  for (i = 0; i < G_N_ELEMENTS (row_z); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + SHIFT_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 3;
    g_hash_table_insert (layout->name_ht, row_z[i], pos);
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + SHIFT_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 3;
    g_hash_table_insert (layout->name_ht, row_Z[i], pos);
  }
  layout->button_width = BUTTON_WIDTH;
  layout->button_height = BUTTON_HEIGHT;
}

void
free_keyboard (struct keyboard_layout *layout)
{
  g_hash_table_destroy (layout->name_ht);
  layout->name_ht = NULL;
}

#ifdef DEBUG_MAIN
/* static void */
/* print_once (gpointer key, gpointer value, gpointer user_data) */
/* { */
/*   struct button_pos *pos = value; */

/*   g_print ("%p %d %d\n", key, pos->x, pos->y); */
/* } */

int
main (int argc, char *argv[])
{
  gchar *row_q[] = { "q", "w", "e", "r", "t", "y", "u", "i", "o", "p"};
  gchar *row_a[] = { "a", "s", "d", "f", "g", "h", "j", "k", "l"};
  gchar *row_z[] = { "z", "x", "c", "v", "b", "n", "m"};
  GHashTable *key_name_ht;
  struct button_pos *pos;
  gint i;

  key_name_ht = g_hash_table_new_full (g_str_hash, g_str_equal,
				       NULL, g_free);
  for (i = 0; i < G_N_ELEMENTS (row_q); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + TAB_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + BUTTON_HEIGHT + REL;
    g_hash_table_insert (key_name_ht, row_q[i], pos);
  }
  for (i = 0; i < G_N_ELEMENTS (row_a); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + CAPSLOCK_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 2;
    g_hash_table_insert (key_name_ht, row_a[i], pos);
  }
  for (i = 0; i < G_N_ELEMENTS (row_z); i++) {
    pos = g_new (struct button_pos, 1);
    pos->x = PAD + SHIFT_WIDTH + REL + (BUTTON_WIDTH + REL) * i;
    pos->y = PAD + (BUTTON_HEIGHT + REL) * 3;
    g_hash_table_insert (key_name_ht, row_z[i], pos);
  }
  pos = g_hash_table_lookup (key_name_ht, "c");
  if (pos) {
    g_print (">> %d %d\n", pos->x, pos->y);
  }
  g_hash_table_destroy (key_name_ht);
  return 0;
}
#endif
