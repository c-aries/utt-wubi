#include <stdio.h>
#include <string.h>
#include <glib/gstdio.h>
#include "wubi_table.h"

struct tree_node {
  gchar *word;
  GPtrArray *code;
  struct tree_node *children;
  struct tree_node *sibling;
  gboolean sorted;		/* code arrary sorted */
};

static void first_child_tree_node_free (struct tree_node *node);

static void
sibling_tree_node_free (struct tree_node *node)
{
  if (!node) {
    return;
  }
  first_child_tree_node_free (node->children);
  if (node->word) {
    g_free (node->word);
    node->word = NULL;
  }
  if (node->code) {
    g_ptr_array_unref (node->code);
    node->code = NULL;
  }
  g_free (node);
}

static void
first_child_tree_node_free (struct tree_node *node)
{
  struct tree_node *temp_node;

  if (!node) {
    return;
  }
  first_child_tree_node_free (node->children);
  temp_node = node;
  while ((temp_node = temp_node->sibling)) {
    sibling_tree_node_free (temp_node);
  }
  /*  */
  if (node->word) {
    g_free (node->word);
    node->word = NULL;
  }
  if (node->code) {
    g_ptr_array_unref (node->code);
    node->code = NULL;
  }
  g_free (node);
}

static void
hash_tree_node_free (struct tree_node *node)
{
  if (!node) {
    return;
  }
  first_child_tree_node_free (node->children);
  /*  */
  if (node->word) {
    g_free (node->word);
    node->word = NULL;
  }
  if (node->code) {
    g_ptr_array_unref (node->code);
    node->code = NULL;
  }
  g_free (node);
}

struct wubi_table *
wubi_table_new ()
{
  struct wubi_table *table;

  table = g_new0 (struct wubi_table, 1);
  table->ht = g_hash_table_new_full (g_str_hash, g_str_equal,
				     NULL,
				     (GDestroyNotify)hash_tree_node_free);
  return table;
}

void
wubi_table_destroy (struct wubi_table *table)
{
  g_hash_table_unref (table->ht);
  g_free (table);
}

