#ifndef __UTT_XML_H__
#define __UTT_XML_H__

#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <glib.h>

struct utt_xml {
  gchar *title;
  gchar *content;
};

struct utt_xml *utt_xml_new ();
void utt_xml_destroy (struct utt_xml *xml);
gboolean utt_parse_xml (struct utt_xml *xml, gchar *filename);
gchar *utt_xml_get_title (struct utt_xml *xml);
gchar *utt_xml_get_content (struct utt_xml *xml);
void utt_xml_write (struct utt_xml *xml, gchar *filename, const gchar *title, const gchar *content);

#endif

