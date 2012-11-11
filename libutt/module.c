#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <utt/module.h>

static void
utt_load_module (char *path)
{
  void *handle;
  struct utt_module *module;

  handle = dlopen (path, RTLD_LAZY);
  module = dlsym (handle, "utt_module");
  printf ("loading module \"%s\"\n", module->module_name);
  dlclose (handle);
}

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

static void
scan_dir (char *path, int array_size)
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
      scan_dir (path, array_size);
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
	utt_load_module (path);	/* load module now */
      }
      path[len] = '\0';
    }
  }
  closedir (dir);
}

void
utt_module_test ()
{
  char path[4096];

  strncpy (path, UTT_MODULE_PATH, 4096);
  scan_dir (path, 4096);
}
