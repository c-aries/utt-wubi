#include <glib.h>
#include <cairo.h>
#include "zigen_images.h"

/* generate by zigen/main.sh */

#define ZIGEN_IMG_PREFIX    "zigen/png/"
#define ZIGEN_FILENAME(name, index)	name##_png[index]
#define ZIGEN_SIZE(name)	G_N_ELEMENTS (name##_png)

static gchar *A_png[9] = {
    "A00.png",
    "A01.png",
    "A02.png",
    "A03.png",
    "A04.png",
    "A05.png",
    "A06.png",
    "A07.png",
    "A08.png",
};
static gchar *B_png[10] = {
    "B00.png",
    "B01.png",
    "B02.png",
    "B03.png",
    "B04.png",
    "B05.png",
    "B06.png",
    "B07.png",
    "B08.png",
    "B09.png",
};
static gchar *C_png[6] = {
    "C00.png",
    "C01.png",
    "C02.png",
    "C03.png",
    "C04.png",
    "C05.png",
};
static gchar *D_png[12] = {
    "D00.png",
    "D01.png",
    "D02.png",
    "D03.png",
    "D04.png",
    "D05.png",
    "D06.png",
    "D07.png",
    "D08.png",
    "D09.png",
    "D10.png",
    "D11.png",
};
static gchar *E_png[12] = {
    "E00.png",
    "E01.png",
    "E02.png",
    "E03.png",
    "E04.png",
    "E05.png",
    "E06.png",
    "E07.png",
    "E08.png",
    "E09.png",
    "E10.png",
    "E11.png",
};
static gchar *F_png[8] = {
    "F00.png",
    "F01.png",
    "F02.png",
    "F03.png",
    "F04.png",
    "F05.png",
    "F06.png",
    "F07.png",
};
static gchar *G_png[5] = {
    "G00.png",
    "G01.png",
    "G02.png",
    "G03.png",
    "G04.png",
};
static gchar *H_png[11] = {
    "H00.png",
    "H01.png",
    "H02.png",
    "H03.png",
    "H04.png",
    "H05.png",
    "H06.png",
    "H07.png",
    "H08.png",
    "H09.png",
    "H10.png",
};
static gchar *I_png[10] = {
    "I00.png",
    "I01.png",
    "I02.png",
    "I03.png",
    "I04.png",
    "I05.png",
    "I06.png",
    "I07.png",
    "I08.png",
    "I09.png",
};
static gchar *J_png[9] = {
    "J00.png",
    "J01.png",
    "J02.png",
    "J03.png",
    "J04.png",
    "J05.png",
    "J06.png",
    "J07.png",
    "J08.png",
};
static gchar *K_png[3] = {
    "K00.png",
    "K01.png",
    "K02.png",
};
static gchar *L_png[9] = {
    "L00.png",
    "L01.png",
    "L02.png",
    "L03.png",
    "L04.png",
    "L05.png",
    "L06.png",
    "L07.png",
    "L08.png",
};
static gchar *M_png[6] = {
    "M00.png",
    "M01.png",
    "M02.png",
    "M03.png",
    "M04.png",
    "M05.png",
};
static gchar *N_png[11] = {
    "N00.png",
    "N01.png",
    "N02.png",
    "N03.png",
    "N04.png",
    "N05.png",
    "N06.png",
    "N07.png",
    "N08.png",
    "N09.png",
    "N10.png",
};
static gchar *O_png[5] = {
    "O00.png",
    "O01.png",
    "O02.png",
    "O03.png",
    "O04.png",
};
static gchar *P_png[6] = {
    "P00.png",
    "P01.png",
    "P02.png",
    "P03.png",
    "P04.png",
    "P05.png",
};
static gchar *Q_png[12] = {
    "Q00.png",
    "Q01.png",
    "Q02.png",
    "Q03.png",
    "Q04.png",
    "Q05.png",
    "Q06.png",
    "Q07.png",
    "Q08.png",
    "Q09.png",
    "Q10.png",
    "Q11.png",
};
static gchar *R_png[9] = {
    "R00.png",
    "R01.png",
    "R02.png",
    "R03.png",
    "R04.png",
    "R05.png",
    "R06.png",
    "R07.png",
    "R08.png",
};
static gchar *S_png[3] = {
    "S00.png",
    "S01.png",
    "S02.png",
};
static gchar *T_png[9] = {
    "T00.png",
    "T01.png",
    "T02.png",
    "T03.png",
    "T04.png",
    "T05.png",
    "T06.png",
    "T07.png",
    "T08.png",
};
static gchar *U_png[10] = {
    "U00.png",
    "U01.png",
    "U02.png",
    "U03.png",
    "U04.png",
    "U05.png",
    "U06.png",
    "U07.png",
    "U08.png",
    "U09.png",
};
static gchar *V_png[6] = {
    "V00.png",
    "V01.png",
    "V02.png",
    "V03.png",
    "V04.png",
    "V05.png",
};
static gchar *W_png[4] = {
    "W00.png",
    "W01.png",
    "W02.png",
    "W03.png",
};
static gchar *X_png[6] = {
    "X00.png",
    "X01.png",
    "X02.png",
    "X03.png",
    "X04.png",
    "X05.png",
};
static gchar *Y_png[10] = {
    "Y00.png",
    "Y01.png",
    "Y02.png",
    "Y03.png",
    "Y04.png",
    "Y05.png",
    "Y06.png",
    "Y07.png",
    "Y08.png",
    "Y09.png",
};

