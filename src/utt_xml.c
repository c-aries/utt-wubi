#include "utt_xml.h"

struct utt_xml *
utt_xml_new ()
{
  return g_new0 (struct utt_xml, 1);
}

void
utt_xml_destroy (struct utt_xml *xml)
{
  g_free (xml->filepath);
  g_free (xml->title);
  g_free (xml->content);
  g_free (xml);
}

static gboolean
parse_title (struct utt_xml *xml, xmlTextReaderPtr reader)
{
  const xmlChar *name, *value;

  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  if (xmlTextReaderNodeType (reader) == XML_READER_TYPE_TEXT &&
      xmlTextReaderHasValue (reader)) {
    value = xmlTextReaderConstValue (reader);
    xml->title = g_strdup ((const gchar *)value);
  }
  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  name = xmlTextReaderConstName (reader);
  if (xmlStrcmp (name, BAD_CAST "Title") == 0 &&
      xmlTextReaderNodeType (reader) == XML_READER_TYPE_END_ELEMENT) {
    return TRUE;
  }
  return FALSE;
}

static gboolean
parse_content (struct utt_xml *xml, xmlTextReaderPtr reader)
{
  const xmlChar *name, *value;

  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  if (xmlTextReaderNodeType (reader) == XML_READER_TYPE_TEXT &&
      xmlTextReaderHasValue (reader)) {
    value = xmlTextReaderConstValue (reader);
    xml->content = g_strdup ((const gchar *)value);
  }
  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  name = xmlTextReaderConstName (reader);
  if (xmlStrcmp (name, BAD_CAST "Content") == 0 &&
      xmlTextReaderNodeType (reader) == XML_READER_TYPE_END_ELEMENT) {
    return TRUE;
  }
  return FALSE;
}

static gboolean
parse_article (struct utt_xml *xml, xmlTextReaderPtr reader)
{
  const xmlChar *name;

  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  name = xmlTextReaderConstName (reader);
  if (xmlStrcmp (name, BAD_CAST "Article") != 0 ||
      xmlTextReaderNodeType (reader) != XML_READER_TYPE_ELEMENT) {
    return FALSE;
  }

  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  name = xmlTextReaderConstName (reader);
  if (xmlStrcmp (name, BAD_CAST "Title") == 0 &&
      xmlTextReaderNodeType (reader) == XML_READER_TYPE_ELEMENT) {
    if (!parse_title (xml, reader)) {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }

  if (!xmlTextReaderRead (reader)) {
    return FALSE;
  }
  name = xmlTextReaderConstName (reader);
  if (xmlStrcmp (name, BAD_CAST "Content") == 0 &&
      xmlTextReaderNodeType (reader) == XML_READER_TYPE_ELEMENT) {
    if (!parse_content (xml, reader)) {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }

  if (xmlStrcmp (name, BAD_CAST "Article") != 0 ||
      xmlTextReaderNodeType (reader) != XML_READER_TYPE_END_ELEMENT) {
    return FALSE;
  }
  return TRUE;
}

gboolean
utt_parse_xml (struct utt_xml *xml, gchar *filename)
{
  xmlTextReaderPtr reader;
  gboolean ret;

  reader = xmlReaderForFile (filename, "UTF-8", 0);
  ret = parse_article (xml, reader);
  xml->filepath = g_strdup (filename);
  xmlFreeTextReader (reader);
  return ret;
}

gchar *
utt_xml_get_title (struct utt_xml *xml)
{
  return xml->title;
}

gchar *
utt_xml_get_content (struct utt_xml *xml)
{
  return xml->content;
}

gchar *
utt_xml_get_filepath (struct utt_xml *xml)
{
  return xml->filepath;
}

void
utt_xml_write (struct utt_xml *xml, const gchar *filename, const gchar *title, const gchar *content)
{
  xmlTextWriterPtr ptr;

  ptr = xmlNewTextWriterFilename (filename, 0);
  xmlTextWriterStartDocument (ptr, NULL, "UTF-8", NULL);
  xmlTextWriterStartElement (ptr, BAD_CAST "Article");
  xmlTextWriterWriteFormatElement (ptr, BAD_CAST "Title", "%s", title);
  xmlTextWriterWriteFormatElement (ptr, BAD_CAST "Content", "%s", content);
  xmlTextWriterEndElement (ptr);
  xmlTextWriterEndDocument (ptr);
  xmlFreeTextWriter (ptr);
}
