#include "utt.h"

static void
on_home_click (GtkToolButton *button, struct utt *utt)
{
  gtk_widget_destroy (utt->ui.class_window);
  gtk_widget_show_all (utt->ui.home_window);
}

/* static void */
/* print_module_name (struct utt_module *module, gpointer data) */
/* { */
/*   g_print ("%s\n", module->module_name); */
/* } */

void
launch_class_window (struct utt *utt)
{
  GtkWidget *window, *vbox;
  GtkWidget *toolbar, *notebook, *label;
  GtkToolItem *item;
  GtkWidget *temp_label;
  struct utt_module_tree_node *node;
  struct utt_module *module;
  gchar *name;

  gtk_widget_hide_all (utt->ui.home_window);

  utt->ui.class_window = window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (window, 640, 480);
  module = utt->im_node->module;
  name = g_strdup_printf ("%s(%s)", _("Universal Typing Training"), module->locale_name ());
  gtk_window_set_title (GTK_WINDOW (utt->ui.class_window), name);
  g_free (name);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  toolbar = gtk_toolbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_INDEX);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_toggle_tool_button_new_from_stock (GTK_STOCK_MEDIA_PAUSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  gtk_widget_set_sensitive (GTK_WIDGET (item), FALSE);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_PREFERENCES);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HOME);
  g_signal_connect (item, "clicked", G_CALLBACK (on_home_click), utt);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);
  item = gtk_tool_button_new_from_stock (GTK_STOCK_HELP);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), GTK_TOOL_ITEM (item), -1);

  notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);
  node = utt->im_node->children;
/*   g_print ("%s %p %p\n", utt->im_node->node_name, */
/* 	   utt->im_node->children, utt->im_node->sibling); */
  /* utt_modules_foreach_module (utt->modules, (GFunc)print_module_name, NULL); */
  /* g_print ("%p\n", node); */
  while (node) {
    module = node->module;
    if (!module || module->module_type != UTT_MODULE_CLASS_TYPE) {
      continue;
    }
    label = gtk_label_new (module->locale_name ());
    temp_label = gtk_label_new ("class page");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			      temp_label/* module->create_main_page () */,
			      label);
    node = node->sibling;
  }

  gtk_widget_show_all (utt->ui.class_window);
}
