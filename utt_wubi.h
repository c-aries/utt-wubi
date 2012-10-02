#ifndef __UTT_WUBI_H__
#define __UTT_WUBI_H__

#include <gtk/gtk.h>
#include "wubi_class.h"
#include "uttclassrecord.h"
#include "keyboard.h"
#include "wubi_table.h"

/* predefines */
#define DISPLAY_CHAR_NUM 6
#define TEXT_MOD DISPLAY_CHAR_NUM
#define TEXT_TOTAL (6 * TEXT_MOD)

struct utt_ui {
  GtkWidget *ch_draw[DISPLAY_CHAR_NUM];
  GtkWidget *kb_draw;
  cairo_surface_t *button_image;
  cairo_surface_t *kb_image;
  gint button_width, button_height;
};
struct ui {
  GtkWidget *main_window;
  GtkWidget *notebook;
  GtkWidget *info;
  guint info_id;
  GtkToolItem *pause_button;
};
struct utt_wubi {
  struct wubi_class wubi;
  struct ui ui;
  UttClassRecord *record;		 /* class statistics object */
  struct keyboard_layout kb_layout; /* keyboard object */
  struct wubi_table *table;
  /* variables */
  enum class_type class_id, previous_class_id;
  int subclass_id;
  /* func */
  GFunc class_clean_func;
};

/* utt interface */
gboolean utt_current_page_is_chosen_class (struct utt_wubi *utt);
gboolean utt_update_class_ids (struct utt_wubi *utt, gint subclass_id);
void utt_info (struct utt_wubi *utt, const gchar *format, ...);
void utt_previous_class_clean (struct utt_wubi *utt);
void utt_set_class_clean_func (struct utt_wubi *utt, GFunc clean_func);
void utt_reset_class_clean_func (struct utt_wubi *utt, GFunc clean_func);
/* common functions */
gboolean utt_continue_dialog_run ();
/* subclass */
void wubi_jianma (struct utt_wubi *utt, GtkWidget *vbox);
void wubi_zigen (struct utt_wubi *utt, GtkWidget *vbox);
void wubi_wenzhang (struct utt_wubi *utt, GtkWidget *vbox);

#endif
