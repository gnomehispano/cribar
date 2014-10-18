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

#include "cribar-application.h"
#include "cribar-application-window.h"

static void cribar_application_class_init (CribarApplicationClass *klass);
static void cribar_application_init       (CribarApplication      *self);
static void cribar_application_activate   (GApplication         *application);

struct _CribarApplicationPrivate {
        GtkWidget *window;
        guint32 activation_timestamp;
};

G_DEFINE_TYPE_WITH_CODE (CribarApplication, cribar_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE (CribarApplication));

static void
cribar_application_class_init (CribarApplicationClass *klass)
{
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        application_class->activate = cribar_application_activate;
}

static void
cribar_application_init (CribarApplication *self)
{
        CribarApplicationPrivate *priv = cribar_application_get_instance_private(self);

        priv->window = NULL;
        priv->activation_timestamp = GDK_CURRENT_TIME;
}

static void
cribar_application_activate (GApplication *application)
{
        CribarApplication *self = CRIBAR_APPLICATION (application);
        CribarApplicationPrivate *priv = cribar_application_get_instance_private (self);

        if (priv->window == NULL) {
                priv->window = cribar_application_window_new (GTK_APPLICATION (self));
        }

        gtk_window_present_with_time (GTK_WINDOW (priv->window), priv->activation_timestamp);
        priv->activation_timestamp = GDK_CURRENT_TIME;
}

GtkApplication*
cribar_application_new (void)
{
        return GTK_APPLICATION (g_object_new (CRIBAR_TYPE_APPLICATION, NULL));
}
