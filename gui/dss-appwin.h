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

#ifndef __DSS_APP_WINDOW_H__
#define __DSS_APP_WINDOW_H__

#include "dss-app.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define DSS_TYPE_APP_WINDOW         (dss_app_window_get_type ())
#define DSS_APP_WINDOW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), DSS_TYPE_APP_WINDOW, DssAppWindow))
#define DSS_APP_WINDOW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), DSS_TYPE_APP_WINDOW, DssAppWindowClass))
#define DSS_IS_APP_WINDOW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), DSS_TYPE_APP_WINDOW))
#define DSS_IS_APP_WINDOW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), DSS_TYPE_APP_WINDOW))
#define DSS_APP_WINDOW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), DSS_TYPE_APP_WINDOW, DssAppWindowClass))

typedef struct _DssAppWindow        DssAppWindow;
typedef struct _DssAppWindowClass   DssAppWindowClass;
typedef struct _DssAppWindowPrivate DssAppWindowPrivate;

GType         dss_app_window_get_type     (void) G_GNUC_CONST;
GtkWidget*    dss_app_window_new          (DssApp *application);

G_END_DECLS

#endif /* __DSS_APP_WINDOW_H__ */
