#ifndef __UTT_MODULE_H__
#define __UTT_MODULE_H__

enum utt_module_type {
  UTT_MODULE_INVALIDATE_TYPE,
};

struct utt_module {
  char *module_name;
  enum utt_module_type module_type;
};

void utt_module_test ();

#endif
