#include <utt/module.h>
#include <utt/intl.h>

static char *
locale_name ()
{
  return _("wubi");
}

struct utt_module utt_module = {
  .module_name = "wubi",
  .locale_name = locale_name,
  .module_type = UTT_MODULE_INPUT_METHOD_TYPE,
  .priv_data = NULL,
};
