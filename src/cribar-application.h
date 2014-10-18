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

#ifndef __CRIBAR_APPLICATION_H__
#define __CRIBAR_APPLICATION_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CRIBAR_TYPE_APPLICATION             (cribar_application_get_type())
#define CRIBAR_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),CRIBAR_TYPE_APPLICATION,CribarApplication))
#define CRIBAR_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),CRIBAR_TYPE_APPLICATION,CribarApplicationClass))
#define CRIBAR_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),CRIBAR_TYPE_APPLICATION))
#define CRIBAR_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),CRIBAR_TYPE_APPLICATION))
#define CRIBAR_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),CRIBAR_TYPE_APPLICATION,CribarApplicationClass))

typedef struct _CribarApplication        CribarApplication;
typedef struct _CribarApplicationClass   CribarApplicationClass;
typedef struct _CribarApplicationPrivate CribarApplicationPrivate;

struct _CribarApplication {
        GtkApplication parent;
        CribarApplicationPrivate *priv;
};

struct _CribarApplicationClass {
        GtkApplicationClass parent_class;
};

GType cribar_application_get_type (void) G_GNUC_CONST;

GtkApplication* cribar_application_new (void);

G_END_DECLS

#endif /* __CRIBAR_APPLICATION_H__ */
