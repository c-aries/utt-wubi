#include <string.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h>
#include "utt_wubi.h"
#include "utttextarea.h"
#include "utt_dashboard.h"

enum mode {
  TEST_MODE,
  EXAM_MODE,
  N_MODE,
};

#define MODE_CONF "/apps/utt/wubi/jianma/mode"
#define CLASS_INDEX_CONF "/apps/utt/wubi/wenzhang/class_index"
#define CLASS_NUM 2

static struct priv {
  struct utt_wubi *utt;
  struct utt_ui ui;
  struct utt_dashboard *dash;
  GtkWidget *area;
  gchar *gen_chars;
} _priv;
static struct priv *priv = &_priv;

static void on_config_click (GtkToolButton *button, gpointer user_data);
static GtkWidget *main_page (gpointer user_data);

static void
wubi_wenzhang_clean ()
{
  utt_text_area_class_end (UTT_TEXT_AREA (priv->area));
  if (priv->gen_chars) {
    g_free (priv->gen_chars);
    priv->gen_chars = NULL;
  }
  if (gtk_main_level () != 0) {
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->dash->progress), "0%");
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->dash->progress), 0);
  }
}

/* register "class-clean" handler */
static void
wubi_wenzhang_class_clean_func (gpointer data, gpointer user_data)
{
  wubi_wenzhang_clean ();
}

static void
wubi_wenzhang_genchars ()
{
  if (priv->gen_chars) {
    g_free (priv->gen_chars);
  }
  priv->gen_chars = wubi_class_gen_wenzhang_chars (&priv->utt->wubi, priv->utt->subclass_id);
}

static void
on_class_item_activate (GtkMenuItem *item, int type)
{
  struct ui *ui = &priv->utt->ui;

  /* @0 update utt class and subclass id,
     if the previous class is not the current class,
     clean previous class enviroment and set a new class clean handler */
  if (utt_update_class_ids (priv->utt, type)) {
    utt_reset_class_clean_func (priv->utt, wubi_wenzhang_class_clean_func);
  }

  wubi_wenzhang_clean ();
  wubi_wenzhang_genchars ();
  utt_text_area_set_text (UTT_TEXT_AREA (priv->area), priv->gen_chars);
  gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ui->pause_button), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (ui->pause_button), TRUE);
  utt_info (priv->utt, "");

  gtk_widget_grab_focus (priv->area);
  utt_text_area_class_begin (UTT_TEXT_AREA (priv->area));
  gtk_widget_queue_draw (priv->utt->ui.main_window);
}

static gboolean
on_button_press (GtkWidget *widget, GdkEventButton *event, GtkWidget *menu)
{
  if (event->button == 3) {
    gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
		    event->button, event->time);
    return TRUE;
  }
  return FALSE;
}

static void
on_end_of_class (UttTextArea *area, struct utt_wubi *utt)
{
  gint ret;

  gtk_widget_queue_draw (utt->ui.main_window);
  ret = utt_continue_dialog_run (utt);
  if (ret == GTK_RESPONSE_YES) {
    /* genchars, class begin, update ui */
    utt_text_area_class_begin (area);
    gtk_widget_queue_draw (utt->ui.main_window);
  }
  else if (ret == GTK_RESPONSE_NO) {
    /* set subclass_id to NONE, set pause button insensitive */
    utt->subclass_id = SUBCLASS_TYPE_NONE;
    gtk_widget_set_sensitive (GTK_WIDGET (utt->ui.pause_button), FALSE);
  }
}

static gboolean
on_key_press (GtkWidget *widget, GdkEventKey *event, struct utt_wubi *utt)
{
  struct query_record *record;
  gchar *name, *cmp_text, *code;
  gchar **code_array;
  gint i;

  /* @0 if class not begin,
     return, stop propagate the event further.
     if the current page isn't related to the chosen class,
     return, stop propagate the event further*/
  if (!utt_class_record_has_begin (utt->record) ||
      !utt_current_page_is_chosen_class (utt)) {
    return TRUE;
  }

  /* @1 handle unprintable keys */
  if (event->keyval == GDK_F2) {			/* help button */
    if (priv->gen_chars) {
      cmp_text = utt_text_area_get_compare_text (UTT_TEXT_AREA (priv->area));
      record = wubi_article_query (utt->table, cmp_text);
      if (record->num && record->code) {
	name = (gchar *)g_malloc (3 * record->num + 1);
	g_utf8_strncpy (name, cmp_text, record->num);

	code_array = g_new0 (gchar *, record->code->len + 1);
	for (i = 0; i < record->code->len; i++) {
	  code_array[i] = g_ptr_array_index (record->code, i);
	}
	code = g_strjoinv (",", code_array);
	g_free (code_array);

	utt_info (utt, "%s: %s", name, code);
	g_free (code);
	g_free (name);
      }
      g_free (record);
    }
  }
  return FALSE;
}

static enum mode
get_mode ()
{
  GConfClient *config;
  GConfValue *value;
  gint default_mode = TEST_MODE;

  config = gconf_client_get_default ();
  value = gconf_client_get (config, MODE_CONF, NULL);
  if (value && value->type == GCONF_VALUE_INT) {
    default_mode = gconf_value_get_int (value);
  }
  g_object_unref (config);
  return default_mode;
}

static gboolean
set_mode (enum mode mode)
{
  GConfClient *config;

  if (mode >= TEST_MODE && mode <= EXAM_MODE) {
    config = gconf_client_get_default ();
    gconf_client_set_int (config, MODE_CONF, mode, NULL);
    g_object_unref (config);
    return TRUE;
  }
  return FALSE;
}

