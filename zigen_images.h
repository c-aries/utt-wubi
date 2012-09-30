#ifndef __ZIGEN_IMAGES_H__
#define __ZIGEN_IMAGES_H__

#include <glib.h>

struct zigen_images {
  GHashTable *char_ht;
};

struct zigen_img {
  gint num;
  cairo_surface_t **img;
};

void load_zigen_images (struct zigen_images *images);
void free_zigen_images (struct zigen_images *images);

#endif
