#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h>
#include "config.h"
#include "utt_wubi.h"
#include "common.h"

#define PAGE_CONF "/apps/utt/wubi/page"

static gint
get_page ()
{
  GConfClient *config;
  GConfValue *value;
  gint default_index = 0;

  config = gconf_client_get_default ();
  value = gconf_client_get (config, PAGE_CONF, NULL);
  if (value && value->type == GCONF_VALUE_INT) {
    default_index = gconf_value_get_int (value);
  }
  g_object_unref (config);
  return default_index;
}

static gboolean
set_page (gint index, struct utt_wubi *utt)
{
  GConfClient *config;

  if (index >= 0 && index < utt_get_plugin_num (utt->plugin)) {
    config = gconf_client_get_default ();
    gconf_client_set_int (config, PAGE_CONF, index, NULL);
    g_object_unref (config);
    return TRUE;
  }
  return FALSE;
}

static void
utt_previous_plugin_clean (struct utt_wubi *utt)
{
  struct utt_plugin *pre_plugin;

  pre_plugin = utt_nth_plugin (utt->plugin, utt->previous_class_id);
  if (pre_plugin) {
    pre_plugin->class_clean ();
  }
}

static struct utt_wubi *
utt_wubi_new ()
{
  struct utt_wubi *utt;
  gchar *path;

  utt = g_new0 (struct utt_wubi, 1);
  utt->record = utt_class_record_new ();
  load_keyboard (&utt->kb_layout);
  wubi_class_init (&utt->wubi);
  utt->previous_class_id = utt->class_id = CLASS_TYPE_NONE;
  utt->subclass_id = SUBCLASS_TYPE_NONE;

  utt->table = wubi_table_new ();
  path = g_build_filename (PKGDATADIR, "Wubi.txt", NULL);
  if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
    g_error (G_STRLOC ": %s doesn't exists.", path);
  }
  wubi_table_parse_file (utt->table, path);
  g_free (path);
  return utt;
}

static void
utt_wubi_destroy (struct utt_wubi *utt)
{
  utt_previous_plugin_clean (utt);
  free_keyboard (&utt->kb_layout);
  wubi_class_free (&utt->wubi);
  g_object_unref (utt->record);
  wubi_table_destroy (utt->table);
  g_free (utt);
}

static void
class_record_timer_func (UttClassRecord *record, struct ui *ui)
{
  gtk_widget_queue_draw (ui->main_window);
}

void
utt_info (struct utt_wubi *utt, const gchar *format, ...)
{
  struct ui *ui = &utt->ui;
  va_list args;
  gchar *string = NULL;

  va_start (args, format);
  g_vasprintf (&string, format, args);
  va_end (args);
  gtk_statusbar_pop (GTK_STATUSBAR (ui->info), ui->info_id);
  gtk_statusbar_push (GTK_STATUSBAR (ui->info),
		      ui->info_id,
		      string);
  g_free (string);
}

gboolean
utt_current_page_is_chosen_class (struct utt_wubi *utt)
{
  return (gtk_notebook_get_current_page (GTK_NOTEBOOK (utt->ui.notebook)) ==
	  utt->class_id);
}

/* return value indicate wanna change class(not subclass)? */
gboolean
utt_update_class_ids (struct utt_wubi *utt, gint subclass_id)
{
  utt->subclass_id = subclass_id;
  utt->previous_class_id = utt->class_id;
  utt->class_id = gtk_notebook_get_current_page (GTK_NOTEBOOK (utt->ui.notebook));
  return (utt->previous_class_id != utt->class_id);
}

gboolean
utt_continue_dialog_run (struct utt_wubi *utt)
{
  GtkWidget *dialog, *content, *label;
  gint ret;

  dialog = gtk_dialog_new_with_buttons ("课程结束",
					GTK_WINDOW (utt->ui.main_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_YES,
					GTK_RESPONSE_YES,
					GTK_STOCK_NO,
					GTK_RESPONSE_NO,
					NULL);
  content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new (_("class continue?"));
  gtk_container_add (GTK_CONTAINER (content), label);
  gtk_widget_show_all (dialog);
  ret = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  return ret;
}

/* **** utt_wubi event handlers **** */
static void
on_pause_button_toggled (GtkToggleToolButton *button, struct utt_wubi *utt)
{
  /* @0 if class not begin, return */
  if (!utt_class_record_has_begin (utt->record)) {
    return;
  }

  /* @1 if the current page isn't related to the chosen class,
     it's a bug, the pause button should be insensitive */
  g_assert (utt_current_page_is_chosen_class (utt));

  /* @2 if class can be pause, pause the class and notify user */
  if (utt_class_record_pause_with_check (utt->record)) {
    utt_info (utt, "\"%s\"训练暂停,按Pause键恢复", wubi_class_get_class_name (&utt->wubi, utt->class_id));
  }
  else if (utt_class_record_resume_with_check (utt->record)) { /* @3 else, if class can be resume,
								  resume the class, set the pause button sensitive,
								  and clean notify information */
    gtk_widget_set_sensitive (GTK_WIDGET (utt->ui.pause_button), TRUE);
    utt_info (utt, "");
  }
}

static void
on_notebook_switch (GtkNotebook *notebook,
		    GtkNotebookPage *page,
		    guint page_num,
		    struct utt_wubi *utt)
{
  /* @0 if class not begin, return */
  if (!utt_class_record_has_begin (utt->record)) {
    return;
  }

  /* @1 if the "switch-to" page is not the chosen class,
     set pause button active and insensitive */
  if (page_num != utt->class_id) {
    gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (utt->ui.pause_button), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET (utt->ui.pause_button), FALSE);
  }
  else {			/* @2 else, if the "switch-to" page is the chosen class,
				   set pause button sensitive */
    gtk_widget_set_sensitive (GTK_WIDGET (utt->ui.pause_button), TRUE);
  }
}

