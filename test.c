#include <stdlib.h>
#include <gtk/gtk.h>
#include "utttextarea.h"
#include "class.h"

static void
class_timer_func (struct class *cls, GtkWidget *window)
{
  gtk_widget_queue_draw (window);
}

static gboolean
continue_dialog_run (GtkWindow *window)
{
  GtkWidget *dialog, *content, *label;
  gint ret;

  dialog = gtk_dialog_new_with_buttons ("课程结束",
					window,
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

static void
on_end_of_class (UttTextArea *area, GtkWidget *window)
{
  gint ret;

  gtk_widget_queue_draw (window);
  ret = continue_dialog_run (GTK_WINDOW (window));
  if (ret == GTK_RESPONSE_YES) {
    /* utt_text_area_reset (area); */
    utt_text_area_set_text (area, "abc");
    utt_text_area_class_begin (area);
  }
  else if (ret == GTK_RESPONSE_NO) {
  }
}

static void
on_class_begin (UttTextArea *area, GtkWidget *window)
{
  gtk_widget_queue_draw (window);
}

int main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *area;
  struct class *class;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);

  class = class_create ();
  class_set_timer_func (class, (GFunc)class_timer_func, window);

  area = utt_text_area_new ();
  utt_text_area_set_class_recorder (UTT_TEXT_AREA (area), class);
  utt_text_area_set_text (UTT_TEXT_AREA (area), "近者作堂名醉墨，如饮美酒销百忧。");
  g_signal_connect_after (area, "end-of-class", G_CALLBACK (on_end_of_class), window);
  g_signal_connect_after (area, "class-begin", G_CALLBACK (on_class_begin), window);
  gtk_container_add (GTK_CONTAINER (window), area);

  gtk_widget_show_all (window);
  utt_text_area_class_begin (UTT_TEXT_AREA (area));
  gtk_main ();
  exit (EXIT_SUCCESS);
}
