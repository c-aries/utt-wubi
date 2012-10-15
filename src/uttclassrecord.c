#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>
#include "uttclassrecord.h"

#define UTT_CLASS_RECORD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), UTT_TYPE_CLASS_RECORD, UttClassRecordPrivate))
G_DEFINE_TYPE (UttClassRecord, utt_class_record, G_TYPE_OBJECT)

struct class_stat {
  gint correct;			/* already correct num */
  gint typed;
  gint total;
  /* time */
  gint elapse;
  gint timer;
  GFunc timer_func;
  gpointer timer_func_data;
};
struct _UttClassRecordPrivate
{
  enum class_status status;
  enum class_mode mode;
  struct class_stat stat;
};

enum {
  CLASS_BEGIN,
  CLASS_END,
  CLASS_PAUSE,
  CLASS_RESUME,
  LAST_SIGNAL,
};
static guint signals[LAST_SIGNAL] = { 0 };

static void
utt_class_record_finalize (GObject *object)
{
  G_OBJECT_CLASS (utt_class_record_parent_class)->finalize (object);
}

static void
utt_class_record_class_init (UttClassRecordClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  g_type_class_add_private (gobject_class, sizeof (UttClassRecordPrivate));
  gobject_class->finalize = utt_class_record_finalize;

  signals[CLASS_BEGIN] =
    g_signal_new ("class-begin",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID, /* FIXME */
		  G_TYPE_NONE, 0);
  signals[CLASS_END] =
    g_signal_new ("class-end",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID, /* FIXME */
		  G_TYPE_NONE, 0);
  signals[CLASS_PAUSE] =
    g_signal_new ("class-pause",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID, /* FIXME */
		  G_TYPE_NONE, 0);
  signals[CLASS_RESUME] =
    g_signal_new ("class-resume",
		  G_OBJECT_CLASS_TYPE (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  0,
		  NULL, NULL,
		  gtk_marshal_VOID__VOID, /* FIXME */
		  G_TYPE_NONE, 0);
}

static void
utt_class_record_init (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->status = CLASS_STATUS_END;
  utt_class_record_set_mode (record, CLASS_ADVANCE_NEED_CORRECT);
}

UttClassRecord *
utt_class_record_new (void)
{
  return g_object_new (UTT_TYPE_CLASS_RECORD, NULL);
}

void
utt_class_record_end (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->status = CLASS_STATUS_END;
  if (priv->stat.timer) {
    g_source_remove (priv->stat.timer);
    priv->stat.timer = 0;
  }
  g_signal_emit (record, signals[CLASS_END], 0);
}

gint
utt_class_record_get_current (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  if (priv->mode == CLASS_ADVANCE_NEED_CORRECT) {
    return priv->stat.correct;
  }
  if (priv->mode == CLASS_ADVANCE_WITHOUT_CHECK) {
    return priv->stat.typed;
  }
  g_warning ("class mode incorrect\n");
  return -1;
}

gint
utt_class_record_stat_correct (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  return (gint)(priv->stat.typed ? 100 * priv->stat.correct * 1.0 / priv->stat.typed : 0);
}

gdouble
utt_class_record_stat_speed (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  return (priv->stat.elapse ? priv->stat.typed * 1.0 / priv->stat.elapse * 60 : 0);
}

gdouble
utt_class_record_stat_finish (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  if (priv->mode == CLASS_ADVANCE_NEED_CORRECT) {
    return (priv->stat.correct * 1.0 / priv->stat.total);
  }
  else if (priv->mode == CLASS_ADVANCE_WITHOUT_CHECK) {
    return (priv->stat.typed * 1.0 / priv->stat.total);
  }
  return 1;
}

void
utt_class_record_format_elapse_time (UttClassRecord *record, gint *hour, gint *min, gint *sec)
{
  UttClassRecordPrivate *priv;
  gint time;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  time = priv->stat.elapse;

  *sec = time % 60;
  time /= 60;
  *min = time % 60;
  time /= 60;
  *hour = time;
}

gint
utt_class_record_get_total (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  return priv->stat.total;
}

void
utt_class_record_set_total (UttClassRecord *record, gint total)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->stat.total = total;
  priv->stat.correct = priv->stat.typed = 0;
  if (priv->stat.timer) {
    g_source_remove (priv->stat.timer);
    priv->stat.timer = 0;
  }
}

