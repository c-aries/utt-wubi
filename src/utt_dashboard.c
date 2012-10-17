#include <glib/gprintf.h>
#include "utt_dashboard.h"
#include "uttclassrecord.h"

struct utt_dashboard_priv {
  struct utt_wubi *utt;
  UttClassRecord *record;		/* default is utt->cls */
  cairo_surface_t *dashboard_image;
};

void
utt_dashboard_set_class_recorder (struct utt_dashboard *dash, UttClassRecord *record)
{
  struct utt_dashboard_priv *priv = dash->priv;

  priv->record = record;
}

static gboolean
on_dashboard_draw_expose (GtkWidget *widget, GdkEventExpose *event, struct utt_dashboard *dash)
{
  struct utt_dashboard_priv *priv = dash->priv;
  cairo_t *cr;
  gchar timestamp[9];		/* "00:00:00" */
  gchar speedstamp[9];		/* "000.00/m" */
  gchar correctstamp[5];	/* "100%" */
  gchar finishstamp[5];		/* "100%" */
  gint hour, min, sec;
  gdouble finish;

  g_assert (utt_class_record_get_total (priv->utt->record) > 0);

  if (!utt_current_page_is_chosen_class (priv->utt)) {
    cr = gdk_cairo_create (event->window);
    cairo_set_source_surface (cr, priv->dashboard_image, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return TRUE;
  }
  utt_class_record_format_elapse_time (priv->utt->record, &hour, &min, &sec);
  g_sprintf (timestamp, "%02d:%02d:%02d", hour, min, sec);
  g_sprintf (correctstamp, "%d%%", utt_class_record_stat_correct (priv->utt->record));
  g_sprintf (speedstamp, "%.2lf/m", utt_class_record_stat_speed (priv->utt->record));
  finish = utt_class_record_stat_finish (priv->utt->record);
  g_sprintf (finishstamp, "%d%%", (gint)(100 * finish));
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (dash->progress), finishstamp);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (dash->progress), finish);

  cr = gdk_cairo_create (event->window);
  cairo_set_source_surface (cr, priv->dashboard_image, 0, 0);
  cairo_paint (cr);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_select_font_face (cr, "Monospace",
			  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, 18.0);

  cairo_move_to (cr, 80, 43);
  cairo_show_text (cr, timestamp);

  cairo_move_to (cr, 284, 43);
  cairo_show_text (cr, speedstamp);

  cairo_move_to (cr, 708, 43);
  cairo_show_text (cr, correctstamp);

  cairo_destroy (cr);
  return TRUE;
}

struct utt_dashboard *
utt_dashboard_new (struct utt_wubi *utt)
{
  struct utt_dashboard *dash;
  struct utt_dashboard_priv *priv;
  GtkWidget *fix, *align;
  gchar *path;

  dash = g_new0 (struct utt_dashboard, 1);
  dash->align = align = gtk_alignment_new (0.5, 0.5, 0, 0);
  fix = gtk_fixed_new ();
  gtk_container_add (GTK_CONTAINER (align), fix);
  gtk_container_set_border_width (GTK_CONTAINER (align), 4);

  priv = dash->priv = g_new0 (struct utt_dashboard, 1);
  priv->utt = utt;
  priv->record = utt->record;
  path = g_build_filename (PKGDATADIR, "dashboard.png", NULL);
  if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
    g_error (G_STRLOC ": %s doesn't exists.", path);
  }
  priv->dashboard_image = cairo_image_surface_create_from_png (path);
  g_free (path);
  dash->draw = gtk_drawing_area_new ();
  gtk_widget_set_size_request (dash->draw,
			       cairo_image_surface_get_width (priv->dashboard_image),
			       cairo_image_surface_get_height (priv->dashboard_image));
  gtk_container_add (GTK_CONTAINER (fix), dash->draw);

  dash->progress = gtk_progress_bar_new ();
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (dash->progress), "0%");
  gtk_widget_set_size_request (dash->progress, 120, -1);
  gtk_fixed_put (GTK_FIXED (fix), dash->progress, 480, 23);

  g_signal_connect (dash->draw, "expose-event", G_CALLBACK (on_dashboard_draw_expose), dash);
  return dash;
}

void
utt_dashboard_queue_draw (struct utt_dashboard *dash)
{
  gtk_widget_queue_draw (dash->draw);
}

void
utt_dashboard_destroy (struct utt_dashboard *dash)
{
  struct utt_dashboard_priv *priv = dash->priv;

  g_object_unref (dash->progress);
  g_object_unref (dash->draw);
  cairo_surface_destroy (priv->dashboard_image);
  g_free (dash);
}
