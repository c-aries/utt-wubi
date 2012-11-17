#ifndef __UTT_KEYBOARD_H__
#define __UTT_KEYBOARD_H__

struct utt_keyboard {
  void *data;
};

struct utt_keyboard *utt_keyboard_new ();
void utt_keyboard_destroy (struct utt_keyboard *kb);

#endif
