/*  vim: set sts=2 sw=2 et :
 *
 *  Copyright (C) 2014-16 Nirbheek Chauhan <nirbheek.chauhan@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef __DSS_APP_H__
#define __DSS_APP_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DSS_TYPE_APP         (dss_app_get_type ())
#define DSS_APP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DSS_TYPE_APP, DssApp))
#define DSS_APP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), DSS_TYPE_APP, DssAppClass))
#define DSS_IS_APP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DSS_TYPE_APP))
#define DSS_IS_APP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DSS_TYPE_APP))
#define DSS_APP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DSS_TYPE_APP, DssAppClass))

typedef struct _DssApp        DssApp;
typedef struct _DssAppClass   DssAppClass;
typedef struct _DssAppPrivate DssAppPrivate;

GType               dss_app_get_type          (void) G_GNUC_CONST;
DssApp*             dss_app_new               (void);
GstElement*         dss_app_get_pipeline      (DssApp *app);
GtkWidget*          dss_app_get_video_widget  (DssApp *app);
void                dss_app_quit              (DssApp *app);

G_END_DECLS

#endif /* __DSS_APP_H__ */
