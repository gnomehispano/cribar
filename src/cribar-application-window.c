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
};

G_DEFINE_TYPE_WITH_PRIVATE (CribarApplicationWindow, cribar_application_window, GTK_TYPE_APPLICATION_WINDOW);

static void
cribar_application_window_class_init (__attribute__ ((unused)) CribarApplicationWindowClass *klass)
{
}

static void
cribar_application_window_init (CribarApplicationWindow *self)
{
        self->priv = cribar_application_window_get_instance_private (self);

        self->priv->header_bar = gtk_header_bar_new ();
        gtk_header_bar_set_title (GTK_HEADER_BAR (self->priv->header_bar), _("Cribar"));
        gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (self->priv->header_bar), TRUE);
        gtk_window_set_titlebar (GTK_WINDOW (self), self->priv->header_bar);
        gtk_widget_show (self->priv->header_bar);

        self->priv->stack = gtk_stack_new ();
        gtk_container_add (GTK_CONTAINER (self), self->priv->stack);
        gtk_widget_show (self->priv->stack);
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
