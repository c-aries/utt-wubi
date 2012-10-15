#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <uuid.h>
#include "utt_article.h"

/* return value should be free */
gchar *
utt_get_article_base_dir ()
{
  gchar *dir;

  dir = g_build_path ("/", g_get_user_data_dir (), "utt", "article", NULL);
  if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
    g_mkdir_with_parents (dir, S_IRWXU);
  }
  return dir;
}

gchar *
utt_generate_new_article_path ()
{
  uuid_t uuid;
  gchar uuid_str[37];
  gchar *path, *dir;

  uuid_generate (uuid);
  uuid_unparse (uuid, uuid_str);
  dir = utt_get_article_base_dir ();
  path = g_build_path ("/", dir, uuid_str, NULL);
  g_free (dir);
  return path;
}

gboolean
utt_article_validate_title (const gchar *title)
{
  return TRUE;
}

gboolean
utt_article_validate_content (const gchar *content)
{
  return TRUE;
}
