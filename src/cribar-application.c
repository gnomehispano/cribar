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

static void cribar_application_activate   (GApplication         *application);
static void cribar_application_startup   (GApplication         *application);

struct _CribarApplicationPrivate {
        GtkWidget *window;
        guint32 activation_timestamp;
        GFile *file_inbox;
        GFileEnumerator *file_enumerator;
        GFile *current_photo;
};

G_DEFINE_TYPE_WITH_CODE (CribarApplication, cribar_application, GTK_TYPE_APPLICATION, G_ADD_PRIVATE (CribarApplication));

/* Code taken from eog-list-store.c in "eog" GNOME project (GPLv2), so, thanks to the EOG hackers (Claudio Saavedra)! */
static GList *supported_mime_types = NULL;

static gint     cribar_application_compare_quarks           (gconstpointer a, gconstpointer b);
static GList*   cribar_application_get_supported_mime_types (void);
static gboolean cribar_application_is_supported_mime_type   (const char *mime_type);
static void     cribar_application_populate_inbox           (CribarApplication *self);
static gboolean cribar_application_check_file               (GFileInfo  *file_info);
static void     cribar_application_next_photo               (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     cribar_application_discard_photo            (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     cribar_application_accept_photo             (GSimpleAction *action, GVariant *parameter, gpointer user_data);

static GActionEntry app_entries[] = {
        { "next", cribar_application_next_photo },
        { "discard", cribar_application_discard_photo },
        { "accept", cribar_application_accept_photo }
};

static void
cribar_application_class_init (CribarApplicationClass *klass)
{
        GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

        application_class->activate = cribar_application_activate;
        application_class->startup = cribar_application_startup;
}

static void
cribar_application_init (CribarApplication *self)
{
        CribarApplicationPrivate *priv = cribar_application_get_instance_private(self);

        self->priv = priv;

        priv->window = NULL;
        priv->activation_timestamp = GDK_CURRENT_TIME;
        priv->file_enumerator = NULL;

        g_action_map_add_action_entries (G_ACTION_MAP (self),
                                         app_entries, G_N_ELEMENTS (app_entries),
                                         self);
}

static void
cribar_application_activate (GApplication *application)
{
        CribarApplication *self = CRIBAR_APPLICATION (application);

        if (self->priv->window == NULL) {
                self->priv->window = cribar_application_window_new (GTK_APPLICATION (self));
        }

        gtk_window_present_with_time (GTK_WINDOW (self->priv->window), self->priv->activation_timestamp);
        self->priv->activation_timestamp = GDK_CURRENT_TIME;

        cribar_application_populate_inbox (self);
}

static void
cribar_application_startup (GApplication *application)
{
        G_APPLICATION_CLASS (cribar_application_parent_class)->startup (application);

        g_object_set (G_OBJECT (gtk_settings_get_default()),
                      "gtk-application-prefer-dark-theme", TRUE,
                      NULL);
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

	if (!supported_mime_types) {
		format_list = gdk_pixbuf_get_formats ();

		for (it = format_list; it != NULL; it = it->next) {
                        gchar **mime_types;
                        gint i;

			mime_types = gdk_pixbuf_format_get_mime_types ((GdkPixbufFormat *) it->data);

			for (i = 0; mime_types[i] != NULL; i++) {
				supported_mime_types = g_list_prepend (supported_mime_types, g_strdup (mime_types[i]));
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

static gboolean
cribar_application_check_file (GFileInfo *file_info)
{
	/* GFile *child; */
	gboolean load_uri = FALSE;
	const char *mime_type, *name;

	mime_type = g_file_info_get_content_type (file_info);
	name = g_file_info_get_name (file_info);

        if (!g_str_has_prefix (name, ".")) {
		if (cribar_application_is_supported_mime_type (mime_type)) {
			load_uri = TRUE;
		}
	}

        return load_uri;
}

static void
cribar_application_populate_inbox (CribarApplication *self)
{
        gchar *inbox_path;

        inbox_path = g_build_path (G_DIR_SEPARATOR_S, g_get_home_dir (), "Inbox", NULL);
        g_return_if_fail (g_file_test (inbox_path, G_FILE_TEST_IS_DIR));
        self->priv->file_inbox = g_file_new_for_path (inbox_path);

        self->priv->file_enumerator = g_file_enumerate_children (self->priv->file_inbox,
                                                                 G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE "," G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                                 0,
                                                                 NULL,
                                                                 NULL);

        g_free (inbox_path);

        g_action_activate (g_action_map_lookup_action (G_ACTION_MAP (self), "next"), NULL);
}

static void
cribar_application_next_photo (__attribute__ ((unused)) GSimpleAction *action, __attribute__ ((unused)) GVariant *parameter, gpointer user_data)
{
        CribarApplication *self;
        GFileInfo *file_info;

        self = CRIBAR_APPLICATION (user_data);
        file_info = g_file_enumerator_next_file (self->priv->file_enumerator, NULL, NULL);
        if (file_info == NULL) {
                cribar_application_window_set_photo (CRIBAR_APPLICATION_WINDOW (self->priv->window), NULL);
                return;
        }

        if (cribar_application_check_file (file_info)) {
                self->priv->current_photo = g_file_get_child (self->priv->file_inbox, g_file_info_get_name (file_info));
                cribar_application_window_set_photo (CRIBAR_APPLICATION_WINDOW (self->priv->window), self->priv->current_photo);
        }

        g_object_unref (file_info);
}

static void
cribar_application_discard_photo (__attribute__ ((unused)) GSimpleAction *action, __attribute__ ((unused)) GVariant *parameter, gpointer user_data)
{
        CribarApplication *self;
        GFileInfo *file_info;
        gboolean can_trash, result;

        self = CRIBAR_APPLICATION (user_data);
        g_return_if_fail (self->priv->current_photo != NULL);

	file_info = g_file_query_info (self->priv->current_photo,
				       G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH,
				       0, NULL, NULL);
	if (file_info == NULL) {
                g_warning ("Couldn't access trash.");
                return;
	}

	can_trash = g_file_info_get_attribute_boolean (file_info, G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH);
	g_object_unref (file_info);
	if (can_trash) {
		result = g_file_trash (self->priv->current_photo, NULL, NULL);
		if (result == FALSE) {
                        g_warning ("Couldn't access trash.");
		}
	}

        g_object_unref (self->priv->current_photo);
        self->priv->current_photo = NULL;
        g_action_activate (g_action_map_lookup_action (G_ACTION_MAP (self), "next"), NULL);
}

static void
cribar_application_accept_photo (__attribute__ ((unused)) GSimpleAction *action, __attribute__ ((unused)) GVariant *parameter, gpointer user_data)
{
        CribarApplication *self;
        GFileInfo *file_info;
        gchar *name;
        gchar *new_path;
        GFile *destination_file;
        GError *error;

        self = CRIBAR_APPLICATION (user_data);
        g_return_if_fail (self->priv->current_photo != NULL);

        file_info = g_file_query_info (self->priv->current_photo,
                                       G_FILE_ATTRIBUTE_STANDARD_NAME,
                                       0, NULL, NULL);
        name = g_file_info_get_attribute_as_string (file_info, G_FILE_ATTRIBUTE_STANDARD_NAME);
        new_path = g_build_path (G_DIR_SEPARATOR_S,
                                 g_get_user_special_dir (G_USER_DIRECTORY_PICTURES),
                                 name,
                                 NULL);
        destination_file = g_file_new_for_path (new_path);
        if (g_file_move (self->priv->current_photo, destination_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error) != TRUE)
                g_warning ("Can't move the file %s to %s: %s\n", name, new_path, error->message);

        g_free (name);
        g_free (new_path);
        g_object_unref (self->priv->current_photo);
        self->priv->current_photo = NULL;
        g_action_activate (g_action_map_lookup_action (G_ACTION_MAP (self), "next"), NULL);        
}

GtkApplication*
cribar_application_new (void)
{
        return GTK_APPLICATION (g_object_new (CRIBAR_TYPE_APPLICATION, NULL));
}
