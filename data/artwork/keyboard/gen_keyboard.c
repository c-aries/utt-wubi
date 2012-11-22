#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <gtk/gtk.h>

#define PAD 7
#define REL 6
#define BUTTON_WIDTH 53
#define BUTTON_HEIGHT 53
#define TAB_WIDTH (BUTTON_WIDTH + REL + (BUTTON_WIDTH - REL) / 2)
#define PICTURE_WIDTH (TAB_WIDTH * 2 + BUTTON_WIDTH * 12 + PAD * 2 + REL * 13)
#define PICTURE_HEIGHT (PAD * 2 + BUTTON_HEIGHT * 5 + REL * 4)
#define BACKSPACE_WIDTH (PICTURE_WIDTH - PAD * 2 - BUTTON_WIDTH * 13 - REL * 13)
#define CAPSLOCK_WIDTH (TAB_WIDTH + REL + BUTTON_WIDTH / 4 - REL / 2)
#define ENTER_WIDTH (PICTURE_WIDTH - PAD * 2 - CAPSLOCK_WIDTH - BUTTON_WIDTH * 11 - REL * 12)
#define SHIFT_WIDTH ((PICTURE_WIDTH - PAD * 2 - BUTTON_WIDTH * 10 - REL * 11) / 2)
#define CTRL_WIDTH TAB_WIDTH
#define ALT_WIDTH BUTTON_WIDTH
#define GNU_WIDTH BUTTON_WIDTH
#define SPACE_WIDTH (PICTURE_WIDTH - PAD * 2 - CTRL_WIDTH * 2 - ALT_WIDTH * 2 - GNU_WIDTH * 2 - REL * 6)

