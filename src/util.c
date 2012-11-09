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

  cmd = g_strdup_printf ("pgrep ^%s$ >/dev/null", name);	/* depends on pgrep, is it great? */
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
#ifdef DEBUG
    g_print ("%s\n", "scim");
#endif
    return IM_SCIM;
  }
  else if (process_exist ("ibus")) {
#ifdef DEBUG
    g_print ("%s\n", "ibus");
#endif
    return IM_IBUS;
  }
  else if (process_exist ("fcitx")) {
#ifdef DEBUG
    g_print ("%s\n", "fcitx");
#endif
    return IM_FCITX;
  }
#ifdef DEBUG
    g_print ("%s\n", "other");
#endif
  return IM_OTHER;
}
