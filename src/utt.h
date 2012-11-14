#ifndef __UTT_H__
#define __UTT_H__

#include <gtk/gtk.h>
#include <utt/module.h>
#include <utt/intl.h>

struct utt {
  struct {
    GtkWidget *home_window;
    GtkWidget *im_view;		/* input method tree view */
    GtkWidget *class_window;
  } ui;
  struct utt_modules *modules;
  struct utt_module_tree_node *im_node; /* current input method tree node */
};

struct utt *utt_new ();
void utt_destroy (struct utt *utt);
void launch_class_window (struct utt *utt);

#endif

