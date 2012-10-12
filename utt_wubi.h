#ifndef __UTT_WUBI_H__
#define __UTT_WUBI_H__

#include <gtk/gtk.h>
#include "wubi_class.h"
#include "uttclassrecord.h"
#include "keyboard.h"
#include "wubi_table.h"
#include "utt_plugin.h"

/* predefines */
#define DISPLAY_CHAR_NUM 6
#define TEXT_MOD DISPLAY_CHAR_NUM
#define TEXT_TOTAL (6 * TEXT_MOD)
#define MAX_KEYTEXT (60 * TEXT_MOD)
#define MIN_KEYTEXT (6 * TEXT_MOD)

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
  GtkToolItem *config_button;
};
struct utt_wubi {
  struct wubi_class wubi;
  struct ui ui;
  UttClassRecord *record;		 /* class statistics object */
  struct keyboard_layout kb_layout; /* keyboard object */
  struct wubi_table *table;
  struct utt_plugin_table *plugin;
  /* variables */
  enum class_type class_id, previous_class_id;
  int subclass_id;
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
void utt_config_dialog_run (struct utt_wubi *utt, GtkWidget *box);

#define load_plugin(name) \
  extern struct utt_plugin name##_plugin; \
  utt_register_plugin (utt->plugin, &name##_plugin)

static inline gint utt_current_page (struct utt_wubi *utt)
{
  return gtk_notebook_get_current_page (GTK_NOTEBOOK (utt->ui.notebook));
}

#endif