/* return static character string */
static gchar *
get_zigen_filename (gchar ch, gint i)
{
  gchar *ret = NULL;

  switch (ch) {
  case 'a':
  case 'A':
    ret = ZIGEN_FILENAME (A, i);
    break;
  case 'b':
  case 'B':
    ret = ZIGEN_FILENAME (B, i);
    break;
  case 'c':
  case 'C':
    ret = ZIGEN_FILENAME (C, i);
    break;
  case 'd':
  case 'D':
    ret = ZIGEN_FILENAME (D, i);
    break;
  case 'e':
  case 'E':
    ret = ZIGEN_FILENAME (E, i);
    break;
  case 'f':
  case 'F':
    ret = ZIGEN_FILENAME (F, i);
    break;
  case 'g':
  case 'G':
    ret = ZIGEN_FILENAME (G, i);
    break;
  case 'h':
  case 'H':
    ret = ZIGEN_FILENAME (H, i);
    break;
  case 'i':
  case 'I':
    ret = ZIGEN_FILENAME (I, i);
    break;
  case 'j':
  case 'J':
    ret = ZIGEN_FILENAME (J, i);
    break;
  case 'k':
  case 'K':
    ret = ZIGEN_FILENAME (K, i);
    break;
  case 'l':
  case 'L':
    ret = ZIGEN_FILENAME (L, i);
    break;
  case 'm':
  case 'M':
    ret = ZIGEN_FILENAME (M, i);
    break;
  case 'n':
  case 'N':
    ret = ZIGEN_FILENAME (N, i);
    break;
  case 'o':
  case 'O':
    ret = ZIGEN_FILENAME (O, i);
    break;
  case 'p':
  case 'P':
    ret = ZIGEN_FILENAME (P, i);
    break;
  case 'q':
  case 'Q':
    ret = ZIGEN_FILENAME (Q, i);
    break;
  case 'r':
  case 'R':
    ret = ZIGEN_FILENAME (R, i);
    break;
  case 's':
  case 'S':
    ret = ZIGEN_FILENAME (S, i);
    break;
  case 't':
  case 'T':
    ret = ZIGEN_FILENAME (T, i);
    break;
  case 'u':
  case 'U':
    ret = ZIGEN_FILENAME (U, i);
    break;
  case 'v':
  case 'V':
    ret = ZIGEN_FILENAME (V, i);
    break;
  case 'w':
  case 'W':
    ret = ZIGEN_FILENAME (W, i);
    break;
  case 'x':
  case 'X':
    ret = ZIGEN_FILENAME (X, i);
    break;
  case 'y':
  case 'Y':
    ret = ZIGEN_FILENAME (Y, i);
    break;
  default:
    break;
  }
  return ret;
}

