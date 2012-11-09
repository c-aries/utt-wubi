/* experiment code */
#include "util.h"

static void
test_input_method ()
{
  check_current_input_method ();
}

static void
test_scim_binary_table ()
{
}

void
utt_debug ()
{
  test_input_method ();
  test_scim_binary_table ();
}
