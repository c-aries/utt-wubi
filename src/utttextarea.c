#include <string.h>
#include <gtk/gtkprivate.h>
#include <gdk/gdkkeysyms.h>
#include "utttextarea.h"

enum {
  PROP_0,
  PROP_CLASS_MODE,
};

#define UTT_TEXT_AREA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), UTT_TYPE_TEXT_AREA, UttTextAreaPrivate))
G_DEFINE_TYPE (UttTextArea, utt_text_area, GTK_TYPE_WIDGET)

struct _UttTextAreaPrivate
{
  /* class recorder */
  UttClassRecord *record;
  /* display */
  gchar *text_buffer;
  gchar *text_base;
  gchar *text_cmp;
  gchar input_buffer[2048];
  gchar *input_base;
  gchar *input_ptr;
  /* mark */
  gboolean mark_show;
  gint timeout_id;
  gdouble mark_x, mark_y;
  /*  */
  gdouble font_height;
  /* input method */
  GtkIMContext *im_context;
  /* signals */
  gulong pause_id, resume_id;
  /* class mode */
  UttClassMode class_mode;
};

enum {
  CLASS_BEGIN,
  CLASS_END,
  STATISTICS,
  LAST_SIGNAL,
};
static guint signals[LAST_SIGNAL] = { 0 };

GType
utt_class_mode_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY (etype == 0)) {
        static const GEnumValue values[] = {
	  { UTT_CLASS_EXERCISE_MODE, "UTT_CLASS_EXERCISE_MODE", "exercise-mode" },
	  { UTT_CLASS_EXAM_MODE, "UTT_CLASS_EXAM_MODE", "exam-mode" },
	  { 0, NULL, NULL }
        };
        etype = g_enum_register_static (g_intern_static_string ("UttClassMode"), values);
  }
  return etype;
}

static gdouble
utt_text_area_get_font_height (GtkWidget *widget)
{
  PangoContext *context;
  PangoLayout *layout;
  PangoFontDescription *desc;
  gint height;
  gdouble fix_height;

  context = gtk_widget_get_pango_context (widget);
  if (!context) {
    return 0;
  }
  layout = pango_layout_new (context);
  desc = pango_font_description_from_string ("Monospace 10");
  pango_font_description_set_absolute_size (desc, 16 * PANGO_SCALE);
  pango_layout_set_font_description (layout, desc);

  pango_layout_set_text (layout, " ", -1);
  pango_layout_get_size (layout, NULL, &height);
  fix_height = (gdouble)height / PANGO_SCALE;

  g_object_unref (layout);
  pango_font_description_free (desc);
  return fix_height;
}

static gboolean
utt_text_area_underscore_on_timeout (gpointer data)
{
  UttTextArea *area = UTT_TEXT_AREA (data);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  priv->mark_show = !priv->mark_show;
  gtk_widget_queue_draw (GTK_WIDGET (area));
  return TRUE;
}

void
utt_text_area_underscore_stop_timeout (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  if (priv->timeout_id) {
    g_source_remove (priv->timeout_id);
    priv->timeout_id = 0;
    priv->mark_show = FALSE;
    gtk_widget_queue_draw (GTK_WIDGET (area));
  }
}

void
utt_text_area_underscore_start_timeout (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  utt_text_area_underscore_stop_timeout (area);  
  priv->mark_show = TRUE;
  gtk_widget_queue_draw (GTK_WIDGET (area));
  priv->timeout_id = g_timeout_add_seconds (1, utt_text_area_underscore_on_timeout, area);
}

void
utt_text_area_underscore_restart_timeout (UttTextArea *area)
{
  utt_text_area_underscore_start_timeout (area);
}

void
utt_text_area_class_begin (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  utt_text_area_reset (area);
  utt_text_area_underscore_start_timeout (area);
  utt_class_record_begin (priv->record);
  g_signal_emit (area, signals[CLASS_BEGIN], 0);
  g_signal_connect_swapped (priv->record, "class-pause",
			    G_CALLBACK (utt_text_area_underscore_stop_timeout), area);
  g_signal_connect_swapped (priv->record, "class-resume",
			    G_CALLBACK (utt_text_area_underscore_restart_timeout), area);
}

