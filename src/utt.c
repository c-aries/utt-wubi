#include <stdlib.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <utt/module.h>
#include "config.h"
#include "common.h"
#include "utt_debug.h"

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
    /* LAZY: not beautifual code, learn scim_setup_ui.cpp:create_main_ui() please */
    gtk_window_set_default_icon_list (list);
    g_list_free (list);
    g_object_unref (pixbuf);
  }
}

static void
locale_setup ()
{
  gchar *locale;

  locale = getenv ("LC_ALL");
  if (locale) {
    setlocale (LC_ALL, locale);
  }
  else {
    locale = getenv ("LANG");
    if (locale) {
      setlocale (LC_ALL, locale);
    }
    else {
      setlocale (LC_ALL, "C");
    }
  }
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  textdomain (GETTEXT_PACKAGE);
}

static void
add_class_list (GtkPaned *pane)
{
  GtkWidget *view, *frame;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *sel;

  frame = gtk_frame_new (NULL);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  gtk_paned_pack1 (pane, frame, FALSE, FALSE);

  store = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
		      0, "Utt",
		      -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
		      0, "Wubi86",
		      -1);

  view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  gtk_container_set_border_width (GTK_CONTAINER (view), 4);
  gtk_container_add (GTK_CONTAINER (frame), view);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
  renderer = gtk_cell_renderer_text_new (); /* DISCUSS: memory leak? maybe not, scim_setup_ui.cpp talk us */
  column = gtk_tree_view_column_new_with_attributes (NULL, renderer,
						     "text", 0,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
  /* learn from scim_setup_ui.cpp:create_main_ui(), select one item by default */
  /* create_splash_view() is a beautiful code also */
  gtk_tree_selection_set_mode (sel, GTK_SELECTION_BROWSE);
}

static void
add_class_intro (GtkPaned *pane)
{
  GtkWidget *vbox, *hbox, *frame;
  GtkWidget *custom_label, *button;

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_paned_pack2 (GTK_PANED (pane), vbox, FALSE, FALSE);

  frame = gtk_frame_new (NULL);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  custom_label = gtk_label_new ("custom instroduction display at here.");
  gtk_container_add (GTK_CONTAINER (frame), custom_label);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);

  button = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_box_pack_end (GTK_BOX (hbox), button, FALSE, FALSE, 6);
  button = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  gtk_box_pack_end (GTK_BOX (hbox), button, FALSE, FALSE, 0);
}

int main (int argc, char *argv[])
{
  GtkWidget *window, *vbox;
  GtkWidget *info, *pane;
  struct utt_modules *modules;

  gtk_init (&argc, &argv);

  logo_setup ();
  locale_setup ();
  g_set_prgname (_("Universal Typing Training"));
  utt_debug ();
  modules = utt_modules_new ();
  utt_modules_scan (modules);
  utt_modules_destroy (modules);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (window, 480, 320);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  pane = gtk_hpaned_new ();
  gtk_box_pack_start (GTK_BOX (vbox), pane, TRUE, TRUE, 0);

  add_class_list (GTK_PANED (pane));
  add_class_intro (GTK_PANED (pane));

  info = gtk_statusbar_new ();
  gtk_box_pack_end (GTK_BOX (vbox), info, FALSE, FALSE, 0);

  gtk_widget_show_all (window);
  gtk_main ();
  exit (EXIT_SUCCESS);
}
