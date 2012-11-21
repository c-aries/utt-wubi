#include <glib.h>
#include <utt/keyboard.h>

#define UTT_KEYBOARD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), UTT_TYPE_KEYBOARD, UttKeyboardPrivate))
G_DEFINE_TYPE (UttKeyboard, utt_keyboard, GTK_TYPE_WIDGET)

typedef struct _UttKeyboardPrivate UttKeyboardPrivate;
struct _UttKeyboardPrivate {
  void *data;
};

static void
utt_keyboard_finalize (GObject *object)
{
  /* UttKeyboard *kb = UTT_KEYBOARD (object); */
  /* UttKeyboardPrivate *priv = UTT_KEYBOARD_GET_PRIVATE (kb); */

  G_OBJECT_CLASS (utt_keyboard_parent_class)->finalize (object);
}

static void
utt_keyboard_realize (GtkWidget *widget)
{
  UttKeyboard *kb = UTT_KEYBOARD (widget);
  GdkWindowAttr attributes;
  gint attributes_mask;

  if (!gtk_widget_get_has_window (widget)) {
    GTK_WIDGET_CLASS (utt_keyboard_parent_class)->realize (widget);
  }
  else {
    gtk_widget_set_realized (widget, TRUE);
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				     &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, kb);
    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_INSENSITIVE);
  }
}

static void
utt_keyboard_unrealize (GtkWidget *widget)
{
  GTK_WIDGET_CLASS (utt_keyboard_parent_class)->unrealize (widget);
}

static void
utt_keyboard_class_init (UttKeyboardClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  g_type_class_add_private (gobject_class, sizeof (UttKeyboardPrivate));
  gobject_class->finalize = utt_keyboard_finalize;

  widget_class->realize = utt_keyboard_realize;
  widget_class->unrealize = utt_keyboard_unrealize;
}

static void
utt_keyboard_init (UttKeyboard *kb)
{
  GtkWidget *widget = GTK_WIDGET (kb);
  UttKeyboardPrivate *priv;

  gtk_widget_set_can_focus (widget, TRUE);
  priv = UTT_KEYBOARD_GET_PRIVATE (kb);
  priv->data = NULL;
}

GtkWidget *
utt_keyboard_new (void)
{
  return g_object_new (UTT_TYPE_KEYBOARD, NULL);
}
