#include <string.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h>
#include "utt_wubi.h"
#include "utttextarea.h"
#include "utt_dashboard.h"
#include "utt_xml.h"
#include "utt_article.h"

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

static void
wubi_wenzhang_genchars ()
{
  if (priv->gen_chars) {
    g_free (priv->gen_chars);
  }
  priv->gen_chars = wubi_class_gen_wenzhang_chars (&priv->utt->wubi, priv->utt->subclass_id);
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
on_add_button_click (GtkButton *button, GtkWindow *parent)
{
  GtkWidget *dialog, *content_area, *scroll;
  GtkWidget *vbox, *entry, *view;
  GtkWidget *title_frame, *content_frame, *label;
  GtkTextBuffer *view_buffer;
  GtkTextIter start_iter, end_iter;
  gint ret;
  const gchar *title, *content;
  enum article_add_result result = ARTICLE_ADD_SUCCESS;

  dialog = gtk_dialog_new_with_buttons ("添加文章",
					parent,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_APPLY,
					GTK_RESPONSE_APPLY,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					NULL);
  gtk_widget_set_size_request (dialog, 320, 240);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);
  gtk_container_add (GTK_CONTAINER (content_area), vbox);

  title_frame = gtk_frame_new ("标题");
  gtk_frame_set_shadow_type (GTK_FRAME (title_frame), GTK_SHADOW_NONE);
  entry = gtk_entry_new ();
  gtk_container_add (GTK_CONTAINER (title_frame), entry);
  gtk_box_pack_start (GTK_BOX (vbox), title_frame, FALSE, FALSE, 0);

  content_frame = gtk_frame_new ("内容");
  gtk_frame_set_shadow_type (GTK_FRAME (content_frame), GTK_SHADOW_NONE);
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (content_frame), scroll);
  gtk_container_set_border_width (GTK_CONTAINER (scroll), 2);
  view = gtk_text_view_new ();
  view_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_CHAR);
  gtk_container_add (GTK_CONTAINER (scroll), view);
  gtk_box_pack_start (GTK_BOX (vbox), content_frame, TRUE, TRUE, 0);

  gtk_widget_show_all (dialog);
  for (;;) {
    ret = gtk_dialog_run (GTK_DIALOG (dialog));
    if (ret == GTK_RESPONSE_APPLY) {
      title = gtk_entry_get_text (GTK_ENTRY (entry));
      gtk_text_buffer_get_start_iter (view_buffer, &start_iter);
      gtk_text_buffer_get_end_iter (view_buffer, &end_iter);
      content = gtk_text_buffer_get_text (view_buffer,
					  &start_iter,
					  &end_iter,
					  FALSE);
      result = utt_add_article (title, content);
    }
    if (result == ARTICLE_ADD_SUCCESS || ret != GTK_RESPONSE_APPLY) {
      break;
    }
    if (result & TITLE_INVALIDATE) {
      label = gtk_frame_get_label_widget (GTK_FRAME (title_frame));
      gtk_label_set_markup (GTK_LABEL (label), "标题<span color=\"red\">(不符合要求)</span>");
    }
    else {
      gtk_frame_set_label (GTK_FRAME (title_frame), "标题");
    }
    if (result & CONTENT_INVALIDATE) {
      label = gtk_frame_get_label_widget (GTK_FRAME (content_frame));
      gtk_label_set_markup (GTK_LABEL (label), "内容<span color=\"red\">(不符合要求)</span>");
    }
    else {
      gtk_frame_set_label (GTK_FRAME (content_frame), "内容");
    }
  }
  gtk_widget_destroy (dialog);
}

