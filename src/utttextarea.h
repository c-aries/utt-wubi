#ifndef __UTT_TEXT_AREA_H__
#define __UTT_TEXT_AREA_H__

#include <gtk/gtk.h>
#include "uttclassrecord.h"		/* FIXME: rename to uttclassrecoder.h */

G_BEGIN_DECLS

#define UTT_TYPE_TEXT_AREA	(utt_text_area_get_type ())
#define UTT_TEXT_AREA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), UTT_TYPE_TEXT_AREA, UttTextArea))
#define UTT_TEXT_AREA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), UTT_TYPE_TEXT_AREA, UttTextAreaClass))
#define UTT_IS_TEXT_AREA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), UTT_TYPE_TEXT_AREA))
#define UTT_IS_TEXT_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((kalss), UTT_TYPE_TEXT_AREA))
#define UTT_TEXT_AREA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), UTT_TYPE_TEXT_AREA, UttTextAreaClass))

#define UTT_TYPE_CLASS_MODE (utt_class_mode_get_type ())

typedef enum {
  UTT_CLASS_EXERCISE_MODE,
  UTT_CLASS_EXAM_MODE,
} UttClassMode;

typedef struct _UttTextArea UttTextArea;
typedef struct _UttTextAreaClass UttTextAreaClass;
typedef struct _UttTextAreaPrivate UttTextAreaPrivate;

struct _UttTextArea
{
  GtkWidget widget;
};

struct _UttTextAreaClass
{
  GtkWidgetClass parent_class;
};

GType utt_text_area_get_type (void) G_GNUC_CONST;
GtkWidget *utt_text_area_new (void);
void utt_text_area_reset (UttTextArea *area);
void utt_text_area_set_class_recorder (UttTextArea *area, UttClassRecord *record);
gboolean utt_text_area_set_text (UttTextArea *area, const gchar *text);
gboolean utt_text_area_set_leading_space (UttTextArea *area, const gchar *leading_space);
gboolean utt_text_area_set_mark (UttTextArea *area, const gchar *mark);
void utt_text_area_underscore_stop_timeout (UttTextArea *area);
void utt_text_area_underscore_start_timeout (UttTextArea *area);
void utt_text_area_underscore_restart_timeout (UttTextArea *area);
void utt_text_area_class_begin (UttTextArea *area);
void utt_text_area_class_end (UttTextArea *area);
gchar *utt_text_area_get_compare_text (UttTextArea *area);
gchar *utt_text_area_dup_strip_text (const gchar *orig_text);

GType utt_class_mode_get_type (void) G_GNUC_CONST;
UttClassMode utt_text_area_get_class_mode (UttTextArea *area);
void utt_text_area_set_class_mode (UttTextArea *area, UttClassMode mode);

G_END_DECLS

#endif
