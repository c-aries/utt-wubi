#ifndef __UTT_ARTICLE_H__
#define __UTT_ARTICLE_H__

#include <glib.h>

enum article_result {
  ARTICLE_ADD_SUCCESS = 0,
  ARTICLE_MODIFY_SUCCESS = 0,
  TITLE_INVALIDATE = 1 << 0,
  CONTENT_INVALIDATE = 1 << 1,
};

GList *utt_get_user_articles ();
enum article_result utt_add_article (const gchar *title, const gchar *content);
enum article_result utt_modify_article (const gchar *filepath, const gchar *title, const gchar *content);

#endif