static gint
get_zigen_size (gchar ch)
{
  gint ret = -1;

  switch (ch) {
  case 'a':
  case 'A':
    ret = ZIGEN_SIZE (A);
    break;
  case 'b':
  case 'B':
    ret = ZIGEN_SIZE (B);
    break;
  case 'c':
  case 'C':
    ret = ZIGEN_SIZE (C);
    break;
  case 'd':
  case 'D':
    ret = ZIGEN_SIZE (D);
    break;
  case 'e':
  case 'E':
    ret = ZIGEN_SIZE (E);
    break;
  case 'f':
  case 'F':
    ret = ZIGEN_SIZE (F);
    break;
  case 'g':
  case 'G':
    ret = ZIGEN_SIZE (G);
    break;
  case 'h':
  case 'H':
    ret = ZIGEN_SIZE (H);
    break;
  case 'i':
  case 'I':
    ret = ZIGEN_SIZE (I);
    break;
  case 'j':
  case 'J':
    ret = ZIGEN_SIZE (J);
    break;
  case 'k':
  case 'K':
    ret = ZIGEN_SIZE (K);
    break;
  case 'l':
  case 'L':
    ret = ZIGEN_SIZE (L);
    break;
  case 'm':
  case 'M':
    ret = ZIGEN_SIZE (M);
    break;
  case 'n':
  case 'N':
    ret = ZIGEN_SIZE (N);
    break;
  case 'o':
  case 'O':
    ret = ZIGEN_SIZE (O);
    break;
  case 'p':
  case 'P':
    ret = ZIGEN_SIZE (P);
    break;
  case 'q':
  case 'Q':
    ret = ZIGEN_SIZE (Q);
    break;
  case 'r':
  case 'R':
    ret = ZIGEN_SIZE (R);
    break;
  case 's':
  case 'S':
    ret = ZIGEN_SIZE (S);
    break;
  case 't':
  case 'T':
    ret = ZIGEN_SIZE (T);
    break;
  case 'u':
  case 'U':
    ret = ZIGEN_SIZE (U);
    break;
  case 'v':
  case 'V':
    ret = ZIGEN_SIZE (V);
    break;
  case 'w':
  case 'W':
    ret = ZIGEN_SIZE (W);
    break;
  case 'x':
  case 'X':
    ret = ZIGEN_SIZE (X);
    break;
  case 'y':
  case 'Y':
    ret = ZIGEN_SIZE (Y);
    break;
  default:
    break;
  }
  return ret;
}

static void
free_zigen_img (gpointer data)
{
  struct zigen_img *img = data;
  gint num = img->num;
  gint i;

  for (i = 0; i < num; i++) {
    cairo_surface_destroy (img->img[i]);
  }
  g_free (img);
}

void
load_zigen_images (struct zigen_images *images)
{
  struct zigen_img *img;
  gchar ch;
  gint i;
  gchar *path;

  images->char_ht = g_hash_table_new_full (g_direct_hash, g_direct_equal,
					  NULL, free_zigen_img); /* FIXME: g_free */
  for (ch = 'a'; ch <= 'y'; ch++) {
    img = g_new (struct zigen_img, 1);
    img->num = get_zigen_size (ch);
    img->img = (cairo_surface_t **)g_new (cairo_surface_t *, img->num);
    for (i = 0; i < img->num; i++) {
      path = g_strdup_printf ("%s%s", ZIGEN_IMG_PREFIX, get_zigen_filename (ch, i));
      img->img[i] = cairo_image_surface_create_from_png (path);
      g_free (path);
    }
    g_hash_table_insert (images->char_ht, GINT_TO_POINTER ((gint)ch), img);
  }
}

void
free_zigen_images (struct zigen_images *images)
{
  g_hash_table_destroy (images->char_ht);
  images->char_ht = NULL;
}

#ifdef DEBUG_MAIN
int
main (int argc, char *argv[])
{
  struct zigen_images images;
  struct zigen_img *img;

  load_zigen_images (&images);
  g_print ("%s, %d, %d, %d\n", ZIGEN_FILENAME (B, 3), ZIGEN_SIZE (B), 'a', 'A');
  img = g_hash_table_lookup (images.char_ht, GINT_TO_POINTER ((gint)'c'));
  if (img) {
    g_print (">> %d\n", img->num);
  }
  free_zigen_images (&images);
  return 0;
}
#endif
