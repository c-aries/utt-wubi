#ifndef __UTT_H__
#define __UTT_H__

#include <gtk/gtk.h>

struct utt {
  struct {
    GtkWidget *home_window;
    GtkWidget *im_view;		/* input method tree view */
    GtkWidget *class_window;
  } ui;
};

struct utt *utt_new ();
void utt_destroy (struct utt *utt);
void create_class_window (struct utt *utt);
void launch_class_window ();

#endif

