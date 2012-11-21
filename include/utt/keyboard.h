#ifndef __UTT_KEYBOARD_H__
#define __UTT_KEYBOARD_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UTT_TYPE_KEYBOARD	(utt_keyboard_get_type ())
#define UTT_KEYBOARD(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), UTT_TYPE_KEYBOARD, UttKeyboard))
#define UTT_KEYBOARD_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), UTT_TYPE_KEYBOARD, UttKeyboardClass))
#define UTT_IS_KEYBOARD(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), UTT_TYPE_KEYBOARD))
#define UTT_IS_KEYBOARD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((kalss), UTT_TYPE_KEYBOARD))
#define UTT_KEYBOARD_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), UTT_TYPE_KEYBOARD, UttKeyboardClass))

typedef struct _UttKeyboard UttKeyboard;
typedef struct _UttKeyboardClass UttKeyboardClass;

struct _UttKeyboard
{
  GtkWidget widget;
};

struct _UttKeyboardClass
{
  GtkWidgetClass parent_class;
};

GType utt_keyboard_get_type (void) G_GNUC_CONST;
GtkWidget *utt_keyboard_new (void);

G_END_DECLS

#endif
