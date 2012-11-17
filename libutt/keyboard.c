#include <glib.h>
#include <utt/keyboard.h>

struct utt_keyboard *
utt_keyboard_new ()
{
  return g_new0 (struct utt_keyboard, 1);
}

void
utt_keyboard_destroy (struct utt_keyboard *kb)
{
  g_free (kb);
}
