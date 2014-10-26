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

#ifndef __CRIBAR_APPLICATION_WINDOW_H__
#define __CRIBAR_APPLICATION_WINDOW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CRIBAR_TYPE_APPLICATION_WINDOW             (cribar_application_window_get_type())
#define CRIBAR_APPLICATION_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),CRIBAR_TYPE_APPLICATION_WINDOW,CribarApplicationWindow))
#define CRIBAR_APPLICATION_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),CRIBAR_TYPE_APPLICATION_WINDOW,CribarApplicationWindowClass))
#define CRIBAR_IS_APPLICATION_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),CRIBAR_TYPE_APPLICATION_WINDOW))
#define CRIBAR_IS_APPLICATION_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),CRIBAR_TYPE_APPLICATION_WINDOW))
#define CRIBAR_APPLICATION_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),CRIBAR_TYPE_APPLICATION_WINDOW,CribarApplicationWindowClass))

typedef struct _CribarApplicationWindow        CribarApplicationWindow;
typedef struct _CribarApplicationWindowClass   CribarApplicationWindowClass;
typedef struct _CribarApplicationWindowPrivate CribarApplicationWindowPrivate;

struct _CribarApplicationWindow {
        GtkApplicationWindow parent;
        CribarApplicationWindowPrivate *priv;
};

struct _CribarApplicationWindowClass {
        GtkApplicationWindowClass parent_class;
};

GType        cribar_application_window_get_type  (void) G_GNUC_CONST;

GtkWidget*   cribar_application_window_new       (GtkApplication *application);
void         cribar_application_window_set_photo (CribarApplicationWindow *self, GFile *photo);

G_END_DECLS

#endif /* __CRIBAR_APPLICATION_WINDOW_H__ */
