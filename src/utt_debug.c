/* experiment code */
#include <glib.h>
#include "util.h"

static void
test_input_method ()
{
  check_current_input_method ();
}

static void
test_parse_scim_binary_table ()
{
  gchar *path = "/usr/share/scim/tables/Wubi.bin";

  path = NULL;
}

void
utt_debug ()
{
  test_input_method ();
  test_parse_scim_binary_table ();
}
