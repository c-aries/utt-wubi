#include <glib.h>
#include <utt/module.h>
#include <utt/intl.h>

enum {
  CLASS_HENG,
  CLASS_SHU,
  CLASS_PIE,
  CLASS_NA,
  CLASS_ZHE,
  CLASS_ALL,
  CLASS_NUM,
};

static char *
locale_name ()
{
  return _("zigen");
}

static gint
class_num (void)
{
  return CLASS_NUM;
}

static gchar *
nth_class_name (gint nth)
{
  gchar *name = NULL;

  if (nth == CLASS_HENG) {
    name = _("HENG(ASDFG)");
  }
  else if (nth == CLASS_SHU) {
    name = _("SHU(HJKLM)");
  }
  else if (nth == CLASS_PIE) {
    name = _("PIE(QWERT)");
  }
  else if (nth == CLASS_NA) {
    name = _("NA(YUIOP)");
  }
  else if (nth == CLASS_ZHE) {
    name = _("ZHE(XCVBN)");
  }
  else if (nth == CLASS_ALL) {
    name = _("ALL");
  }
  return name;
}

struct utt_class_module class_module = {
  .class_num = class_num,
  .nth_class_name = nth_class_name,
};
struct utt_module utt_module = {
  .module_name = "wubi::zigen",
  .locale_name = locale_name,
  .module_type = UTT_MODULE_CLASS_TYPE,
  .priv_data = &class_module,
};
