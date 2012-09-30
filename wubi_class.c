#include <glib.h>
#include <cairo.h>
#include "wubi_class.h"
#include "zigen_images.h"

#define SUBCLASS_NAME(name, index)	name##_subclass_name[index]
#define SUBCLASS_NUM(name)	G_N_ELEMENTS (name##_subclass_name)

static gchar *class_name[] = {
  "字根",
  "简码",
  "文章",
};
static gchar *zigen_subclass_name[] = {
  "横区(ASDFG)",
  "竖区(HJKLM)",
  "撇区(QWERT)",
  "捺区(YUIOP)",
  "折区(XCVBN)",
  "综合",
};
static gchar *jianma_subclass_name[] = {
  "一级简码",
};
static gchar *wenzhang_subclass_name[] = {
  "石苍舒醉墨堂",
};

static gchar *yiji[] = {
  "工",			/* a */
  "了",
  "以",
  "在",
  "有",
  "地",
  "一",
  "上",
  "不",
  "是",
  "中",
  "国",
  "同",
  "民",
  "为",
  "这",
  "我",
  "的",
  "要",
  "和",
  "产",
  "发",
  "人",
  "经",
  "主",			/* y */
};

void
free_zigen_chars (struct zigen_chars *chars)
{
  g_free (chars->ch);
  g_free (chars);
}

static struct zigen_chars *
gen_zigen_chars (GHashTable *ht, enum zigen_subclass_type type, gint char_num)
{
  struct zigen_chars *zchars;
  struct zigen_img *img;
  gint i;
  gchar class_char[N_SUBCLASS_TYPE - 2][5] = {
    "asdfg",			/* CLASS_TYPE_HENG */
    "hjklm",			/* CLASS_TYPE_SHU */
    "qwert",			/* CLASS_TYPE_PIE */
    "yuiop",			/* CLASS_TYPE_NA */
    "xcvbn",			/* CLASS_TYPE_ZHE */
  };
  
  
  zchars = g_new (struct zigen_chars, 1);
  zchars->num = char_num;
  zchars->ch = g_new0 (struct zigen_char, char_num);

  for (i = 0; i < 6; i++) {
    if (type == SUBCLASS_TYPE_ALL) {
      zchars->ch[i].value = class_char[g_random_int_range (0, SUBCLASS_TYPE_ZHE + 1)][g_random_int_range (0, 5)];
    }
    else {
      zchars->ch[i].value = class_char[type][g_random_int_range (0, 5)];
    }
    img = g_hash_table_lookup (ht, GINT_TO_POINTER ((gint)zchars->ch[i].value));
    if (img && img->num > 0) {
      zchars->ch[i].img = img->img[g_random_int_range (0, img->num)];
    }
  }
#ifdef DEBUG
  /* g_print ("%s\n", chars); */
#endif
  return zchars;
}

void
wubi_class_init (struct wubi_class *wubi)
{
  load_zigen_images (&wubi->zigen_images);
}

void
wubi_class_free (struct wubi_class *wubi)
{
  free_zigen_images (&wubi->zigen_images);
}

struct zigen_chars *
wubi_class_gen_zigen_chars (struct wubi_class *wubi,
			    enum zigen_subclass_type type,
			    gint char_num)
{
  return gen_zigen_chars (wubi->zigen_images.char_ht,
			  type,
			  char_num);
}

gchar *
wubi_class_get_jianma_by_char (gchar ch)
{
  return yiji[ch - 'a'];
}

/* need to free */
gchar *
wubi_class_gen_jianma_chars (struct wubi_class *wubi, gint char_num)
{
  gint i, index;
  gchar *ret;

  ret = g_malloc (sizeof (gchar) * char_num);
  for (i = 0; i < char_num; i++) {
    index = g_random_int_range (0, G_N_ELEMENTS (yiji));
    ret[i] = 'a' + index;
  }
  return ret;
}

/* need to free */
gchar *
wubi_class_gen_wenzhang_chars (struct wubi_class *wubi, gint subclass_id)
{
  return g_strdup (wubi_article_get_by_id (subclass_id));
}

static int
get_subclass_num (int class_type)
{
  int num = 0;

  switch (class_type) {
  case CLASS_TYPE_ZIGEN:
    num = SUBCLASS_NUM(zigen);
    break;
  case CLASS_TYPE_JIANMA:
    num = SUBCLASS_NUM(jianma);
    break;
  case CLASS_TYPE_WENZHANG:
    num = SUBCLASS_NUM(wenzhang);
    break;
  default:
    break;
  }
  return num;
}

static gchar *
get_subclass_name (int class_type, int subclass_index)
{
  gchar *name = NULL;

  switch (class_type) {
  case CLASS_TYPE_ZIGEN:
    name = SUBCLASS_NAME(zigen, subclass_index);
    break;
  case CLASS_TYPE_JIANMA:
    name = SUBCLASS_NAME(jianma, subclass_index);
    break;
  case CLASS_TYPE_WENZHANG:
    name = SUBCLASS_NAME(wenzhang, subclass_index);
    break;
  default:
    break;
  }
  return name;
}

int
wubi_class_get_class_num (struct wubi_class *wubi)
{
  return G_N_ELEMENTS (class_name);
}

gchar *
wubi_class_get_class_name (struct wubi_class *wubi,
			   int class_index)
{
  return class_name[class_index];
}

int
wubi_class_get_subclass_num (struct wubi_class *wubi, int class_type)
{
  return get_subclass_num (class_type);
}

gchar *
wubi_class_get_subclass_name (struct wubi_class *wubi,
			      int class_type,
			      int subclass_index)
{
  return get_subclass_name (class_type, subclass_index);
}
