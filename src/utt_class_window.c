#include "utt.h"

void
launch_class_window (struct utt *utt)
{
  gtk_widget_hide_all (utt->ui.home_window);
  gtk_widget_show_all (utt->ui.class_window);
}

static void
on_home_click (GtkToolButton *button, struct utt *utt)
{
  gtk_widget_hide_all (utt->ui.class_window);
  gtk_widget_show_all (utt->ui.home_window);
}

void
create_class_window (struct utt *utt)
{
  GtkWidget *window, *vbox;
  GtkWidget *toolbar;
  GtkToolItem *item;

  utt->ui.class_window = window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (window, 640, 480);

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
}
