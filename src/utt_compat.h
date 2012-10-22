#ifndef __UTT_COMPAT_H__
#define __UTT_COMPAT_H__

/*
 * see https://developer.pidgin.im/doxygen/dev3.0.0/html/gtk3compat_8h_source.html.
 * Just to make old version gtk+-2.0 (GTK+ 2.18.9 on Slackware 13.1) to work.
 */

#if !GTK_CHECK_VERSION(2,20,0)

#define gtk_widget_get_mapped GTK_WIDGET_MAPPED
#define gtk_widget_set_mapped(widget, realized) do {         \
  if (realized)                                      \
    GTK_WIDGET_SET_FLAGS(widget, GTK_MAPPED);      \
  else                                        \
    GTK_WIDGET_UNSET_FLAGS(widget, GTK_MAPPED);    \
  } while(0)
#define gtk_widget_get_realized GTK_WIDGET_REALIZED
#define gtk_widget_set_realized(widget, realized) do {       \
  if (realized)                                      \
    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);    \
  else                                        \
    GTK_WIDGET_UNSET_FLAGS(widget, GTK_REALIZED);  \
  } while(0)

#endif /* 2.20.0 */

#endif
