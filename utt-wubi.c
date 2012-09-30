#include <stdlib.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include "utt_wubi.h"

static struct utt_wubi *
utt_wubi_new ()
{
  struct utt_wubi *utt;

  utt = g_new0 (struct utt_wubi, 1);
  utt->record = utt_class_record_new ();
  load_keyboard (&utt->kb_layout);
  wubi_class_init (&utt->wubi);
  utt->previous_class_id = utt->class_id = CLASS_TYPE_NONE;
  utt->subclass_id = SUBCLASS_TYPE_NONE;
  return utt;
}

void
utt_previous_class_clean (struct utt_wubi *utt)
{
  if (utt->class_clean_func) {
    utt->class_clean_func (NULL, NULL);
    utt->class_clean_func = NULL;
  }
}

void
utt_set_class_clean_func (struct utt_wubi *utt, GFunc clean_func)
{
  utt->class_clean_func = clean_func;
}

void
utt_reset_class_clean_func (struct utt_wubi *utt, GFunc class_clean)
{
  utt_previous_class_clean (utt);
  utt_set_class_clean_func (utt, class_clean);
}

static void
utt_wubi_destroy (struct utt_wubi *utt)
{
  utt_previous_class_clean (utt);
  free_keyboard (&utt->kb_layout);
  wubi_class_free (&utt->wubi);
  g_object_unref (utt->record);
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
  label = gtk_label_new ("继续训练吗?");
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
  return FALSE;
}

int main (int argc, char *argv[])
{
  struct utt_wubi *utt;
  struct ui *ui;
  struct wubi_class *wubi;
  GtkWidget *window;
  GtkWidget *vbox, *vbox2, *label;
  GtkWidget *toolbar;
  GtkToolItem *item;
  gint i;

  gtk_init (&argc, &argv);

  utt = utt_wubi_new ();
  ui = &utt->ui;
  wubi = &utt->wubi;

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
  ui->pause_button = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_MEDIA_PAUSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (ui->pause_button), -1);
  g_signal_connect (ui->pause_button, "toggled", G_CALLBACK (on_pause_button_toggled), utt);
  gtk_widget_set_sensitive (GTK_WIDGET (ui->pause_button), FALSE);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_PREFERENCES);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HELP);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);

  ui->notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (vbox), ui->notebook, TRUE, TRUE, 0);
  for (i = 0; i < wubi_class_get_class_num (wubi); i++) {
    label = gtk_label_new (wubi_class_get_class_name (wubi, i));
    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK (ui->notebook), vbox2, label);
    if (i == CLASS_TYPE_ZIGEN) {
      wubi_zigen (utt, vbox2);
    }
    else if (i == CLASS_TYPE_JIANMA) {
      wubi_jianma (utt, vbox2);
    }
    else if (i == CLASS_TYPE_WENZHANG) {
      wubi_wenzhang (utt, vbox2);
    }
  }
  g_signal_connect (ui->notebook, "switch-page", G_CALLBACK (on_notebook_switch), utt);

  ui->info = gtk_statusbar_new ();
  gtk_statusbar_get_context_id (GTK_STATUSBAR (ui->info), "wubi/zigen/help");
  gtk_box_pack_start (GTK_BOX (vbox), ui->info, FALSE, FALSE, 0);
  gtk_statusbar_push (GTK_STATUSBAR (ui->info),
		      ui->info_id,
		      "右键点击\"键盘/比对输入\"选择开始训练的项目,按F2可获得当前字词输入方法的提示.");

  g_signal_connect (window, "focus-out-event", G_CALLBACK (on_focus_out), utt);

  gtk_widget_show_all (window);
  gtk_main ();
  exit (EXIT_SUCCESS);
}
