#ifndef __UTT_ARTICLE_H__
#define __UTT_ARTICLE_H__

#include <glib.h>

gchar *utt_get_article_base_dir ();
gchar *utt_generate_new_article_path ();
gboolean utt_article_validate_title (const gchar *title);
gboolean utt_article_validate_content (const gchar *content);

#endif
