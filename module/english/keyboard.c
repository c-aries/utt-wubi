#include <glib.h>
#include <utt/module.h>
#include <utt/intl.h>

static char *
locale_name ()
{
  return _("keyboard");
}

static gint
class_num (void)
{
  return 1;
}

static gchar *
nth_class_name (gint nth)
{
  if (nth == 0) {
    return _("alphabetic character");
  }
  return NULL;
}

struct utt_class_module class_module = {
  .class_num = class_num,
  .nth_class_name = nth_class_name,
};
struct utt_module utt_module = {
  .module_name = "english::keyboard",
  .locale_name = locale_name,
  .module_type = UTT_MODULE_CLASS_TYPE,
  .priv_data = &class_module,
};