static GtkWidget *
create_article_view ()
{
  GtkWidget *view;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GList *articles;
  struct utt_xml *xml;
  gint i;

  articles = utt_get_user_articles ();
  store = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < g_list_length (articles); i++) {
    xml = g_list_nth_data (articles, i);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
			0, utt_xml_get_title (xml),
			-1);
    utt_xml_destroy (xml);
  }
  g_list_free (articles);
  view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
  renderer = gtk_cell_renderer_text_new (); /* FIXME: memory leak? */
  column = gtk_tree_view_column_new_with_attributes ("", renderer,
						     "text", 0,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
  return view;
}

static void
on_delete_button_click (GtkButton *button, GtkTreeView *view)
{
  GtkTreeSelection *sel;
  GtkTreePath *path;
  GtkTreeIter iter;
  gint id;

  sel = gtk_tree_view_get_selection (view);
  gtk_tree_selection_get_selected (sel, NULL, &iter);
  path = gtk_tree_model_get_path (gtk_tree_view_get_model (view),
				  &iter);
  id = gtk_tree_path_get_indices (path)[0];
  g_print ("%d\n", id);
  gtk_tree_path_free (path);
}

static void
on_button_click (GtkButton *button, GtkWindow *parent)
{
  GtkWidget *dialog, *content;
  GtkWidget *vbox, *hbox, *button2, *view;

  dialog = gtk_dialog_new_with_buttons ("管理文章",
					parent,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					NULL);
  gtk_widget_set_size_request (dialog, 320, 240);
  content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);
  gtk_container_add (GTK_CONTAINER (content), vbox);
  view = create_article_view ();
  gtk_box_pack_start (GTK_BOX (vbox), view, TRUE, TRUE, 0);
  hbox = gtk_hbox_new (TRUE, 2);
  gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  button2 = gtk_button_new_with_label ("添加");
  g_signal_connect (button2, "clicked", G_CALLBACK (on_add_button_click), dialog);
  gtk_box_pack_start (GTK_BOX (hbox), button2, TRUE, TRUE, 0);
  button2 = gtk_button_new_with_label ("修改");
  gtk_box_pack_start (GTK_BOX (hbox), button2, TRUE, TRUE, 0);
  button2 = gtk_button_new_with_label ("删除");
  g_signal_connect (button2, "clicked", G_CALLBACK (on_delete_button_click), view);
  gtk_box_pack_start (GTK_BOX (hbox), button2, TRUE, TRUE, 0);

  gtk_widget_show_all (dialog);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static GtkWidget *
create_config_page (GtkWidget *dialog)
{
  GtkWidget *vbox, *hbox, *label, *button;
  GtkWidget *radio[N_MODE];

  vbox = gtk_vbox_new (TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  hbox = gtk_hbox_new (FALSE, 0);
  button = gtk_button_new_with_label ("管理文章");
  g_signal_connect (button, "clicked", G_CALLBACK (on_button_click), dialog);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
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
  return vbox;
}

static GtkWidget *
create_main_page ()
{
  GtkWidget *vbox;
  struct utt_wubi *utt = priv->utt;
  GtkWidget *frame, *hbox;

  vbox = gtk_vbox_new (FALSE, 0);

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
  g_signal_connect (priv->area, "key-press-event", G_CALLBACK (on_key_press), utt);

  priv->dash = utt_dashboard_new (priv->utt);
  gtk_box_pack_start (GTK_BOX (vbox), priv->dash->align, FALSE, FALSE, 0);
  return vbox;
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

static void
class_begin ()
{
  wubi_wenzhang_genchars ();
  utt_text_area_set_text (UTT_TEXT_AREA (priv->area), priv->gen_chars);

  gtk_widget_grab_focus (priv->area);
  utt_text_area_class_begin (UTT_TEXT_AREA (priv->area));
  gtk_widget_queue_draw (priv->utt->ui.main_window);
}

static void
init (gpointer user_data)
{
  priv->utt = user_data;
}

static void
destroy ()
{
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
  .init = init,
  .destroy = destroy,
  .class_begin = class_begin,
  .class_clean = class_clean,
  .create_main_page = create_main_page,
  .create_config_page = create_config_page,
};
