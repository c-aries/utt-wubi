#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <glib.h>
#include <utt/module.h>

static void
normalize_dirname (char *path, int array_size)
{
  int len;

  len = strlen (path);
  if (len < 1 || len + 1 >= array_size) {
    fprintf (stderr, "length error\n");
    exit (EXIT_FAILURE);
  }
  if (path[len - 1] != '/') {	/* FIXME: support GNU/BSD, but not WOE = = */
    path[len] = '/';
    path[len + 1] = '\0';
  }
}

static gboolean
validate_module_name (char *module_name)
{
  gchar **name;
  gint i, j, len;
  gboolean validate = TRUE;

  name = g_strsplit (module_name, "::", 0);
  for (i = 0; *(name + i); i++) {
    len = strlen (*(name + i));
    for (j = 0; j < len; j++) {
      if (!g_ascii_isalnum (*(*(name + i) + j))) {
	validate = FALSE;
	break;
      }
    }
    if (!validate) {
      g_warning ("invalidate module name: \"%s\"\n", module_name);
      break;
    }
  }
  g_strfreev (name);
  return validate;
}

void
utt_module_test ()
{
}

struct utt_modules *
utt_modules_new ()
{
  return g_new0 (struct utt_modules, 1);
}

void
utt_modules_destroy (struct utt_modules *modules)
{
  g_free (modules);
}

static void
utt_load_module (struct utt_modules *modules, char *path)
{
  struct utt_module *module;
  struct utt_class_module *class_module;
  void *handle;
  int i;

  handle = dlopen (path, RTLD_LAZY);
  module = dlsym (handle, "utt_module");
/*   printf ("loading module \"%s\" (%s)\n", */
/* 	  module->module_name, module->locale_name ()); */
  validate_module_name (module->module_name);
  if (module->module_type == UTT_MODULE_CLASS_TYPE) {
    class_module = module->priv_data;
    /* puts ("class list:"); */
    for (i = 0; i < class_module->class_num (); i++) {
      /* puts (class_module->nth_class_name (i)); */
    }
  }
  dlclose (handle);
}

static void
utt_modules_scan_dir (struct utt_modules *modules, char *path, int array_size)
{
  DIR *dir;
  struct dirent *dirp;
  int len, dname_len;
  /* FIXME: assert path is directory */

  normalize_dirname (path, array_size);
  dir = opendir (path);
  while ((dirp = readdir (dir))) {
    if (strncmp (dirp->d_name, ".", 1) == 0 ||
	strncmp (dirp->d_name, "..", 2) == 0) {
      continue;
    }
    if (dirp->d_type == DT_DIR) {
      len = strlen (path);
      strncat (path, dirp->d_name, array_size);
      normalize_dirname (path, array_size);
      utt_modules_scan_dir (modules, path, array_size);
      path[len] = '\0';
    }
    else if (dirp->d_type == DT_REG) {
      len = strlen (path);
      strncat (path, dirp->d_name, array_size);
      dname_len = strlen (dirp->d_name);
      if (dname_len > 3 &&
	  dirp->d_name[dname_len - 3] == '.' &&
	  dirp->d_name[dname_len - 2] == 's' &&
	  dirp->d_name[dname_len - 1] == 'o') {
	utt_load_module (modules, path);	/* load module now */
      }
      path[len] = '\0';
    }
  }
  closedir (dir);
}

void
utt_modules_scan (struct utt_modules *modules)
{
  char path[4096];

  strncpy (path, UTT_MODULE_PATH, 4096);
  utt_modules_scan_dir (modules, path, 4096);
}
