#ifndef __UTT_DASHBOARD_H__
#define __UTT_DASHBOARD_H__

#include "utt_wubi.h"

struct utt_dashboard {
  GtkWidget *draw;
  GtkWidget *progress;
  GtkWidget *align;
  gpointer priv;
};

struct utt_dashboard *utt_dashboard_new (struct utt_wubi *utt);
void utt_dashboard_destroy (struct utt_dashboard *dash);
void utt_dashboard_set_class_recorder (struct utt_dashboard *dash, UttClassRecord *record);

#endif
