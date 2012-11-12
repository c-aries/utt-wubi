#ifndef __UTT_MODULE_H__
#define __UTT_MODULE_H__

enum utt_module_type {
  UTT_MODULE_INPUT_METHOD_TYPE,
  UTT_MODULE_CLASS_TYPE,
  UTT_MODULE_INVALIDATE_TYPE,
};

struct utt_class_module {
  int (*class_num) (void);
  char *(*nth_class_name) (int nth);
};
struct utt_module {
  char *module_name;
  enum utt_module_type module_type;
  char *(*locale_name) (void);
  void *priv_data;
};

struct utt_modules {
  void *data;
};

void utt_module_test ();
struct utt_modules *utt_modules_new ();
void utt_modules_destroy (struct utt_modules *modules);
void utt_modules_scan (struct utt_modules *modules);

#endif