void
utt_text_area_class_end (UttTextArea *area)
{
  UttTextAreaPrivate *priv;

  g_return_if_fail (UTT_IS_TEXT_AREA (area));
  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  if (utt_class_record_end_with_check (priv->record)) {
    utt_text_area_underscore_stop_timeout (area);
    g_signal_emit (area, signals[STATISTICS], 0);
    g_signal_emit (area, signals[CLASS_END], 0);
    utt_class_record_handlers_disconnect (priv->record);
  }
}

static void
utt_text_area_update_im_location (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  GdkRectangle rect;

  rect.x = priv->mark_x;
  rect.y = priv->mark_y + priv->font_height;
  rect.width = 0;
  rect.height = 0;
  gtk_im_context_set_cursor_location (priv->im_context, &rect);
}

static void
utt_text_area_preedit_cb (GtkIMContext *context, UttTextArea *area)
{
  utt_text_area_update_im_location (area);
}

/* return value indicate should we end the class */
static gboolean
utt_text_area_handle_keyevent_unicode (UttTextArea *area, gunichar unicode)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  gunichar text_unicode;
  gchar word[4];
  gint ret;

  if (!g_unichar_isprint (unicode)) {
    return FALSE;
  }

  ret = g_unichar_to_utf8 (unicode, word);
  word[ret] = '\0';
  if (priv->input_ptr + ret <
      priv->input_buffer + G_N_ELEMENTS (priv->input_buffer)) {
    strcpy (priv->input_ptr, word);
    priv->input_ptr += ret;
    utt_class_record_type_inc (priv->record);
    text_unicode = g_utf8_get_char (priv->text_cmp);
    if (unicode == text_unicode) {
      utt_class_record_correct_inc (priv->record);
    }
    ret = g_unichar_to_utf8 (text_unicode, word);
    priv->text_cmp += ret;
    utt_text_area_underscore_restart_timeout (area);
    if (priv->text_base != NULL &&
	g_utf8_strlen (priv->input_base, -1) == g_utf8_strlen (priv->text_base, -1)) {
      return TRUE;
    }
  }
  g_signal_emit (area, signals[STATISTICS], 0);
  return FALSE;
}

static void
utt_text_area_commit_cb (GtkIMContext *context, const gchar *input_str, UttTextArea *area)
{
  gunichar unicode;
  const gchar *input_cur;
  gboolean class_should_end = FALSE;

  if (input_str == NULL || *input_str == '\0') {
    return;
  }
  for (input_cur = input_str;
       *input_cur != '\0' && !class_should_end;
       input_cur = g_utf8_next_char (input_cur)) {
    unicode = g_utf8_get_char (input_cur);
    class_should_end = utt_text_area_handle_keyevent_unicode (area, unicode);
  }
  if (class_should_end) {
    /* FIXME: when pop a dialog, you will get warning.
       it's scim bug? If you use ibus, it works fine. */
    utt_text_area_class_end (area);
  }
}

