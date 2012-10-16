#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <uuid.h>
#include "utt_article.h"
#include "utt_xml.h"

/* return value should be free */
static gchar *
utt_get_article_base_dir ()
{
  gchar *dir;

  dir = g_build_path ("/", g_get_user_data_dir (), "utt", "wubi", "article", NULL);
  if (!g_file_test (dir, G_FILE_TEST_EXISTS)) {
    g_mkdir_with_parents (dir, S_IRWXU);
  }
  return dir;
}

GList *
utt_get_user_articles ()
{
  struct utt_xml *xml;
  GList *list = NULL;
  gchar *base_path, *path;
  const gchar *name;
  GDir *dir;
  GError *error;

  base_path = utt_get_article_base_dir ();
  dir = g_dir_open (base_path, 0, &error);
  while ((name = g_dir_read_name (dir))) {
    xml = utt_xml_new ();
    path = g_build_path ("/", base_path, name, NULL);
    utt_parse_xml (xml, path);
    list = g_list_prepend (list, xml);
    g_free (path);
  }
  g_dir_close (dir);
  g_free (base_path);
  return list;
}

static gchar *
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

static gboolean
utt_article_validate_title (const gchar *title)
{
  if (!title ||
      !g_utf8_validate (title, -1, NULL) ||
      g_utf8_strlen (title, -1) <= 0) {
    return FALSE;
  }
  return TRUE;
}

static gboolean
utt_article_validate_content (const gchar *content)
{
  if (!content ||
      !g_utf8_validate (content, -1, NULL) ||
      g_utf8_strlen (content, -1) <= 0) {
    return FALSE;
  }
  return TRUE;
}

gboolean
utt_add_article (const gchar *title, const gchar *content)
{
  struct utt_xml *xml;
  gchar *path;

  path = utt_generate_new_article_path ();
  xml = utt_xml_new ();
  if (!utt_article_validate_title (title) ||
      !utt_article_validate_content (content)) {
    return FALSE;
  }
  utt_xml_write (xml, path, title, content);
  utt_xml_destroy (xml);
  g_free (path);
  return TRUE;
}
