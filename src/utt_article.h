#ifndef __UTT_ARTICLE_H__
#define __UTT_ARTICLE_H__

#include <glib.h>

GList *utt_get_user_articles ();
gboolean utt_add_article (const gchar *title, const gchar *content);

#endif
