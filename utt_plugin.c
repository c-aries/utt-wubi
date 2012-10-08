#include "utt_plugin.h"

struct utt_plugin_table *
utt_plugin_table_new ()
{
  struct utt_plugin_table *table;

  table = g_new0 (struct utt_plugin_table, 1);
  table->ht = g_hash_table_new (g_str_hash, g_str_equal);
  return table;
}

void
utt_plugin_table_destroy (struct utt_plugin_table *table)
{
  g_hash_table_unref (table->ht);
  g_list_free (table->list);
  g_free (table);
}

gboolean
utt_register_plugin (struct utt_plugin_table *table, struct utt_plugin *plugin)
{
  struct utt_plugin *ret;

  ret = g_hash_table_lookup (table->ht, plugin->plugin_name);
  if (ret) {
    return FALSE;
  }
  g_hash_table_insert (table->ht, plugin->plugin_name, plugin);
  table->list = g_list_append (table->list, plugin);
  table->num++;
  return TRUE;
}

struct utt_plugin *
utt_plugin_lookup (struct utt_plugin_table *table, gchar *plugin_name)
{
  return g_hash_table_lookup (table->ht, plugin_name);
}

struct utt_plugin *
utt_nth_plugin (struct utt_plugin_table *table, int n)
{
  return g_list_nth_data (table->list, n);
}
