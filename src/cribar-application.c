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

#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

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

/* Code taken from eog-list-store.c in "eog" GNOME project (GPLv2), so, thanks to the EOG hackers (Claudio Saavedra)! */
static GList *supported_mime_types = NULL;

static gint     cribar_application_compare_quarks           (gconstpointer a, gconstpointer b);
static GList*   cribar_application_get_supported_mime_types (void);
static gboolean cribar_application_is_supported_mime_type   (const char *mime_type);
static void     cribar_application_populate_inbox           (CribarApplication *self);
static void     cribar_application_directory_visit          (GFile *directory, GFileInfo *children_info);

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
        /* g_get_user_special_dir */
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

        cribar_application_populate_inbox (self);
}

static gint
cribar_application_compare_quarks (gconstpointer a, gconstpointer b)
{
	GQuark quark;

	quark = g_quark_from_string ((const gchar *) a);

	return quark - GPOINTER_TO_INT (b);
}

static GList*
cribar_application_get_supported_mime_types (void)
{
	GSList *format_list, *it;
	gchar **mime_types;
	int i;

	if (!supported_mime_types) {
		format_list = gdk_pixbuf_get_formats ();

		for (it = format_list; it != NULL; it = it->next) {
			mime_types =
				gdk_pixbuf_format_get_mime_types ((GdkPixbufFormat *) it->data);

			for (i = 0; mime_types[i] != NULL; i++) {
				supported_mime_types =
					g_list_prepend (supported_mime_types,
							g_strdup (mime_types[i]));
			}

			g_strfreev (mime_types);
		}

		supported_mime_types = g_list_sort (supported_mime_types,
						    (GCompareFunc) cribar_application_compare_quarks);

		g_slist_free (format_list);
	}

	return supported_mime_types;
}

static gboolean
cribar_application_is_supported_mime_type (const char *mime_type)
{
	GList *supported_mime_types, *result;
	GQuark quark;

	if (mime_type == NULL) {
		return FALSE;
	}

	supported_mime_types = cribar_application_get_supported_mime_types ();

	quark = g_quark_from_string (mime_type);

	result = g_list_find_custom (supported_mime_types,
				     GINT_TO_POINTER (quark),
				     (GCompareFunc) cribar_application_compare_quarks);

	return (result != NULL);
}

static void
cribar_application_directory_visit (GFile *directory, GFileInfo *children_info)
{
	/* GFile *child; */
	gboolean load_uri = FALSE;
	const char *mime_type, *name;

	mime_type = g_file_info_get_content_type (children_info);
	name = g_file_info_get_name (children_info);

        if (!g_str_has_prefix (name, ".")) {
		if (cribar_application_is_supported_mime_type (mime_type)) {
			load_uri = TRUE;
		}
	}

	if (load_uri) {
		/* child = g_file_get_child (directory, name); */
		/* eog_list_store_append_image_from_file (store, child); */
                g_print ("Going to append: %s\n", name);
	}
}

static void
cribar_application_populate_inbox (CribarApplication *self)
{
        gchar *inbox_path;
        GFile *inbox_file;
	GFileEnumerator *file_enumerator;
        GFileInfo *file_info;

        inbox_path = g_build_path (G_DIR_SEPARATOR_S, g_get_home_dir (), "Inbox", NULL);
        g_print ("Inbox path: %s\n", inbox_path);
        g_return_if_fail (g_file_test (inbox_path, G_FILE_TEST_IS_DIR));
        inbox_file = g_file_new_for_path (inbox_path);

        file_enumerator = g_file_enumerate_children (inbox_file,
                                                     G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE "," G_FILE_ATTRIBUTE_STANDARD_NAME,
						     0,
                                                     NULL,
                                                     NULL);
        file_info = g_file_enumerator_next_file (file_enumerator, NULL, NULL);
        while (file_info != NULL) {
                cribar_application_directory_visit (inbox_file, file_info);
		g_object_unref (file_info);
		file_info = g_file_enumerator_next_file (file_enumerator, NULL, NULL);
        }

        g_object_unref (file_enumerator);
        g_free (inbox_path);
}

GtkApplication*
cribar_application_new (void)
{
        return GTK_APPLICATION (g_object_new (CRIBAR_TYPE_APPLICATION, NULL));
}
