#ifndef __WUBI_TABLE_H__
#define __WUBI_TABLE_H__

#include <glib.h>

struct wubi_table {
  GHashTable *ht;
};
struct query_record {
  gint num;
  gint deep;
  gboolean stop;
  GPtrArray *code;
};
struct article_query_result {
  gchar *name;
  GPtrArray *code;
};

struct wubi_table *wubi_table_new ();
void wubi_table_destroy (struct wubi_table *table);
gboolean wubi_table_insert (struct wubi_table *table, gchar *utf8str, gchar *code);
GPtrArray *wubi_table_query (struct wubi_table *table, gchar *utf8str);
struct query_record *wubi_article_query (struct wubi_table *table, gchar *utf8str);
void wubi_table_parse_file (struct wubi_table *table, gchar *path);

#endif