int main (int argc, char *argv[])
{
  cairo_t *cr;
  cairo_surface_t *surface;
  GdkPixbuf *button_pixbuf, *backspace_pixbuf, *tab_pixbuf;
  GdkPixbuf *capslock_pixbuf, *enter_pixbuf, *shift_pixbuf;
  GdkPixbuf *space_pixbuf;
  int i;

  g_type_init ();
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, PICTURE_WIDTH, PICTURE_HEIGHT);
  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_rectangle (cr, 0, 0, PICTURE_WIDTH, PICTURE_HEIGHT);
  cairo_fill (cr);

  button_pixbuf = gdk_pixbuf_new_from_file ("button.jpeg", NULL);
  if (!button_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  backspace_pixbuf = gdk_pixbuf_new_from_file ("backspace.jpeg", NULL);
  if (!backspace_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  tab_pixbuf = gdk_pixbuf_new_from_file ("tab.jpeg", NULL);
  if (!tab_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  capslock_pixbuf = gdk_pixbuf_new_from_file ("capslock.jpeg", NULL);
  if (!capslock_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  enter_pixbuf = gdk_pixbuf_new_from_file ("enter.jpeg", NULL);
  if (!enter_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  shift_pixbuf = gdk_pixbuf_new_from_file ("shift.jpeg", NULL);
  if (!shift_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }
  space_pixbuf = gdk_pixbuf_new_from_file ("space.jpeg", NULL);
  if (!space_pixbuf) {
    fprintf (stderr, "fail %d\n", __LINE__);
  }

  for (i = 0; i < 13; i++) {
    if (i == 0) {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD, PAD);
    }
    else {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + BUTTON_WIDTH * i + REL * i, PAD);
    }
    cairo_paint (cr);
  }
  gdk_cairo_set_source_pixbuf (cr, backspace_pixbuf, PAD + BUTTON_WIDTH * i + REL * i, PAD);
  cairo_paint (cr);

  g_print ("button_width %d\n", BUTTON_WIDTH);
  g_print ("tab_width %d\n", TAB_WIDTH);
  g_print ("picture_width %d\n", PICTURE_WIDTH);
  g_print ("picture_height %d\n", PICTURE_HEIGHT);
  g_print ("backspace_width %d\n", BACKSPACE_WIDTH);
  g_print ("capslock_width %d\n", CAPSLOCK_WIDTH);
  g_print ("enter_width %d\n", ENTER_WIDTH);
  g_print ("shift_width %d\n", SHIFT_WIDTH);
  g_print ("space_width %d\n", SPACE_WIDTH);
  gdk_cairo_set_source_pixbuf (cr, tab_pixbuf, PAD, PAD + BUTTON_HEIGHT + REL);
  cairo_paint (cr);
  for (i = 0; i < 12; i++) {
    if (i == 0) {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + REL, PAD + BUTTON_HEIGHT + REL);
    }
    else {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + BUTTON_HEIGHT + REL);
    }
    cairo_paint (cr);
  }
  gdk_cairo_set_source_pixbuf (cr, tab_pixbuf, PAD + TAB_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + BUTTON_HEIGHT + REL);
  cairo_paint (cr);

  gdk_cairo_set_source_pixbuf (cr, capslock_pixbuf, PAD, PAD + (BUTTON_HEIGHT + REL) * 2);
  cairo_paint (cr);
  for (i = 0; i < 11; i++) {
    if (i == 0) {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + CAPSLOCK_WIDTH + REL, PAD + (BUTTON_HEIGHT + REL) * 2);
    }
    else {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + CAPSLOCK_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + (BUTTON_HEIGHT + REL) * 2);
    }
    cairo_paint (cr);
  }
  gdk_cairo_set_source_pixbuf (cr, enter_pixbuf, PAD + CAPSLOCK_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + (BUTTON_HEIGHT + REL) * 2);
  cairo_paint (cr);

  gdk_cairo_set_source_pixbuf (cr, shift_pixbuf, PAD, PAD + (BUTTON_HEIGHT + REL) * 3);
  cairo_paint (cr);
  for (i = 0; i < 10; i++) {
    if (i == 0) {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + SHIFT_WIDTH + REL, PAD + (BUTTON_HEIGHT + REL) * 3);
    }
    else {
      gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + SHIFT_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + (BUTTON_HEIGHT + REL) * 3);
    }
    cairo_paint (cr);
  }
  gdk_cairo_set_source_pixbuf (cr, shift_pixbuf, PAD + SHIFT_WIDTH + REL + (BUTTON_WIDTH + REL) * i, PAD + (BUTTON_HEIGHT + REL) * 3);
  cairo_paint (cr);

  /* ctrl */
  gdk_cairo_set_source_pixbuf (cr, tab_pixbuf, PAD, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* gnu */
  gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + REL, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* alt */
  gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + BUTTON_WIDTH + REL * 2, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* space */
  gdk_cairo_set_source_pixbuf (cr, space_pixbuf, PAD + TAB_WIDTH + BUTTON_WIDTH * 2 + REL * 3, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* alt */
  gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + BUTTON_WIDTH * 2 + SPACE_WIDTH + REL * 4, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* gnu */
  gdk_cairo_set_source_pixbuf (cr, button_pixbuf, PAD + TAB_WIDTH + BUTTON_WIDTH * 3 + SPACE_WIDTH + REL * 5, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);
  /* ctrl */
  gdk_cairo_set_source_pixbuf (cr, tab_pixbuf, PICTURE_WIDTH - CTRL_WIDTH - PAD, PAD + (BUTTON_HEIGHT + REL) * 4);
  cairo_paint (cr);

  g_object_unref (button_pixbuf);
  g_object_unref (backspace_pixbuf);
  g_object_unref (tab_pixbuf);
  g_object_unref (capslock_pixbuf);
  g_object_unref (enter_pixbuf);
  g_object_unref (shift_pixbuf);
  g_object_unref (space_pixbuf);
  cairo_surface_write_to_png (surface, "keyboard.png");
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
  exit (EXIT_SUCCESS);
}