static gboolean
on_focus_out (GtkWidget *widget, GdkEventFocus *event, struct utt_wubi *utt)
{
  /* @0 if class not begin,
     return, stop propagate the event further */
  if (!utt_class_record_has_begin (utt->record)) {
    return TRUE;
  }

  /* @1 if the current page isn't related to the chosen class,
     return, stop propagate the event further */
  if (!utt_current_page_is_chosen_class (utt)) {
    return TRUE;
  }

  /* @2 if class can be pause, set the pause button active,
     return, propagate the event further */
  if (utt_class_record_can_pause (utt->record)) {
    gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (utt->ui.pause_button), TRUE);
    return FALSE;
  }
  /* @3 else, return, propagate the event further */
  return FALSE;
}

static gboolean
on_key_press (GtkWidget *widget, GdkEventKey *event, struct utt_wubi *utt)
{
  struct ui *ui = &utt->ui;

  if (utt_class_record_has_begin (utt->record) &&
      utt_current_page_is_chosen_class (utt)) {
    if (event->keyval == GDK_Pause) {
      if (gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON (ui->pause_button))) {
	gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ui->pause_button), FALSE);
      }
      else {
	gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ui->pause_button), TRUE);
      }
      return TRUE;
    }
  }
  return FALSE;
}

static gboolean
on_delete (GtkWidget *window, GdkEvent *event, struct utt_wubi *utt)
{
  utt_wubi_destroy (utt);
  utt_plugin_table_destroy (utt->plugin);
  return FALSE;
}

static void
on_preferences_click (GtkToolButton *button, struct utt_wubi *utt)
{
  GtkWidget *dialog, *content, *notebook;
  GtkWidget *vbox, *label, *page;
  struct utt_plugin *plugin;

  plugin = utt_nth_plugin (utt->plugin, utt_current_page (utt));
  dialog = gtk_dialog_new_with_buttons ("配置",
					GTK_WINDOW (utt->ui.main_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
/* 					GTK_STOCK_CLOSE, */
/* 					GTK_RESPONSE_CLOSE, */
					NULL);
  gtk_widget_set_size_request (dialog, 320, 240);
  content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  notebook = gtk_notebook_new ();
  gtk_container_add (GTK_CONTAINER (content), notebook);

  label = gtk_label_new (plugin->locale_name);
  page = plugin->create_config_page (dialog);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page, label);

  vbox = gtk_vbox_new (FALSE, 0);
  label = gtk_label_new ("全局");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);

  gtk_widget_show_all (dialog);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void
logo_setup ()
{
  GdkPixbuf *pixbuf = NULL;
  GError *error = NULL;
  GList *list = NULL;
  gchar *path = NULL;

  path = g_build_filename (DATAROOTDIR, "icons", "hicolor", "48x48", "apps", "utt-wubi.png", NULL);
  pixbuf = gdk_pixbuf_new_from_file (path, &error);
  if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
    g_error (G_STRLOC ": %s doesn't exists.", path);
  }
  if (error) {
    g_error_free (error);
  }
  g_free (path);
  if (error) {
    g_error_free (error);
    return;
  }
  if (pixbuf) {
    list = g_list_append (list, pixbuf);
    gtk_window_set_default_icon_list (list);
    g_list_free (list);
    g_object_unref (pixbuf);
  }
}

static GtkWidget *
fill_content_area (GtkWidget *content_area, struct utt_plugin *plugin)
{
  GtkWidget *view;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeIter iter;
  GtkListStore *store;
  GtkTreePath *path;
  GtkTreeSelection *sel;
  gint i, id;

  store = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < plugin->class_num (); i++) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
			0, plugin->nth_class_name (i),
			-1);
  }

  id = plugin->get_class_index ();
  path = gtk_tree_path_new_from_indices (id, -1);

  view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
  renderer = gtk_cell_renderer_text_new (); /* FIXME: memory leak? */
  column = gtk_tree_view_column_new_with_attributes ("", renderer,
						     "text", 0,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
  gtk_tree_selection_select_path (sel, path);
  gtk_container_add (GTK_CONTAINER (content_area), view);
  gtk_tree_path_free (path);
  return view;
}

static gboolean
on_index_key_press (GtkWidget *widget, GdkEventKey *event, GtkDialog *dialog)
{
  if (event->keyval == GDK_Return) {
    gtk_dialog_response (dialog, GTK_RESPONSE_OK);
  }
  return FALSE;
}

