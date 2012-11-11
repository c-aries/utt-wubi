#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
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

static void
test_dir ()
{
}

void
utt_module_test ()
{
  DIR *dir;
  struct dirent *dirp;
  char path[4096];
  int len;

  dir = opendir (UTT_MODULE_PATH);
  strncpy (path, UTT_MODULE_PATH, 4096);
  normalize_dirname (path, 4096);
  if ((dirp = readdir (dir))) {
    if (dirp->d_type == DT_DIR) {
      len = strlen (path);
      strncat (path, dirp->d_name, 4096);
      normalize_dirname (path, 4096);
      test_dir ();
      path[len] = '\0';
    }
  }
  closedir (dir);
}
