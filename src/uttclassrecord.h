#ifndef __UTT_CLASS_RECORD_H__
#define __UTT_CLASS_RECORD_H__

#include <glib-object.h>

G_BEGIN_DECLS

enum class_status {
  CLASS_STATUS_END,
  CLASS_STATUS_IN,		/* begin or resume */
  CLASS_STATUS_PAUSE,
};

enum class_mode {
  CLASS_ADVANCE_NEED_CORRECT,
  CLASS_ADVANCE_WITHOUT_CHECK,
};

#define UTT_TYPE_CLASS_RECORD	(utt_class_record_get_type ())
#define UTT_CLASS_RECORD(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), UTT_TYPE_CLASS_RECORD, UttClassRecord))
#define UTT_CLASS_RECORD_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), UTT_TYPE_CLASS_RECORD, UttClassRecordClass))
#define UTT_IS_CLASS_RECORD(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), UTT_TYPE_CLASS_RECORD))
#define UTT_IS_CLASS_RECORD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((kalss), UTT_TYPE_CLASS_RECORD))
#define UTT_CLASS_RECORD_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), UTT_TYPE_CLASS_RECORD, UttClassRecordClass))

typedef struct _UttClassRecord UttClassRecord;
typedef struct _UttClassRecordClass UttClassRecordClass;
typedef struct _UttClassRecordPrivate UttClassRecordPrivate;

struct _UttClassRecord
{
  GObject parent_instance;
};

struct _UttClassRecordClass
{
  GObjectClass parent_class;
};

GType utt_class_record_get_type (void) G_GNUC_CONST;
UttClassRecord *utt_class_record_new (void);
void utt_class_record_end (UttClassRecord *record);
gboolean utt_class_record_end_with_check (UttClassRecord *record);
gint utt_class_record_get_current (UttClassRecord *record);
gint utt_class_record_get_total (UttClassRecord *record);
void utt_class_record_set_total (UttClassRecord *record, gint total);
void utt_class_record_set_mode (UttClassRecord *record, enum class_mode mode);
void utt_class_record_begin (UttClassRecord *record);
gboolean utt_class_record_has_begin (UttClassRecord *record);
void utt_class_record_type_inc (UttClassRecord *record);
void utt_class_record_type_dec (UttClassRecord *record);
void utt_class_record_correct_inc (UttClassRecord *record);
void utt_class_record_correct_dec (UttClassRecord *record);
void utt_class_record_format_elapse_time (UttClassRecord *record,
					  gint *hour, gint *min, gint *sec);
gint utt_class_record_stat_correct (UttClassRecord *record);
gdouble utt_class_record_stat_speed (UttClassRecord *record);
gdouble utt_class_record_stat_finish (UttClassRecord *record);
gboolean utt_class_record_can_pause (UttClassRecord *record);
gboolean utt_class_record_pause_with_check (UttClassRecord *record);
gboolean utt_class_record_resume_with_check (UttClassRecord *record);
void utt_class_record_set_timer_func (UttClassRecord *record, GFunc timer_func, gpointer data);
void utt_class_record_handlers_disconnect (UttClassRecord *record);

G_END_DECLS

#endif