static void
on_index_click (GtkToolButton *button, struct utt_wubi *utt)
{
  GtkWidget *dialog, *content_area, *view;
  GtkTreeSelection *sel;
  GtkTreePath *path;
  GtkTreeIter iter;
  gint ret, id;
  struct utt_plugin *plugin, *pre_plugin;

  plugin = utt_nth_plugin (utt->plugin, utt_current_page (utt));

  dialog = gtk_dialog_new_with_buttons ("请选择课程内容",
					GTK_WINDOW (utt->ui.main_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK, GTK_RESPONSE_OK,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					NULL);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  view = fill_content_area (content_area, plugin);
  g_signal_connect (view, "key-press-event", G_CALLBACK (on_index_key_press), dialog);
  gtk_widget_show_all (dialog);
  ret = gtk_dialog_run (GTK_DIALOG (dialog));
  if (ret == GTK_RESPONSE_OK) {
    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
    if (gtk_tree_selection_get_selected (sel, NULL, &iter)) {
      path = gtk_tree_model_get_path (gtk_tree_view_get_model (GTK_TREE_VIEW (view)),
				      &iter);
      id = gtk_tree_path_get_indices (path)[0];
      plugin->set_class_index (id);
      gtk_tree_path_free (path);
      if (utt_update_class_ids (utt, id)) {
	if (utt->previous_class_id != CLASS_TYPE_NONE) {
	  pre_plugin = utt_nth_plugin (utt->plugin, utt->previous_class_id);
	  if (pre_plugin) {
	    pre_plugin->class_clean ();
	  }
	}
	set_page (utt_current_page (utt), utt);
      }
      else {
	plugin->class_clean ();
      }
      gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (utt->ui.pause_button), FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET (utt->ui.pause_button), TRUE);
      utt_info (utt, "");
      plugin->class_begin ();
    }
  }
  gtk_widget_destroy (dialog);
}

static void
locale_setup ()
{
  gchar *locale;

  locale = getenv ("LANG");
  if (!locale) {
    locale = "C";
  }
  setlocale (LC_ALL, locale);
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  textdomain (GETTEXT_PACKAGE);
}

int main (int argc, char *argv[])
{
  struct utt_wubi *utt;
  struct ui *ui;
  struct utt_plugin *plugin;
  GtkWidget *window;
  GtkWidget *vbox, *label;
  GtkWidget *toolbar;
  GtkToolItem *item;
  gint i;

  gtk_init (&argc, &argv);

  logo_setup ();
  locale_setup ();
  utt = utt_wubi_new ();
  ui = &utt->ui;

  utt->plugin = utt_plugin_table_new ();
  load_plugin (wubi_zigen);
  load_plugin (wubi_jianma);
  load_plugin (wubi_wenzhang);

  utt_class_record_set_total (utt->record, 5 * TEXT_MOD);
  utt_class_record_set_timer_func (utt->record, (GFunc)class_record_timer_func, &utt->ui);

  ui->main_window = window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  g_signal_connect (window, "delete-event", G_CALLBACK (on_delete), utt);
  g_signal_connect (window, "key-press-event", G_CALLBACK (on_key_press), utt);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  toolbar = gtk_toolbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  g_signal_connect (item, "clicked", G_CALLBACK (on_index_click), utt);
  ui->pause_button = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_MEDIA_PAUSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (ui->pause_button), -1);
  g_signal_connect (ui->pause_button, "toggled", G_CALLBACK (on_pause_button_toggled), utt);
  gtk_widget_set_sensitive (GTK_WIDGET (ui->pause_button), FALSE);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  ui->config_button = gtk_tool_button_new_from_stock (GTK_STOCK_PREFERENCES);
  g_signal_connect (ui->config_button, "clicked", G_CALLBACK (on_preferences_click), utt);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (ui->config_button), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HELP);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);

  ui->notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (vbox), ui->notebook, TRUE, TRUE, 0);
  for (i = 0; i < utt_get_plugin_num (utt->plugin); i++) {
    plugin = utt_nth_plugin (utt->plugin, i);
    label = gtk_label_new (plugin->locale_name);
    gtk_notebook_append_page (GTK_NOTEBOOK (ui->notebook),
			      plugin->create_main_page (),
			      label);
  }
  g_signal_connect (ui->notebook, "switch-page", G_CALLBACK (on_notebook_switch), utt);

  ui->info = gtk_statusbar_new ();
  gtk_statusbar_get_context_id (GTK_STATUSBAR (ui->info), "wubi/zigen/help");
  gtk_box_pack_start (GTK_BOX (vbox), ui->info, FALSE, FALSE, 0);
  gtk_statusbar_push (GTK_STATUSBAR (ui->info),
		      ui->info_id,
		      "回车选择开始训练的项目,按F2可获得当前字词输入方法的提示.");

  g_signal_connect (window, "focus-out-event", G_CALLBACK (on_focus_out), utt);

  gtk_widget_show_all (window);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (ui->notebook), get_page ());
  gtk_main ();
  exit (EXIT_SUCCESS);
}
