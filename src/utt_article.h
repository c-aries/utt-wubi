#ifndef __UTT_ARTICLE_H__
#define __UTT_ARTICLE_H__

#include <glib.h>

enum article_add_result {
  ARTICLE_ADD_SUCCESS = 0,
  TITLE_INVALIDATE = 1 << 0,
  CONTENT_INVALIDATE = 1 << 1,
};

GList *utt_get_user_articles ();
enum article_add_result utt_add_article (const gchar *title, const gchar *content);

#endif
