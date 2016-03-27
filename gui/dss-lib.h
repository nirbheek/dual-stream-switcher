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


#ifndef __DSS_LIB_H__
#define __DSS_LIB_H__

#include <glib.h>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

gboolean      dss_pipeline_toggle_video     (GstElement *pipeline);
GstElement*   dss_pipeline_new              (GFile *file0,
                                             GFile *file1,
                                             GtkWidget **out_video_widget);

G_END_DECLS

#endif /* __DSS_LIB_H__ */
