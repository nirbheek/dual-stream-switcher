/*  vim: set sts=2 sw=2 et :
 *
 *  Copyright (C) 2014-16 Nirbheek Chauhan <nirbheek.chauhan@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