void
utt_class_record_set_mode (UttClassRecord *record, enum class_mode mode)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->mode = mode;
}

static gchar *
format_time (gint time)
{
  gchar *str = NULL;
  gint sec, min, hour;

  sec = time % 60;
  time /= 60;
  min = time % 60;
  time /= 60;
  hour = time;
  str = g_strdup_printf ("%02d:%02d:%02d", hour, min, sec);
  return str;
}

static gboolean
on_stat_timeout (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  gchar *time;

  priv->stat.elapse++;
  if (priv->stat.timer_func) {
    priv->stat.timer_func(record, priv->stat.timer_func_data);
  }
  
  time = format_time (priv->stat.elapse);
  g_free (time);
  return TRUE;
}

void
utt_class_record_type_inc (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->stat.typed++;
}

void
utt_class_record_correct_inc (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->stat.correct++;
}

static gboolean
utt_class_record_can_end (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);  
  g_assert ((priv->mode == CLASS_ADVANCE_NEED_CORRECT &&
	     priv->stat.correct <= priv->stat.total) ||
	    (priv->mode == CLASS_ADVANCE_WITHOUT_CHECK &&
	     priv->stat.typed <= priv->stat.total));
  if (utt_class_record_has_begin (record) &&
      ((priv->mode == CLASS_ADVANCE_NEED_CORRECT &&
       priv->stat.correct == priv->stat.total) ||
      (priv->mode == CLASS_ADVANCE_WITHOUT_CHECK &&
       priv->stat.typed == priv->stat.total))) {
    return TRUE;
  }
  return FALSE;
}

gboolean
utt_class_record_end_with_check (UttClassRecord *record)
{
  g_return_val_if_fail (UTT_IS_CLASS_RECORD (record), FALSE);
  if (utt_class_record_can_end (record)) {
    utt_class_record_end (record);
    return TRUE;
  }
  return FALSE;
}

gboolean
utt_class_record_can_pause (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);  
  return priv->status == CLASS_STATUS_IN;
}

static void
utt_class_record_pause (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);  
  priv->status = CLASS_STATUS_PAUSE;
  if (priv->stat.timer) {
    g_source_remove (priv->stat.timer);
    priv->stat.timer = 0;
  }
  g_signal_emit (record, signals[CLASS_PAUSE], 0);
}

gboolean
utt_class_record_pause_with_check (UttClassRecord *record)
{
  if (utt_class_record_can_pause (record)) {
    utt_class_record_pause (record);
    return TRUE;
  }
  return FALSE;
}

static gboolean
utt_class_record_can_resume (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);  
  return priv->status == CLASS_STATUS_PAUSE;
}

static void
utt_class_record_resume (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);  
  priv->status = CLASS_STATUS_IN;
  priv->stat.timer = g_timeout_add_seconds (1, (GSourceFunc)on_stat_timeout, record);
  g_signal_emit (record, signals[CLASS_RESUME], 0);
}

gboolean
utt_class_record_resume_with_check (UttClassRecord *record)
{
  if (utt_class_record_can_resume (record)) {
    utt_class_record_resume (record);
    return TRUE;
  }
  return FALSE;
}

void
utt_class_record_begin (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  if (priv->stat.total <= 0) {
    g_warning ("total <= 0");
    return;
  }
  priv->stat.correct = priv->stat.typed = 0;
  priv->stat.elapse = 0;
  priv->status = CLASS_STATUS_IN;
  priv->stat.timer = g_timeout_add_seconds (1, (GSourceFunc)on_stat_timeout, record);
  g_signal_emit (record, signals[CLASS_BEGIN], 0);
}

gboolean
utt_class_record_has_begin (UttClassRecord *record)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  if (priv->status == CLASS_STATUS_END) {
    return FALSE;
  }
  return TRUE;
}

void
utt_class_record_set_timer_func (UttClassRecord *record, GFunc timer_func, gpointer data)
{
  UttClassRecordPrivate *priv;

  priv = UTT_CLASS_RECORD_GET_PRIVATE (record);
  priv->stat.timer_func = timer_func;
  priv->stat.timer_func_data = data;
}

void
utt_class_record_handlers_disconnect (UttClassRecord *record)
{
  gint i;

  for (i = CLASS_BEGIN; i < LAST_SIGNAL; i++) {
    g_signal_handlers_disconnect_matched (NULL, G_SIGNAL_MATCH_ID,
					  signals[i], 0, NULL, NULL, NULL);
  }
}