static gboolean
utt_text_area_input_expose_exceed (UttTextArea *area, PangoLayout *layout,
				   gchar *input_row_base,
				   gchar *text_row_base, gint text_num, gdouble text_width,
				   gdouble *exceed_text_start)
{
  gchar word[4];
  gchar *input_cur = input_row_base;
  gchar *text_cur = text_row_base;
  gint width, input_num, len;
  gdouble mark_width, input_width, temp_width, text_compare_width;
  gboolean is_last_row = FALSE;
  gboolean input_is_end = FALSE;

  g_utf8_strncpy (word, "_", 1);
  pango_layout_set_text (layout, word, -1);
  pango_layout_get_size (layout, &width, NULL);
  mark_width = (gdouble)width / PANGO_SCALE;

  if (*input_row_base == '\0') {
    return FALSE;
  }

  input_width = 0;
  text_compare_width = 0;
  for (input_num = 0;
       input_num < text_num && *input_cur != '\0';
       input_num++) {
    g_utf8_strncpy (word, text_cur, 1);
    len = strlen (word);
    pango_layout_set_text (layout, word, -1);
    pango_layout_get_size (layout, &width, NULL);
    temp_width = (gdouble)width / PANGO_SCALE;
    text_compare_width += temp_width;
    text_cur += len;
    g_utf8_strncpy (word, input_cur, 1);
    len = strlen (word);
    pango_layout_set_text (layout, word, -1);
    pango_layout_get_size (layout, &width, NULL);
    temp_width = (gdouble)width / PANGO_SCALE;
    input_width += temp_width;
    input_cur += len;
  }

  input_is_end = (input_num == text_num && *input_cur == '\0');
  if ((input_num < text_num) || input_is_end) {
    is_last_row = TRUE;
  }
  if (!is_last_row) {
    if (input_width > text_width) {
      if (exceed_text_start) {
	*exceed_text_start = text_width - input_width;
      }
      return TRUE;
    }
    else {
      if (exceed_text_start) {
	*exceed_text_start = 0;
      }
      return FALSE;
    }
  }
  else {			/* FIXME: is_last_row, display mark */
    if (input_is_end) {
      if (input_width > text_width) { /* input_is_end don't display mark */
	if (exceed_text_start) {
	  *exceed_text_start = text_width - input_width;
	}
	return TRUE;
      }
      else {
	if (exceed_text_start) {
	  *exceed_text_start = 0;
	}
	return FALSE;
      }
    }
    else {
      if (exceed_text_start) {
	*exceed_text_start = text_compare_width - input_width;
	if (input_width + mark_width > text_width) {
	  return TRUE;
	}
	else {
	  return FALSE;
	}
      }
    }
  }
  return FALSE;
}

static void
utt_text_area_finalize (GObject *object)
{
  UttTextArea *area = UTT_TEXT_AREA (object);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  if (priv->text_buffer) {
    g_free (priv->text_buffer);
  }
  g_object_unref (priv->im_context);
  utt_text_area_underscore_stop_timeout (area);
  G_OBJECT_CLASS (utt_text_area_parent_class)->finalize (object);
}

static void
utt_text_area_realize (GtkWidget *widget)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  GdkWindowAttr attributes;
  gint attributes_mask;

  if (!gtk_widget_get_has_window (widget)) {
    GTK_WIDGET_CLASS (utt_text_area_parent_class)->realize (widget);
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
    attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK;
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				     &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, area);
    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_INSENSITIVE);
  }
  gtk_im_context_set_client_window (priv->im_context, widget->window);
}

static void
utt_text_area_unrealize (GtkWidget *widget)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  gtk_im_context_set_client_window (priv->im_context, NULL);
  GTK_WIDGET_CLASS (utt_text_area_parent_class)->unrealize (widget);
}

static gboolean
utt_text_area_key_press (GtkWidget *widget, GdkEventKey *event)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  gunichar unicode, text_unicode;
  gboolean class_should_end = FALSE;

  if (!utt_class_record_has_begin (priv->record)) {
    return TRUE;
  }

  if (event->keyval == GDK_Pause) {
    /* FIXME */
    return TRUE;
  }
  if (event->keyval == GDK_BackSpace &&
      utt_text_area_get_class_mode (area) == UTT_CLASS_EXERCISE_MODE) {
    if (priv->text_cmp > priv->text_base) {
      priv->input_ptr = g_utf8_prev_char (priv->input_ptr);
      unicode = g_utf8_get_char (priv->input_ptr);
      priv->text_cmp = g_utf8_prev_char (priv->text_cmp);
      text_unicode = g_utf8_get_char (priv->text_cmp);
      utt_class_record_type_dec (priv->record);
      if (unicode == text_unicode) {
	utt_class_record_correct_dec (priv->record);
      }
      *priv->input_ptr = '\0';
      utt_text_area_underscore_restart_timeout (area);
    }
    g_signal_emit (area, signals[STATISTICS], 0);
    return TRUE;
  }
  if (gtk_im_context_filter_keypress (priv->im_context, event)) {
    return TRUE;
  }

  unicode = gdk_keyval_to_unicode (event->keyval);
  class_should_end = utt_text_area_handle_keyevent_unicode (area, unicode);
  if (class_should_end) {
    utt_text_area_class_end (area);
    return TRUE;
  }
  return FALSE;
}

