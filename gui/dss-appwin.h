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
