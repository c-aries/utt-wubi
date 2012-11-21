#include <glib.h>
#include <utt/module.h>
#include <utt/intl.h>
#include <utt/keyboard.h>

enum {
  CLASS_HENG,
  CLASS_SHU,
  CLASS_PIE,
  CLASS_NA,
  CLASS_ZHE,
  CLASS_ALL,
  CLASS_NUM,
};

struct _zigen_priv {
  GtkWidget *kb;
};
static struct _zigen_priv *priv;

static char *
locale_name ()
{
  return _("zigen");
}

static gint
class_num (void)
{
  return CLASS_NUM;
}

static gchar *
nth_class_name (gint nth)
{
  gchar *name = NULL;

  if (nth == CLASS_HENG) {
    name = _("HENG(ASDFG)");
  }
  else if (nth == CLASS_SHU) {
    name = _("SHU(HJKLM)");
  }
  else if (nth == CLASS_PIE) {
    name = _("PIE(QWERT)");
  }
  else if (nth == CLASS_NA) {
    name = _("NA(YUIOP)");
  }
  else if (nth == CLASS_ZHE) {
    name = _("ZHE(XCVBN)");
  }
  else if (nth == CLASS_ALL) {
    name = _("ALL");
  }
  return name;
}

GtkWidget *create_class_page (void)
{
#if 1
  GtkWidget *vbox;

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), priv->kb, FALSE, FALSE, 0);
  return vbox;
#else
  GtkWidget *vbox;
  GtkWidget *frame, *hbox, *hbox2, *align;
  GtkWidget *ch_draw[6];
  GtkWidget *kb_draw;
  cairo_surface_t *kb_image;
  gint i;
  gchar *path;

  vbox = gtk_vbox_new (FALSE, 0);
  frame = gtk_frame_new (_("Display Zone"));
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (frame), hbox);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);

  /* padding */
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), hbox2, TRUE, TRUE, 0);
  for (i = 0; i < 6; i++) {
    ch_draw[i] = gtk_drawing_area_new ();
    gtk_widget_set_size_request (ch_draw[i], 48, 48);
    hbox2 = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), hbox2, TRUE, TRUE, 0);
    align = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (align), ch_draw[i]);
    gtk_box_pack_start (GTK_BOX (hbox2), align, TRUE, TRUE, 0);
  }
  /* padding */
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), hbox2, TRUE, TRUE, 0);

  kb_draw = gtk_drawing_area_new ();
  path = g_build_filename (PKGDATADIR, "keyboard_wubi.png", NULL);
  if (!g_file_test (path, G_FILE_TEST_EXISTS)) {
    g_error (G_STRLOC ": %s doesn't exists.", path);
  }
  kb_image = cairo_image_surface_create_from_png (path);
  g_free (path);

  gtk_widget_add_events (kb_draw, GDK_KEY_PRESS_MASK);
  /* set can focus and grab focus, so we can receive key press event */
  gtk_widget_set_can_focus (kb_draw, TRUE);
  gtk_widget_set_size_request (kb_draw,
			       cairo_image_surface_get_width (kb_image),
			       cairo_image_surface_get_height (kb_image));
  align = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_container_add (GTK_CONTAINER (align), kb_draw);
  gtk_container_set_border_width (GTK_CONTAINER (align), 4);
  gtk_box_pack_start (GTK_BOX (vbox), align, TRUE, TRUE, 0);

  return vbox;
#endif
}

static void
init (void)
{
  priv = g_new0 (struct _zigen_priv, 1);
  /* needn't call g_object_unref, gtk_widget_destroy(class_window) free it */
  priv->kb = utt_keyboard_new ();
}

static void
destroy (void)
{
  g_free (priv);
}

struct utt_class_module class_module = {
  .init = init,
  .destroy = destroy,
  .class_num = class_num,
  .nth_class_name = nth_class_name,
  .create_class_page = create_class_page,
};
struct utt_module utt_module = {
  .module_name = "wubi::zigen",
  .locale_name = locale_name,
  .module_type = UTT_MODULE_CLASS_TYPE,
  .priv_data = &class_module,
};