static gboolean
utt_text_area_key_release (GtkWidget *widget, GdkEventKey *event)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  if (gtk_im_context_filter_keypress (priv->im_context, event)) {
    return TRUE;
  }
  return GTK_WIDGET_CLASS (utt_text_area_parent_class)->key_release_event (widget, event);
}

static gboolean
utt_text_area_focus_in (GtkWidget *widget, GdkEventFocus *focus)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  gtk_im_context_focus_in (priv->im_context);
  return FALSE;
}

static gboolean
utt_text_area_focus_out (GtkWidget *widget, GdkEventFocus *focus)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  gtk_im_context_focus_out (priv->im_context);
  return FALSE;
}

static gboolean
utt_text_area_expose (GtkWidget *widget, GdkEventExpose *event)
{
  UttTextArea *area = UTT_TEXT_AREA (widget);
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  PangoLayout *layout;
  PangoFontDescription *desc;
  cairo_t *cr;
  gdouble text_x, text_y, input_x, input_y, temp_width;
  gdouble text_width;
  gint expose_width, expose_height, width, len;
  gint input_num, row;
  gchar *draw_text, *cmp_input;
  gchar *input_cur, *text_cur, *input_row_base, *text_row_base;
  gchar word[4];
  gunichar text_ch, input_ch;
  gboolean is_exceed;
  GArray *text_array;
  struct text_record {
    gint num;
    gdouble width;
  } text_record;
  gint text_num = 0;
  gdouble exceed_text_start = 0;

  cr = gdk_cairo_create (event->window);
  cairo_set_source_rgba (cr, 1, 1, 1, 1);
  cairo_paint (cr);

  if (!priv->text_base) {	/* in case set NULL text */
    cairo_destroy (cr);
    return FALSE;
  }

  layout = pango_cairo_create_layout (cr);
  desc = pango_font_description_from_string ("Monospace 10");
  /* pango_font_description_set_weight (desc, PANGO_WEIGHT_BOLD); */
  pango_font_description_set_absolute_size (desc, 16 * PANGO_SCALE);
  pango_layout_set_font_description (layout, desc);

  text_x = text_y = 0;
  gdk_drawable_get_size (widget->window, &expose_width, &expose_height);
  draw_text = priv->text_base;
  cmp_input = priv->input_base;
  text_array = g_array_new (FALSE, TRUE, sizeof (struct text_record));
  memset (&text_record, 0, sizeof (struct text_record));

  /* draw text first */
  for (;;) {
    text_ch = g_utf8_get_char (draw_text);
    if (*cmp_input == '\0') {
      cairo_set_source_rgb (cr, 0, 0, 0);
    }
    else if (*cmp_input != '\0') {
      input_ch = g_utf8_get_char (cmp_input);
      if (text_ch == input_ch) {
	cairo_set_source_rgb (cr, 0, 1, 0);
      }
      else {
	cairo_set_source_rgb (cr, 1, 0, 0);
      }
      /* input and text compared */
      g_utf8_strncpy (word, cmp_input, 1);
      len = strlen (word);
      cmp_input += len;
    }
    g_utf8_strncpy (word, draw_text, 1);
    len = strlen (word);
    pango_layout_set_text (layout, word, -1);
    pango_layout_get_size (layout, &width, NULL);
    temp_width = (gdouble)width / PANGO_SCALE;
    if (text_x + temp_width < expose_width) {
      cairo_move_to (cr, text_x, text_y);
      pango_cairo_show_layout (cr, layout);
      draw_text += len;
      text_x += temp_width;
      text_record.num++;
      text_record.width += temp_width;
    }
    else {
      if (text_y + 4 * priv->font_height > expose_height) {
	/* waiting change for next page */
	break;
      }
      g_array_append_val (text_array, text_record);
      text_x = 0;
      text_y += 2 * priv->font_height;
      cairo_move_to (cr, text_x, text_y);
      pango_cairo_show_layout (cr, layout);
      draw_text += len;
      text_x += temp_width;
      text_record.num = 1;
      text_record.width = temp_width;
    }
    if (*draw_text == '\0') {
      break;
    }
  }
  g_array_append_val (text_array, text_record);

  /* draw input text below */
  input_x = 0;
  input_y = priv->font_height;
  input_row_base = input_cur = priv->input_base;	/* screen base */
  text_cur = priv->text_base;
  input_num = 0;
  row = 0;
  if (text_cur != NULL && *text_cur != '\0') {
    while (*input_cur != '\0' && *text_cur != '\0') { /* test *text_cur for security */
      if (input_num == 0) {					      /* FIXME: if havn't exceed row */
	text_num = g_array_index (text_array, struct text_record, row).num;
	text_width = g_array_index (text_array, struct text_record, row).width;
	exceed_text_start = 0;
	text_row_base = text_cur;
	is_exceed = utt_text_area_input_expose_exceed (area, layout, input_row_base,
						       text_row_base, text_num, text_width,
						       &exceed_text_start);
      }
      g_utf8_strncpy (word, input_cur, 1);
      input_ch = g_utf8_get_char (input_cur);
      text_ch = g_utf8_get_char (text_cur);
      if (input_ch == text_ch) {
	cairo_set_source_rgb (cr, 0, 1, 0);
      }
      else {
	cairo_set_source_rgb (cr, 1, 0, 0);
      }
      pango_layout_set_text (layout, word, -1);
      pango_layout_get_size (layout, &width, NULL);
      temp_width = (gdouble)width / PANGO_SCALE;
      cairo_move_to (cr, input_x + exceed_text_start, input_y);
      pango_cairo_show_layout (cr, layout);
      input_num++;
      input_cur = g_utf8_next_char (input_cur);
      text_cur = g_utf8_next_char (text_cur);
      if (input_num == text_num) {
	input_num = 0;
	row++;
	input_row_base = input_cur;
	input_x = 0;
	input_y += 2 * priv->font_height;
	is_exceed = FALSE;
	exceed_text_start = 0;
      }
      else {
	input_x += temp_width;
      }
    }
  }
  if (is_exceed) {};		/* to get rid of gcc warning */

  /* FIXME: display mark when class not end. */
  cairo_set_source_rgb (cr, 0, 0, 0);
  if (*text_cur == '\0') {
    g_utf8_strncpy (word, " ", 1);
    pango_layout_set_text (layout, word, -1);
    cairo_move_to (cr, input_x + exceed_text_start, input_y);
    pango_cairo_show_layout (cr, layout);
    priv->mark_show = FALSE;
  }
  else {
    if (priv->mark_show) {
      g_utf8_strncpy (word, "_", 1);
    }
    else {
      g_utf8_strncpy (word, " ", 1);
    }
    pango_layout_set_text (layout, word, -1);
    cairo_move_to (cr, input_x + exceed_text_start, input_y);
    pango_cairo_show_layout (cr, layout);
  }
  priv->mark_x = input_x + exceed_text_start;
  priv->mark_y = input_y;

  if (row == text_array->len && text_cur != NULL && *text_cur != '\0') {
    priv->text_base = text_cur;
    priv->input_base = input_cur;
    gtk_widget_queue_draw (widget);
  }

  g_array_free (text_array, TRUE);
  g_object_unref (layout);
  pango_font_description_free (desc);
  cairo_destroy (cr);
  return TRUE;
}

