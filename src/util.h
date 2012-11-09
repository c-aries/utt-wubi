#ifndef __UTIL_H__
#define __UTIL_H__

/* input method program type */
enum im_type {
  IM_SCIM,
  IM_IBUS,
  IM_FCITX,
  IM_OTHER,
};

enum im_type check_current_input_method ();

#endif