/* FIXME: common function */
static gint
get_class_index ()
{
  GConfClient *config;
  GConfValue *value;
  gint default_index = 0;

  config = gconf_client_get_default ();
  value = gconf_client_get (config, CLASS_INDEX_CONF, NULL);
  if (value && value->type == GCONF_VALUE_INT) {
    default_index = gconf_value_get_int (value);
  }
  g_object_unref (config);
  return default_index;
}

static gboolean
set_class_index (gint index)
{
  GConfClient *config;

  if (index >= 0 && index < CLASS_NUM) {
    config = gconf_client_get_default ();
    gconf_client_set_int (config, CLASS_INDEX_CONF, index, NULL);
    g_object_unref (config);
    return TRUE;
  }
  return FALSE;
}

static void
on_radio_toggle (GtkToggleButton *button, enum mode mode)
{
  if (gtk_toggle_button_get_active (button)) {
    set_mode (mode);
  }
}

static void
on_config_click (GtkToolButton *button, gpointer user_data)
{
  GtkWidget *vbox, *hbox, *label;
  GtkWidget *radio[N_MODE];
  struct utt_wubi *utt = user_data;

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("添加文章");
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  label = gtk_label_new ("模式(下次训练时生效):");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  radio[EXAM_MODE] = gtk_radio_button_new_with_label (NULL, "考试");
  g_signal_connect (radio[EXAM_MODE], "toggled", G_CALLBACK (on_radio_toggle), GINT_TO_POINTER (EXAM_MODE));
  gtk_box_pack_end (GTK_BOX (hbox), radio[EXAM_MODE], FALSE, TRUE, 0);
  radio[TEST_MODE] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (radio[EXAM_MODE]), "训练");
  g_signal_connect (radio[TEST_MODE], "toggled", G_CALLBACK (on_radio_toggle), GINT_TO_POINTER (TEST_MODE));
  gtk_box_pack_end (GTK_BOX (hbox), radio[TEST_MODE], FALSE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio[get_mode ()]), TRUE);

  utt_config_dialog_run (utt, vbox);
}

static GtkWidget *
main_page (gpointer user_data)
{
  GtkWidget *vbox;
  struct utt_wubi *utt = user_data;
  GtkWidget *frame, *hbox;
  GtkWidget *menu, *class_item;
  gint i;

  vbox = gtk_vbox_new (FALSE, 0);
  priv->utt = utt;

  menu = gtk_menu_new ();	/* take care of memory leak */
  for (i = 0; i < wubi_class_get_subclass_num (&utt->wubi, CLASS_TYPE_WENZHANG); i++) {
    class_item = gtk_menu_item_new_with_label (wubi_class_get_subclass_name (&utt->wubi, CLASS_TYPE_WENZHANG, i));
    g_signal_connect (class_item, "activate", G_CALLBACK (on_class_item_activate), GINT_TO_POINTER (i));
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), class_item);
  }
  gtk_widget_show_all (menu);

  frame = gtk_frame_new ("对比输入");
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 6);
  priv->area = utt_text_area_new ();
  g_signal_connect (priv->area, "class-end", G_CALLBACK (on_end_of_class), utt);
  utt_text_area_set_class_recorder (UTT_TEXT_AREA (priv->area), utt->record);
  gtk_box_pack_start (GTK_BOX (hbox), priv->area, TRUE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (frame), hbox);
  g_signal_connect (priv->area, "button-press-event", G_CALLBACK (on_button_press), menu);
  g_signal_connect (priv->area, "key-press-event", G_CALLBACK (on_key_press), utt);

  priv->dash = utt_dashboard_new (priv->utt);
  gtk_box_pack_start (GTK_BOX (vbox), priv->dash->align, FALSE, FALSE, 0);
  return vbox;
}

static void
class_begin ()
{
  struct ui *ui = &priv->utt->ui;

  wubi_wenzhang_clean ();
  wubi_wenzhang_genchars ();
  utt_text_area_set_text (UTT_TEXT_AREA (priv->area), priv->gen_chars);
  gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (ui->pause_button), FALSE);
  gtk_widget_set_sensitive (GTK_WIDGET (ui->pause_button), TRUE);
  utt_info (priv->utt, "");

  gtk_widget_grab_focus (priv->area);
  utt_text_area_class_begin (UTT_TEXT_AREA (priv->area));
  gtk_widget_queue_draw (priv->utt->ui.main_window);
}

static void
class_clean ()
{
  utt_text_area_class_end (UTT_TEXT_AREA (priv->area));
  if (priv->gen_chars) {
    g_free (priv->gen_chars);
    priv->gen_chars = NULL;
  }
  if (gtk_main_level () != 0) {
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (priv->dash->progress), "0%");
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->dash->progress), 0);
  }
}

static gchar *
nth_class_name (gint n)
{
  static gchar *name[] = {
    "石苍舒醉墨堂",
    "测试",
  };
  return name[n];
}

struct utt_plugin wubi_wenzhang_plugin = {
  .plugin_name = "wubi::wenzhang",
  .locale_name = "文章",
  .class_num = CLASS_NUM,
  .nth_class_name = nth_class_name,
  .get_class_index = get_class_index,
  .set_class_index = set_class_index,
  .create_main_page = main_page,
  .class_begin = class_begin,
  .class_clean = class_clean,
  .config_button_click = on_config_click,
};
