#include <gdk/gdkkeysyms.h>
#include "utt.h"

static void
on_home_click (GtkToolButton *button, struct utt *utt)
{
  struct utt_module *module;
  struct utt_class_module *class_module;

  gtk_widget_destroy (utt->ui.class_window);
  if (utt->class_node) {
    module = utt->class_node->module;
    g_assert (module->module_type == UTT_MODULE_CLASS_TYPE);
    if (module->module_type == UTT_MODULE_CLASS_TYPE) {
      class_module = module->priv_data;
      class_module->destroy ();
    }
  }
  gtk_widget_show_all (utt->ui.home_window);
}

static GtkWidget *
fill_content_area (GtkWidget *scroll, struct utt_class_module *class_module)
{
  GtkListStore *store;
  GtkTreeIter iter;
  GtkWidget *view;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *sel;
  gint i;

  store = gtk_list_store_new (1, G_TYPE_STRING);
  for (i = 0; i < class_module->class_num (); i++) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
			0, class_module->nth_class_name (i),
			-1);
  }

  view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
  renderer = gtk_cell_renderer_text_new (); /* FIXME: memory leak? */
  column = gtk_tree_view_column_new_with_attributes ("", renderer,
						     "text", 0,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
  gtk_tree_selection_set_mode (sel, GTK_SELECTION_BROWSE);

  gtk_container_add (GTK_CONTAINER (scroll), view);
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
on_index_click (GtkToolButton *button, struct utt *utt)
{
  GtkWidget *dialog, *content_area, *scroll, *view;
  GtkTreeSelection *sel;
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreePath *path;
  struct utt_module *module;
  struct utt_class_module *class_module;
  gint ret, id;

  if (!utt->class_node) {
    return;
  }
  module = utt->class_node->module;
  class_module = module->priv_data;
  dialog = gtk_dialog_new_with_buttons (_("choose one class"),
					GTK_WINDOW (utt->ui.class_window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_OK, GTK_RESPONSE_OK,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					NULL);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_widget_set_size_request (content_area, -1, 240);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (content_area), scroll);
  gtk_container_set_border_width (GTK_CONTAINER (scroll), 2);

  view = fill_content_area (scroll, class_module);
  g_signal_connect (view, "key-press-event", G_CALLBACK (on_index_key_press), dialog);
  gtk_widget_show_all (dialog);

  ret = gtk_dialog_run (GTK_DIALOG (dialog));
  if (ret == GTK_RESPONSE_OK) {
    sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
    if (gtk_tree_selection_get_selected (sel, &model, &iter)) {
      path = gtk_tree_model_get_path (gtk_tree_view_get_model (GTK_TREE_VIEW (view)),
				      &iter);
      utt->class_item = id = gtk_tree_path_get_indices (path)[0];
      g_print ("class item %d, %s\n", id, class_module->nth_class_name (id));
      gtk_tree_path_free (path);
    }
  }
  gtk_widget_destroy (dialog);
}

static void
on_notebook_switch (GtkNotebook *notebook,
		    GtkNotebookPage *page,
		    guint page_num,
		    struct utt *utt)
{
  struct utt_module *module;
  
  utt->class_node = utt_nth_child_node (utt->im_node, page_num);
  module = utt->class_node->module;
}

void
launch_class_window (struct utt *utt)
{
  GtkWidget *window, *vbox;
  GtkWidget *toolbar, *notebook, *label, *page;
  GtkToolItem *item;
  struct utt_module_tree_node *node;
  struct utt_module *module;
  struct utt_class_module *class_module;
  gchar *name;

  gtk_widget_hide_all (utt->ui.home_window);

  utt->ui.class_window = window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  module = utt->im_node->module;
  name = g_strdup_printf ("%s(%s)", _("Universal Typing Training"), module->locale_name ());
  gtk_window_set_title (GTK_WINDOW (utt->ui.class_window), name);
  g_free (name);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  toolbar = gtk_toolbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);
  g_signal_connect (item, "clicked", G_CALLBACK (on_index_click), utt);
  gtk_tool_item_set_tooltip_text (item, _("choose a new class to begin"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_MEDIA_PAUSE);
  gtk_tool_item_set_tooltip_text (item, _("class pause"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  gtk_widget_set_sensitive (GTK_WIDGET (item), FALSE);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_PREFERENCES);
  gtk_tool_item_set_tooltip_text (item, _("configuration"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HOME);
  gtk_tool_item_set_tooltip_text (item, _("go back homepage"));
  g_signal_connect (item, "clicked", G_CALLBACK (on_home_click), utt);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HELP);
  gtk_tool_item_set_tooltip_text (item, _("help manual and tutorial"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);

  notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);
  node = utt->im_node->children;
  while (node) {
    module = node->module;
    if (!module || module->module_type != UTT_MODULE_CLASS_TYPE) {
      continue;
    }
    class_module = module->priv_data;
    class_module->init ();
    label = gtk_label_new (module->locale_name ());
    page = class_module->create_class_page ();
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page, label);
    node = node->sibling;
  }
  g_signal_connect (notebook, "switch-page", G_CALLBACK (on_notebook_switch), utt);

  gtk_widget_show_all (utt->ui.class_window);
}
