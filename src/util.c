#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "util.h"

#define DEBUG

static gboolean
process_exist (gchar *name)
{
  gint ret;
  gint status = -1;
  gchar *cmd;

  cmd = g_strdup_printf ("pgrep ^%s$ >/dev/null", name);	/* FIXME: depends on pgrep, is it great? */
  ret = system (cmd);
  if (WIFEXITED (ret)) {
    status = WEXITSTATUS (ret);
  }
  g_free (cmd);
  return status == 0;
}

enum im_type
check_current_input_method ()
{
  if (process_exist ("scim")) {
    return IM_SCIM;
  }
  else if (process_exist ("ibus-daemon")) {
    return IM_IBUS;
  }
  else if (process_exist ("fcitx")) {
    return IM_FCITX;
  }
  return IM_OTHER;
}
