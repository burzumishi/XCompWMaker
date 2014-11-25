/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-extension-loader.c
 * Copyright (C) Nicolas Bruguier 2007-2011 <gandalfn@club-internet.fr>
 * 
 * cairo-compmgr is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cairo-compmgr is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ccm-extension-loader.h"
#include "ccm-extension.h"
#include "ccm-window-plugin.h"
#include "ccm-screen-plugin.h"
#include "ccm-preferences-page-plugin.h"

static GSList *CCMPluginPath = NULL;

struct _CCMExtensionLoaderPrivate
{
    GSList *plugins;
};

#define CCM_EXTENSION_LOADER_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), CCM_TYPE_EXTENSION_LOADER, CCMExtensionLoaderPrivate))

G_DEFINE_TYPE (CCMExtensionLoader, ccm_extension_loader, G_TYPE_OBJECT);

static void
ccm_extension_loader_init (CCMExtensionLoader * self)
{
    self->priv = CCM_EXTENSION_LOADER_GET_PRIVATE (self);
    self->priv->plugins = NULL;
}

static void
ccm_extension_loader_finalize (GObject * object)
{
    CCMExtensionLoader *self = CCM_EXTENSION_LOADER (object);

    if (self->priv->plugins)
    {
        g_slist_foreach (self->priv->plugins, (GFunc) g_object_unref, NULL);
        g_slist_free (self->priv->plugins);
    }
    G_OBJECT_CLASS (ccm_extension_loader_parent_class)->finalize (object);
}

static void
ccm_extension_loader_class_init (CCMExtensionLoaderClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (CCMExtensionLoaderPrivate));

    object_class->finalize = ccm_extension_loader_finalize;
}


CCMExtensionLoader *
ccm_extension_loader_new ()
{
    static CCMExtensionLoader *self = NULL;

    if (!self)
    {
        self = g_object_new (CCM_TYPE_EXTENSION_LOADER, NULL);
        GDir *plugins_dir;
        gchar *filename;
        GSList *item;

        for (item = CCMPluginPath; item; item = item->next)
        {
            if ((plugins_dir = g_dir_open ((gchar *) item->data, 0, NULL)) == NULL)
                continue;
            while ((filename = (gchar *) g_dir_read_name (plugins_dir)) != NULL)
            {
                if (g_pattern_match_simple ("*.plugin", filename))
                {
                    CCMExtension *plugin;
                    gchar *file = g_build_filename ((gchar *) item->data,
                                                    filename, NULL);

                    if ((plugin = ccm_extension_new (file)) != NULL)
                    {
                        self->priv->plugins =
                            g_slist_insert_sorted (self->priv->plugins, plugin,
                                                   (GCompareFunc) _ccm_extension_compare);
                    }
                    g_free (file);
                }
            }
            g_dir_close (plugins_dir);
            self->priv->plugins = g_slist_sort (self->priv->plugins,
                                                (GCompareFunc) _ccm_extension_compare);
        }
    }

    return g_object_ref (self);
}

GSList *
ccm_extension_loader_get_preferences_plugins (CCMExtensionLoader * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    static GSList* preferences_plugins = NULL;

    if (preferences_plugins == NULL)
    {
        GSList *item;

        for (item = self->priv->plugins; item; item = item->next)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            if (g_type_is_a (plugin, CCM_TYPE_PREFERENCES_PAGE_PLUGIN))
            {
                preferences_plugins = g_slist_prepend (preferences_plugins, 
                                                       GINT_TO_POINTER (plugin));
            }
            preferences_plugins = g_slist_reverse (preferences_plugins);
        }
    }

    return preferences_plugins ? g_slist_copy (preferences_plugins) : NULL;
}

GSList *
ccm_extension_loader_get_screen_window_plugins (CCMExtensionLoader * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    static GSList* screen_window_plugins = NULL;

    if (screen_window_plugins == NULL)
    {
        GSList *item;

        for (item = self->priv->plugins; item; item = item->next)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            if (g_type_is_a (plugin, CCM_TYPE_SCREEN_PLUGIN) ||
                g_type_is_a (plugin, CCM_TYPE_WINDOW_PLUGIN))
            {
                screen_window_plugins = g_slist_prepend (screen_window_plugins,
                                                         item->data);
            }
        }

        screen_window_plugins = g_slist_reverse (screen_window_plugins);
    }

    return screen_window_plugins ? g_slist_copy (screen_window_plugins) : NULL;
}

GSList *
ccm_extension_loader_get_screen_plugins (CCMExtensionLoader * self,
                                         GSList * filter)
{
    g_return_val_if_fail (self != NULL, NULL);

    static GSList* screen_plugins = NULL;
    GSList *item, *plugins = NULL;

    if (screen_plugins == NULL)
    {
        for (item = self->priv->plugins; item; item = item->next)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            if (g_type_is_a (plugin, CCM_TYPE_SCREEN_PLUGIN))
            {
                screen_plugins = g_slist_prepend (screen_plugins, item->data);
            }
        }

        screen_plugins = g_slist_reverse (screen_plugins);
    }

    for (item = screen_plugins; item; item = item->next)
    {
        GSList *f;
        gboolean found = FALSE;

        for (f = filter; f; f = f->next)
        {
            if (!g_ascii_strcasecmp (f->data, 
                                     ccm_extension_get_label (item->data)))
            {
                found = TRUE;
                break;
            }
        }

        if (found)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            plugins = g_slist_prepend (plugins, GINT_TO_POINTER (plugin));
        }
    }

    return g_slist_reverse (plugins);
}

GSList *
ccm_extension_loader_get_window_plugins (CCMExtensionLoader * self,
                                         GSList * filter)
{
    g_return_val_if_fail (self != NULL, NULL);

    static GSList* window_plugins = NULL;
    GSList *item, *plugins = NULL;

    if (window_plugins == NULL)
    {
        for (item = self->priv->plugins; item; item = item->next)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            if (g_type_is_a (plugin, CCM_TYPE_WINDOW_PLUGIN))
            {
                window_plugins = g_slist_prepend (window_plugins, item->data);
            }
        }

        window_plugins = g_slist_reverse (window_plugins);
    }

    for (item = window_plugins; item; item = item->next)
    {
        GSList *f;
        gboolean found = FALSE;

        for (f = filter; f; f = f->next)
        {
            if (!g_ascii_strcasecmp (f->data, 
                                     ccm_extension_get_label (item->data)))
            {
                found = TRUE;
                break;
            }
        }

        if (found)
        {
            GType plugin = ccm_extension_get_type_object (item->data);

            plugins = g_slist_prepend (plugins, GINT_TO_POINTER (plugin));
        }
    }

    return g_slist_reverse (plugins);
}

void
ccm_extension_loader_add_plugin_path (gchar * path)
{
    g_return_if_fail (path != NULL);

    if (g_file_test (path, G_FILE_TEST_EXISTS) && 
        g_file_test (path, G_FILE_TEST_IS_DIR))
        CCMPluginPath = g_slist_prepend (CCMPluginPath, g_strdup (path));
}