static gboolean
code_has_exist (GPtrArray *array, gchar *code)
{
  gchar *store_code;
  gint i;

  g_return_val_if_fail (array != NULL, FALSE);
  for (i = 0; i < array->len; i++) {
    store_code = g_ptr_array_index (array, i);
    if (g_strcmp0 (store_code, code) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

static gint
code_cmp (gconstpointer a, gconstpointer b)
{
  const gchar *code_a = a;
  const gchar *code_b = b;

  return g_strcmp0 (code_a, code_b);
}

static GPtrArray *
code_array_sort (GPtrArray *array)
{
  g_return_val_if_fail (array, NULL);

  g_ptr_array_sort (array, code_cmp);
  return array;
}

static GPtrArray *
node_sorted_code_array (struct tree_node *node)
{
  if (!node->sorted) {
    node->sorted = TRUE;
    return code_array_sort (node->code);
  }
  return node->code;
}

static gboolean
wubi_table_insert_node (struct tree_node *node, gchar *utf8str, gchar *code)
{
  gchar word[7];
  gulong utf8_len;
  gint len;
  struct tree_node *child_node, *pre_node;

  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return FALSE;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  child_node = node->children;
  pre_node = NULL;
  while (child_node) {
    if (g_strcmp0 (child_node->word, word) == 0) {
      break;
    }
    pre_node = child_node;
    child_node = child_node->sibling;
  }
  if (child_node) {
    if (utf8_len == 1) {
      if (child_node->code == NULL) {
	child_node->code = g_ptr_array_new ();
	child_node->sorted = TRUE;
      }
      else {
	if (code_has_exist (child_node->code, code)) {
	  return FALSE;
	}
	child_node->sorted = FALSE;
      }
      g_ptr_array_add (child_node->code, g_strdup (code));
      return TRUE;
    }
  }
  else {
    child_node = g_new0 (struct tree_node, 1);
    child_node->word = g_strdup (word);
    if (utf8_len == 1) {
      child_node->code = g_ptr_array_new ();
      child_node->sorted = TRUE;
      g_ptr_array_add (child_node->code, g_strdup (code));
    }
    if (node->children == NULL) {
      node->children = child_node;
    }
    else if (pre_node) {
      pre_node->sibling = child_node;
    }
    if (utf8_len == 1) {
      return TRUE;
    }
  }
  return wubi_table_insert_node (child_node, utf8str + len, code);
}

gboolean
wubi_table_insert (struct wubi_table *table, gchar *utf8str, gchar *code)
{
  gchar word[7];
  gint len;
  struct tree_node *node;
  gulong utf8_len;

  /* get the first utf8 character information */
  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return FALSE;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  node = g_hash_table_lookup (table->ht, word);
  if (!node) {
    /* create a new node */
    node = g_new0 (struct tree_node, 1);
    node->word = g_strdup (word);
    if (utf8_len == 1) {
      node->code = g_ptr_array_new ();
      node->sorted = TRUE;
      g_ptr_array_add (node->code, g_strdup (code));
    }
    g_hash_table_insert (table->ht, node->word, node); /* node->word DON'T BE word! */
    if (utf8_len == 1) {
      return TRUE;
    }
  }
  else {
    if (utf8_len == 1) {
      if (node->code == NULL) {
	node->code = g_ptr_array_new ();
	node->sorted = TRUE;
      }
      else {
	if (code_has_exist (node->code, code)) {
	  return FALSE;
	}
	node->sorted = FALSE;
      }
      g_ptr_array_add (node->code, g_strdup (code));
      return TRUE;
    }
  }
  return wubi_table_insert_node (node, utf8str + len, code);
}

static GPtrArray *
wubi_table_query_node (struct tree_node *node, gchar *utf8str)
{
  gchar word[7];
  gulong utf8_len;
  gint len;
  struct tree_node *child_node;

  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return NULL;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  child_node = node->children;
  while (child_node) {
    if (g_strcmp0 (child_node->word, word) == 0) {
      break;
    }
    child_node = child_node->sibling;
  }
  if (child_node) {
    if (utf8_len == 1) {
      return node_sorted_code_array (child_node);
    }
  }
  else {
    return NULL;
  }
  return wubi_table_query_node (child_node, utf8str + len);
}

GPtrArray *
wubi_table_query (struct wubi_table *table, gchar *utf8str)
{
  gchar word[7];
  gint len;
  gulong utf8_len;
  struct tree_node *node;

  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return NULL;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  node = g_hash_table_lookup (table->ht, word);
  if (!node) {
    return NULL;
  }
  else {
    if (utf8_len == 1) {
      if (g_strcmp0 (node->word, word) == 0) {
	return node_sorted_code_array (node);
      }
    }
  }
  return wubi_table_query_node (node, utf8str + len);
}

static void
wubi_article_query_node (struct tree_node *node,
			 struct query_record *record,
			 gchar *utf8str)
{
  gchar word[7];
  gulong utf8_len;
  gint len;
  struct tree_node *child_node;

  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  child_node = node->children;
  while (child_node) {
    if (g_strcmp0 (child_node->word, word) == 0) {
      break;
    }
    child_node = child_node->sibling;
  }
  if (child_node) {
    record->deep++;
    if (child_node->code) {
      record->num = record->deep;
      record->code = child_node->code;
    }
    wubi_article_query_node (child_node, record, utf8str + len);
  }
}

struct query_record *
wubi_article_query (struct wubi_table *table, gchar *utf8str)
{
  struct query_record *record;
  gchar word[7];
  gulong utf8_len;
  gint len;
  struct tree_node *node;

  record = g_new0 (struct query_record, 1);
  utf8_len = g_utf8_strlen (utf8str, -1);
  if (utf8_len == 0) {
    return record;
  }
  g_utf8_strncpy (word, utf8str, 1);
  len = strlen (word);

  node = g_hash_table_lookup (table->ht, word);
  if (!node) {
    return record;
  }

  record->deep++;
  if (node->code) {
    record->num = record->deep;
    record->code = node->code;
  }

  wubi_article_query_node (node, record, utf8str + len);
  return record;
}

void
wubi_table_parse_file (struct wubi_table *table, gchar *path)
{
  FILE *fp;
  gchar buf[4096];
  gchar code[5];
  gchar word[4096];
  gulong count;
  gint index;
  gboolean begin = FALSE;

  fp = g_fopen (path, "r");
  while (fgets (buf, sizeof (buf), fp)) {
    index = strlen (buf) - 1;
    index = (index > 0) ? index : 0;
    if (buf[index] != '\n' || buf[0] == ' ') {
      g_error ("c-aries fault");
    }
    buf[index] = '\0';
    if (g_strcmp0 (buf, "BEGIN_TABLE") == 0) {
      begin = TRUE;
      continue;
    }
    if (!begin) {
      continue;
    }
    if (g_strcmp0 (buf, "END_TABLE") == 0) {
      break;
    }
    sscanf (buf, "%s %s %lu", code, word, &count);
    wubi_table_insert (table, g_strdup (word), g_strdup (code));
  }
  fclose (fp);
}
