#ifndef __UTT_PLUGIN_H__
#define __UTT_PLUGIN_H__

#include <gtk/gtk.h>

struct utt_plugin_table {
  GHashTable *ht;
  GList *list;
  gint num;
};
struct utt_plugin {
  gchar *plugin_name;
  gchar *locale_name;
  gint class_num;
  gchar *(*nth_class_name) (gint n);
  gint (*get_class_index) ();
  gboolean (*set_class_index) (gint index);
  void (*init) (gpointer user_data);
  void (*destroy) ();
  void (*class_begin) (void);
  void (*class_clean) (void);
  GtkWidget *(*create_main_page) (void);
  GtkWidget *(*create_config_page) (GtkWidget *dialog);
  void (*config_button_click) (GtkToolButton *button, gpointer user_data);
};

struct utt_plugin_table *utt_plugin_table_new ();
void utt_plugin_table_destroy (struct utt_plugin_table *table);
gboolean utt_register_plugin (struct utt_plugin_table *table, struct utt_plugin *plugin);
struct utt_plugin *utt_plugin_lookup (struct utt_plugin_table *table, gchar *plugin_name);
struct utt_plugin *utt_nth_plugin (struct utt_plugin_table *table, gint n);
gint utt_get_plugin_num (struct utt_plugin_table *table);

#endif

