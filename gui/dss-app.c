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

#include "dss-lib.h"
#include "dss-app.h"
#include "dss-appwin.h"

#define APPMENU_FILE "/org/gtk/DualStreamSwitcher/dss-appmenu.ui"

struct _DssApp
{
  GtkApplication parent;
};

struct _DssAppClass
{
  GtkApplicationClass parent_class;
};

struct _DssAppPrivate
{
  GstElement *pipeline;
  GtkWidget *video_widget;
  GtkWidget *window;
};

G_DEFINE_TYPE_WITH_PRIVATE (DssApp, dss_app, GTK_TYPE_APPLICATION);

static GOptionEntry app_options[] = {
  {NULL}
};

static void
quit_activated (GSimpleAction * action, GVariant * param, gpointer app)
{
  g_application_quit (G_APPLICATION (app));
}

static GActionEntry app_entries[] = {
  {"quit", quit_activated, NULL, NULL, NULL},
};

void
dss_app_quit (DssApp * app)
{
  g_action_group_activate_action (G_ACTION_GROUP (app), "quit", NULL);
}

static void
dss_app_init (DssApp * app)
{
  const gchar *name = "Dual stream switcher";
  g_set_prgname (name);
  g_set_application_name (name);
  gtk_window_set_default_icon_name (name);
  g_application_add_main_option_entries (G_APPLICATION (app), app_options);
}

static void
dss_app_activate (GApplication * app)
{
  g_printerr ("Must pass two files as arguments\n");
}

static void
dss_app_open (GApplication * app, GFile ** files, gint n_files,
    const gchar * hint)
{
  DssAppPrivate *priv;

  g_return_if_fail (DSS_IS_APP (app));

  if (n_files != 2) {
    g_printerr ("Must pass two files as arguments (got %i)\n", n_files);
    return;
  }

  priv = dss_app_get_instance_private (DSS_APP (app));
  if (DSS_IS_APP_WINDOW (priv->window))
    gtk_window_present (GTK_WINDOW (priv->window));

  priv->pipeline = dss_pipeline_new (files[0], files[1], &priv->video_widget);
  if (!priv->pipeline)
    return;

  priv->window = dss_app_window_new (DSS_APP (app));
  gtk_window_present (GTK_WINDOW (priv->window));
}

static void
dss_app_startup (GApplication * app)
{
  GtkBuilder *builder;
  GMenuModel *app_menu;
  const gchar *quit_accels[2] = { "<Ctrl>Q", NULL };

  G_APPLICATION_CLASS (dss_app_parent_class)->startup (app);

  /* Setup app menu and accels */
  g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries,
      G_N_ELEMENTS (app_entries), app);
  gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.quit",
      quit_accels);

  builder = gtk_builder_new_from_resource (APPMENU_FILE);
  app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));
  gtk_application_set_app_menu (GTK_APPLICATION (app), app_menu);
  g_object_unref (builder);

  /* Initialize gstreamer only once per application */
  gst_init (NULL, NULL);
}

static void
dss_app_shutdown (GApplication * app)
{
  DssAppPrivate *priv = dss_app_get_instance_private (DSS_APP (app));

  if (priv->pipeline != NULL)
    gst_element_set_state (priv->pipeline, GST_STATE_NULL);

  G_APPLICATION_CLASS (dss_app_parent_class)->shutdown (app);
}

static void
dss_app_dispose (GObject * object)
{
  DssAppPrivate *priv = dss_app_get_instance_private (DSS_APP (object));

  g_clear_object (&priv->window);
  g_clear_object (&priv->pipeline);

  G_OBJECT_CLASS (dss_app_parent_class)->dispose (object);
}

static void
dss_app_finalize (GObject * object)
{
  /* Free non-refcounted objects here */
  G_OBJECT_CLASS (dss_app_parent_class)->finalize (object);
}

static void
dss_app_class_init (DssAppClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GApplicationClass *application_class = G_APPLICATION_CLASS (class);

  application_class->open = dss_app_open;
  application_class->activate = dss_app_activate;
  application_class->startup = dss_app_startup;
  application_class->shutdown = dss_app_shutdown;

  object_class->dispose = dss_app_dispose;
  object_class->finalize = dss_app_finalize;
}

DssApp *
dss_app_new (void)
{
  return g_object_new (DSS_TYPE_APP, "application-id",
      "org.gtk.DualStreamSwitcher", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}

GstElement *
dss_app_get_pipeline (DssApp * app)
{
  DssAppPrivate *priv;

  g_return_val_if_fail (DSS_IS_APP (app), NULL);
  priv = dss_app_get_instance_private (DSS_APP (app));

  return priv->pipeline;
}

GtkWidget *
dss_app_get_video_widget (DssApp * app)
{
  DssAppPrivate *priv;

  g_return_val_if_fail (DSS_IS_APP (app), NULL);
  priv = dss_app_get_instance_private (DSS_APP (app));

  return priv->video_widget;
}
