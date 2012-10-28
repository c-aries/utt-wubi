#include <stdlib.h>
#include <locale.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include "config.h"
#include "common.h"

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
  GtkWidget *window;

  gtk_init (&argc, &argv);

  logo_setup ();
  locale_setup ();
  g_set_prgname (_("Universal Typing Training"));

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

  gtk_widget_show_all (window);
  gtk_main ();
  exit (EXIT_SUCCESS);
}
