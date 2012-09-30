#ifndef __WUBI_CLASS_H__
#define __WUBI_CLASS_H__

#include <glib.h>
#include <cairo.h>
#include "zigen_images.h"

enum class_type {
  CLASS_TYPE_ZIGEN,
  CLASS_TYPE_JIANMA,
  CLASS_TYPE_WENZHANG,
  CLASS_TYPE_NONE,
  N_CLASS_TYPE,
};
enum zigen_subclass_type {
  SUBCLASS_TYPE_HENG,
  SUBCLASS_TYPE_SHU,
  SUBCLASS_TYPE_PIE,
  SUBCLASS_TYPE_NA,
  SUBCLASS_TYPE_ZHE,
  SUBCLASS_TYPE_ALL,
  SUBCLASS_TYPE_NONE,
  N_SUBCLASS_TYPE,
};
enum wenzhange_subclass_type {
  WENZHANG_SUBCLASS_TYPE_TEXT0,
  WENZHANG_SUBCLASS_TYPE_NONE,
  N_WENZHANG_SUBCLASS_TYPE,
};
struct wubi_class {
  struct zigen_images zigen_images;
};
void wubi_class_init (struct wubi_class *wubi);
void wubi_class_free (struct wubi_class *wubi);
struct zigen_chars *wubi_class_gen_zigen_chars (struct wubi_class *wubi,
						enum zigen_subclass_type,
						gint char_num);
gchar *wubi_class_gen_jianma_chars (struct wubi_class *wubi, gint char_num);
gchar *wubi_class_get_jianma_by_char (gchar ch);
gchar *wubi_class_gen_wenzhang_chars (struct wubi_class *wubi,
				      gint subclass_id);
int wubi_class_get_class_num (struct wubi_class *wubi);
gchar *wubi_class_get_class_name (struct wubi_class *wubi, int class_index);
int wubi_class_get_subclass_num (struct wubi_class *wubi, int class_type);
gchar *wubi_class_get_subclass_name (struct wubi_class *wubi, int class_type, int subclass_index);

struct zigen_char {
  gchar value;
  cairo_surface_t *img;
};
struct zigen_chars {
  gint num;
  struct zigen_char *ch;
};
/* struct zigen_chars *class_gen_zigen_chars (GHashTable *ht, enum subclass_type type, gint char_num); */
void free_zigen_chars (struct zigen_chars *chars);

gchar *wubi_article_get_by_id (int id);

#endif