UttClassMode
utt_text_area_get_class_mode (UttTextArea *area)
{
  UttTextAreaPrivate *priv;

  g_return_val_if_fail (UTT_IS_TEXT_AREA (area), 0);

  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  return priv->class_mode;
}

void
utt_text_area_set_class_mode (UttTextArea *area, UttClassMode mode)
{
  UttTextAreaPrivate *priv;

  g_return_if_fail (UTT_IS_TEXT_AREA (area));

  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  if (priv->class_mode == mode) {
    return;
  }
  priv->class_mode = mode;
  g_object_notify (G_OBJECT (area), "class-mode");
}

static void
utt_text_area_get_property (GObject *object,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *pspec)
{
  UttTextArea *area = UTT_TEXT_AREA (object);

  switch (prop_id) {
  case PROP_CLASS_MODE:
    g_value_set_enum (value,
		      utt_text_area_get_class_mode (area));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
utt_text_area_set_property (GObject *object,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *pspec)
{
  UttTextArea *area = UTT_TEXT_AREA (object);

  switch (prop_id) {
  case PROP_CLASS_MODE:
    utt_text_area_set_class_mode (area, g_value_get_enum (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
utt_text_area_class_init (UttTextAreaClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  g_type_class_add_private (gobject_class, sizeof (UttTextAreaPrivate));
  gobject_class->finalize = utt_text_area_finalize;
  gobject_class->get_property = utt_text_area_get_property;
  gobject_class->set_property = utt_text_area_set_property;
  g_object_class_install_property (gobject_class,
				   PROP_CLASS_MODE,
				   g_param_spec_enum ("class-mode",
						      "Class mode",
						      "class mode, exercise or exam",
						      UTT_TYPE_CLASS_MODE,
						      UTT_CLASS_EXERCISE_MODE,
						      GTK_PARAM_READWRITE));

  widget_class->realize = utt_text_area_realize;
  widget_class->unrealize = utt_text_area_unrealize;
  widget_class->expose_event = utt_text_area_expose;
  widget_class->key_press_event = utt_text_area_key_press;
  widget_class->key_release_event = utt_text_area_key_release;
  widget_class->focus_in_event = utt_text_area_focus_in;
  widget_class->focus_out_event = utt_text_area_focus_out;

  signals[CLASS_BEGIN] =
    g_signal_new ("class-begin",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  signals[CLASS_END] =
    g_signal_new ("class-end",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  signals[STATISTICS] =
    g_signal_new ("statistics",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
}

static void
utt_text_area_init (UttTextArea *area)
{
  GtkWidget *widget = GTK_WIDGET (area);
  UttTextAreaPrivate *priv;

  gtk_widget_set_can_focus (widget, TRUE);

  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  priv->record = NULL;
  priv->text_buffer = priv->text_base = priv->text_cmp = NULL;
  memset (priv->input_buffer, 0, sizeof (priv->input_buffer));
  priv->input_ptr = priv->input_base = priv->input_buffer;
  priv->mark_show = TRUE;
  priv->mark_x = priv->mark_y = 0;
  priv->timeout_id = 0;
  priv->font_height = utt_text_area_get_font_height (GTK_WIDGET (area));

  priv->im_context = gtk_im_multicontext_new ();
  g_signal_connect (priv->im_context, "preedit-start", G_CALLBACK (utt_text_area_preedit_cb), area);
  g_signal_connect (priv->im_context, "commit", G_CALLBACK (utt_text_area_commit_cb), area);
}

GtkWidget *
utt_text_area_new (void)
{
  return g_object_new (UTT_TYPE_TEXT_AREA, NULL);
}

void
utt_text_area_set_class_recorder (UttTextArea *area, UttClassRecord *record)
{
  UttTextAreaPrivate *priv;

  g_return_if_fail (UTT_IS_TEXT_AREA (area));

  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  priv->record = record;
  utt_class_record_set_mode (priv->record, CLASS_ADVANCE_WITHOUT_CHECK);
  priv->pause_id = g_signal_connect_swapped (priv->record, "class-pause",
					     G_CALLBACK (utt_text_area_underscore_stop_timeout), area);
  priv->resume_id = g_signal_connect_swapped (priv->record, "class-resume",
					      G_CALLBACK (utt_text_area_underscore_restart_timeout), area);
}

gchar *
utt_text_area_dup_strip_text (const gchar *orig_text)
{
  gint orig_len = g_utf8_strlen (orig_text, -1);
  const gchar *orig_p = orig_text;
  gunichar unicode;
  gchar *text = g_malloc0 ((strlen (orig_text) + 1) * sizeof (gchar));
  gchar *text_p = text;
  const gchar *copy_base = NULL;
  gint i, base_i, save_i, j;
  gchar *ret = NULL;

  for (base_i = save_i = i = 0, copy_base = NULL; i < orig_len;) {
    while (i < orig_len) {
      unicode = g_utf8_get_char (orig_p);
      if (g_unichar_isspace (unicode)) {
	orig_p = g_utf8_next_char (orig_p);
	i++;
	continue;
      }
      copy_base = orig_p;
      base_i = save_i = i;
      break;
    }
    orig_p = g_utf8_next_char (orig_p);
    i++;
    /* i >= orig_len or has a word */
    for (; i < orig_len;
	 i++, orig_p = g_utf8_next_char (orig_p)) {
      unicode = g_utf8_get_char (orig_p);
      if (!g_unichar_isspace (unicode)) {
	save_i = i;
      }
      if (unicode == '\n') {
	break;
      }
    }
    if (copy_base) {
      g_utf8_strncpy (text_p, copy_base, save_i - base_i + 1);
      for (j = 0; j < save_i - base_i + 1; j++) { /* FIXME */
	text_p = g_utf8_next_char (text_p);
      }
      if (unicode == '\n') {
	g_utf8_strncpy (text_p, "!", 1);
	text_p = g_utf8_next_char (text_p);
      }
      copy_base = NULL;
    }
  }
  text_p = g_utf8_prev_char (text_p);
  unicode = g_utf8_get_char (text_p);
  if (unicode == '!') {
    *text_p = '\0';
  }
  ret = g_strdup (text);
  g_free (text);
  return ret;
}

gboolean
utt_text_area_set_text (UttTextArea *area, const gchar *text)
{
  UttTextAreaPrivate *priv;

  g_return_val_if_fail (UTT_IS_TEXT_AREA (area) &&
			g_utf8_validate (text, -1, NULL), FALSE);

  priv = UTT_TEXT_AREA_GET_PRIVATE (area);
  g_return_val_if_fail (priv->record != NULL, FALSE);

  if (priv->text_buffer) {
    g_free (priv->text_buffer);
  }
  priv->text_cmp = priv->text_base = priv->text_buffer = g_strdup (text);
  if (text == NULL) {
    utt_class_record_set_total (priv->record, 0);
  }
  else {
    utt_class_record_set_total (priv->record, g_utf8_strlen (text, -1));
  }
  return TRUE;
}

void
utt_text_area_reset (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  priv->text_cmp = priv->text_base = priv->text_buffer;
  memset (priv->input_buffer, 0, sizeof (priv->input_buffer));
  priv->input_ptr = priv->input_base = priv->input_buffer;
  priv->mark_show = TRUE;
  priv->mark_x = priv->mark_y = 0;
  utt_class_record_set_mode (priv->record, CLASS_ADVANCE_WITHOUT_CHECK);
}

gchar *
utt_text_area_get_compare_text (UttTextArea *area)
{
  UttTextAreaPrivate *priv = UTT_TEXT_AREA_GET_PRIVATE (area);

  return priv->text_cmp;
}
