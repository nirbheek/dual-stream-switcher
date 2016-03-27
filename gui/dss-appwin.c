/*  vim: set sts=2 sw=2 et :
 *
 *  Copyright (C) 2014-16 Nirbheek Chauhan <nirbheek@centricular.com>
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

#include "dss-lib.h"
#include "dss-app.h"
#include "dss-appwin.h"

struct _DssAppWindow
{
  GtkApplicationWindow parent;
};

struct _DssAppWindowClass
{
  GtkApplicationWindowClass parent_class;
};

struct _DssAppWindowPrivate
{
  GtkWidget *header_bar;
  GtkWidget *video_box;

  GstElement *pipeline;
  gboolean r_pressed;
  gboolean is_fullscreen;
};

G_DEFINE_TYPE_WITH_PRIVATE (DssAppWindow, dss_app_window,
    GTK_TYPE_APPLICATION_WINDOW);

static gboolean
on_key_event (DssAppWindow * win, GdkEventKey * e, DssAppWindowPrivate * priv)
{
  switch (e->keyval) {
    case GDK_KEY_q:
      if (e->type == GDK_KEY_PRESS) {
        GtkApplication *app = gtk_window_get_application (GTK_WINDOW (win));
        dss_app_quit (DSS_APP (app));
        return TRUE;
      }
      break;
    case GDK_KEY_r:
    case GDK_KEY_R:
      if (e->type == GDK_KEY_PRESS && !priv->r_pressed) {
        priv->r_pressed = TRUE;
        dss_pipeline_toggle_video (priv->pipeline);
        return TRUE;
      } else if (e->type != GDK_KEY_PRESS && priv->r_pressed) {
        priv->r_pressed = FALSE;
        dss_pipeline_toggle_video (priv->pipeline);
        return TRUE;
      }
      break;
    case GDK_KEY_f:
    case GDK_KEY_F:
      if (e->type == GDK_KEY_PRESS) {
        if (priv->is_fullscreen) {
          gtk_window_unfullscreen (GTK_WINDOW (win));
          priv->is_fullscreen = FALSE;
        } else {
          gtk_window_fullscreen (GTK_WINDOW (win));
          priv->is_fullscreen = TRUE;
        }
        return TRUE;
      }
      break;
  }

  return FALSE;
}

static gboolean
check_size_request (GtkWidget * video)
{
  gint width, height;

  gtk_widget_get_preferred_width (video, NULL, &width);
  gtk_widget_get_preferred_height (video, NULL, &height);
  if (width <= 10 || height <= 10)
    /* Widget still hasn't set the natural size */
    return G_SOURCE_CONTINUE;

  gtk_widget_set_size_request (video, width / 2, height / 2);

  return G_SOURCE_REMOVE;
}

static gboolean
setup_window (DssAppWindow * win)
{
  GtkWidget *video;
  GtkApplication *app;
  DssAppWindowPrivate *priv;

  app = gtk_window_get_application (GTK_WINDOW (win));
  priv = dss_app_window_get_instance_private (win);

  priv->pipeline = dss_app_get_pipeline (DSS_APP (app));
  g_assert (priv->pipeline != NULL);
  video = dss_app_get_video_widget (DSS_APP (app));
  gtk_container_add (GTK_CONTAINER (priv->video_box), video);
  gst_element_set_state (priv->pipeline, GST_STATE_PLAYING);

  gtk_widget_show (video);
  gtk_widget_add_events (GTK_WIDGET (win),
      GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
  g_signal_connect (win, "key-press-event", G_CALLBACK (on_key_event), priv);
  g_signal_connect (win, "key-release-event", G_CALLBACK (on_key_event), priv);

  /* HACK: gtksink sets the preferred size too late; after the allocation has
   * already been done, so the minimum size of 10x10 gets set as the natural
   * size and gtk_widget_queue_resize() never changes the allocation even after
   * the preferred size has changed */
  g_idle_add ((GSourceFunc) check_size_request, video);

  return G_SOURCE_REMOVE;
}

static void
dss_app_window_init (DssAppWindow * win)
{
  DssAppWindowPrivate *priv;

  gtk_widget_init_template (GTK_WIDGET (win));

  priv = dss_app_window_get_instance_private (win);

  gtk_header_bar_set_title (GTK_HEADER_BAR (priv->header_bar), "Playing");
  priv->r_pressed = FALSE;
  priv->is_fullscreen = FALSE;

  /* We can only initialize all this once the init is fully chained */
  g_idle_add ((GSourceFunc) setup_window, win);
}

static void
dss_app_window_class_init (DssAppWindowClass * class)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  gtk_widget_class_set_template_from_resource (widget_class,
      "/org/gtk/DualStreamSwitcher/dss-window.ui");
  gtk_widget_class_bind_template_child_private (widget_class, DssAppWindow,
      header_bar);
  gtk_widget_class_bind_template_child_private (widget_class, DssAppWindow,
      video_box);
}

GtkWidget *
dss_app_window_new (DssApp * app)
{
  return g_object_new (DSS_TYPE_APP_WINDOW, "application", app, NULL);
}
