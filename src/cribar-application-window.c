/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Cribar - Photos selection application for GNOME
 * Copyright © 2014 Álvaro Peña <alvaropg@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "config.h"

#include <glib/gi18n.h>

#include "cribar-application-window.h"

#define WINDOW_MIN_WIDTH 600
#define WINDOW_MIN_HEIGHT 400

static void cribar_application_window_class_init (CribarApplicationWindowClass *klass);
static void cribar_application_window_init       (CribarApplicationWindow *self);

struct _CribarApplicationWindowPrivate {
        GtkWidget *header_bar;
        GtkWidget *stack;
        GtkWidget *image_widget;
        GtkWidget *label_widget;
};

G_DEFINE_TYPE_WITH_PRIVATE (CribarApplicationWindow, cribar_application_window, GTK_TYPE_APPLICATION_WINDOW);

static void
cribar_application_window_class_init (__attribute__ ((unused)) CribarApplicationWindowClass *klass)
{
}

static void
cribar_application_window_init (CribarApplicationWindow *self)
{
        GtkWidget *widget;

        self->priv = cribar_application_window_get_instance_private (self);

        self->priv->header_bar = gtk_header_bar_new ();
        gtk_header_bar_set_title (GTK_HEADER_BAR (self->priv->header_bar), _("Cribar"));
        gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (self->priv->header_bar), TRUE);
        gtk_window_set_titlebar (GTK_WINDOW (self), self->priv->header_bar);
        gtk_widget_show (self->priv->header_bar);

        widget = gtk_button_new ();
        gtk_button_set_label (GTK_BUTTON (widget), "Accept");
        gtk_actionable_set_action_name (GTK_ACTIONABLE (widget), "app.accept");
        gtk_style_context_add_class (gtk_widget_get_style_context (widget), "suggested-action");
        gtk_header_bar_pack_start (GTK_HEADER_BAR (self->priv->header_bar), widget);
        gtk_widget_show (widget);

        widget = gtk_button_new_from_icon_name ("user-trash-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
        atk_object_set_name (gtk_widget_get_accessible (widget), "Trash");
        gtk_actionable_set_action_name (GTK_ACTIONABLE (widget), "app.discard");
        gtk_style_context_add_class (gtk_widget_get_style_context (widget), "destructive-action");
        gtk_header_bar_pack_end (GTK_HEADER_BAR (self->priv->header_bar), widget);
        gtk_widget_show (widget);

        self->priv->stack = gtk_stack_new ();
        gtk_container_add (GTK_CONTAINER (self), self->priv->stack);
        gtk_widget_show (self->priv->stack);

        self->priv->image_widget = gtk_image_new ();
        gtk_stack_add_named (GTK_STACK (self->priv->stack), self->priv->image_widget, "photo");
        gtk_widget_show (self->priv->image_widget);

        self->priv->label_widget = gtk_label_new ("<big>No more images</big>");
        gtk_label_set_use_markup (GTK_LABEL (self->priv->label_widget), TRUE);
        gtk_stack_add_named (GTK_STACK (self->priv->stack), self->priv->label_widget, "label");
        gtk_widget_show (self->priv->label_widget);
}

GtkWidget*
cribar_application_window_new (GtkApplication *application)
{
        return GTK_WIDGET (g_object_new (CRIBAR_TYPE_APPLICATION_WINDOW,
                                         "application", application,
                                         "title", _(PACKAGE_NAME),
                                         "width_request", WINDOW_MIN_WIDTH,
                                         "height_request", WINDOW_MIN_HEIGHT,
                                         "window-position", GTK_WIN_POS_CENTER,
                                         NULL));
}

void
cribar_application_window_set_photo (CribarApplicationWindow *self, GFile *photo)
{
        gchar *path;

        g_return_if_fail (CRIBAR_IS_APPLICATION_WINDOW (self));

        if (photo == NULL) {
                gtk_stack_set_visible_child_name (GTK_STACK (self->priv->stack), "label");
                return;
        }

        path = g_file_get_path (photo);
        if (path) {
                GdkPixbuf *original_pixbuf;
                GError *error = NULL;

                original_pixbuf = gdk_pixbuf_new_from_file (path, &error);
                if (error != NULL) {
                        g_warning ("Can't load image %s: %s", path, error->message);
                } else {
                        GFileInfo *file_info;
                        gchar *name;
                        GdkPixbuf *sized_pixbuf;

                        sized_pixbuf = gdk_pixbuf_scale_simple (original_pixbuf, 640, 480, GDK_INTERP_NEAREST);
                        gtk_image_set_from_pixbuf (GTK_IMAGE (self->priv->image_widget), sized_pixbuf);
                        g_object_unref (original_pixbuf);

                       	file_info = g_file_query_info (photo,
                                                       G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                       0, NULL, NULL);
                        name = g_file_info_get_attribute_as_string (file_info, G_FILE_ATTRIBUTE_STANDARD_NAME);
                        if (name) {
                                gtk_header_bar_set_title (GTK_HEADER_BAR (self->priv->header_bar), name);
                                g_free (name);
                        }
                }
                g_free (path);
        }
}
