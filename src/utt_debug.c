/* experiment code */
#include <iconv.h>
#include <glib.h>
#include "util.h"
#include "wubi_table.h"

static void
test_input_method ()
{
  check_current_input_method ();
}

static void
test_parse_scim_binary_table ()
{
  struct wubi_table *table;
  iconv_t conv;

  table = wubi_table_new ();
  wubi_table_parse_binary_file (table, "/usr/share/scim/tables/Wubi.bin");
  wubi_table_destroy (table);
  conv = iconv_open ("UCS-4LE", "UTF-8");
  if (conv == (iconv_t)-1) {
    g_error ("iconv_open fail");
  }
  iconv_close (conv);
}

void
utt_debug ()
{
  test_input_method ();
  test_parse_scim_binary_table ();
}
