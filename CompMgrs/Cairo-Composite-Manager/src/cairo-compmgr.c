/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cairo-compmgr.c
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

#include <config.h>
#include <signal.h>
#include <stdlib.h>

#include "ccm.h"
#include "ccm-debug.h"
#include "ccm-config.h"
#include "ccm-tray-icon.h"
#include "ccm-extension-loader.h"
#include "ccm-preferences.h"
#include "eggsmclient.h"
#include "eggdesktopfile.h"

#ifdef ENABLE_GOBJECT_INTROSPECTION
#include <girepository.h>
#endif

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

gboolean crashed = FALSE;
CCMDisplay* display = NULL;

static void
crash (int signum)
{
    if (!crashed)
    {
        crashed = TRUE;
        ccm_log_print_backtrace ();
        exit (0);
    }
}

static void
log_func (const gchar * log_domain, GLogLevelFlags log_level,
          const gchar * message, gpointer user_data)
{
    ccm_log ("");
    ccm_log (message);
    ccm_log_print_backtrace ();
}

static void
on_preferences_closed (CCMPreferences * pref, gpointer data)
{
    gtk_main_quit ();
}

static void
on_enable_ccm_changed (CCMConfig * config, gpointer data)
{
    gboolean val = ccm_config_get_boolean (config, NULL);

    ccm_debug ("CCM ENABLE %i", val);

    if (val)
    {
        if (!display)
            display = ccm_display_new (NULL);
    }
    else if (display)
    {
        ccm_debug ("UNSET CCM");
        g_object_unref (display);
        display = NULL;
    }
}

int
main (gint argc, gchar ** argv)
{
    CCMTrayIcon *trayicon;
    GError *error = NULL;
    gchar *user_plugin_path = NULL;
    EggDesktopFile *desktop;
    EggSMClient *client;

    static gboolean configure = FALSE;
    static gboolean restart = FALSE;
    static gboolean no_tray_icon = FALSE;
#ifdef ENABLE_GCONF
    static gboolean use_gconf = FALSE;
#endif
#ifdef ENABLE_GOBJECT_INTROSPECTION
    static gchar *gir_output = NULL;
#endif

    GOptionContext *option_context;
    GOptionEntry options[] = {
        {"restart", 'r', 0, G_OPTION_ARG_NONE, &restart,
            N_("Always restart cairo composite manager"),
            NULL},
        {"configure", 'c', 0, G_OPTION_ARG_NONE, &configure,
            N_("Start cairo composite manager configuration tools"),
            NULL},
        {"no-tray-icon", 'n', 0, G_OPTION_ARG_NONE, &no_tray_icon,
            N_("Disable cairo composite manager tray icon"),
            NULL},
#ifdef ENABLE_GCONF
        {"use-gconf", 'g', 0, G_OPTION_ARG_NONE, &use_gconf,
            N_("Force use gconf for configuration files"),
            NULL},
#endif
#ifdef ENABLE_GOBJECT_INTROSPECTION
        {"introspect-dump", 'i', 0, G_OPTION_ARG_STRING, &gir_output,
            N_("Dump gobject introspection file"),
            N_("types.txt,out.xml")},
#endif
        {NULL, '\0', 0, 0, NULL, NULL, NULL}
    };

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

    signal (SIGSEGV, crash);

    g_type_init ();

    egg_set_desktop_file (PACKAGE_DATA_DIR "/applications/cairo-compmgr.desktop");

    option_context = g_option_context_new (_("- Cairo composite manager"));
    g_option_context_add_group (option_context, gtk_get_option_group (TRUE));
    g_option_context_add_group (option_context, egg_sm_client_get_option_group ());
    g_option_context_add_main_entries (option_context, options, GETTEXT_PACKAGE);

    if (!g_option_context_parse (option_context, &argc, &argv, &error))
    {
        g_print ("%s\n", error->message);
        return 1;
    }

    g_log_set_default_handler (log_func, NULL);

#ifdef ENABLE_GCONF
    if (use_gconf)
        ccm_config_set_backend ("gconf");
    else
#endif
        ccm_config_set_backend ("key");

    ccm_extension_loader_add_plugin_path (PACKAGE_PLUGIN_DIR);

    user_plugin_path = g_strdup_printf ("%s/%s/plugins", g_get_user_data_dir (), PACKAGE);
    ccm_extension_loader_add_plugin_path (user_plugin_path);
    g_free (user_plugin_path);

    if (configure)
    {
        CCMPreferences *pref = ccm_preferences_new ();
        if (pref)
        {
            g_signal_connect (pref, "closed", G_CALLBACK (on_preferences_closed), NULL);

            ccm_preferences_show (pref);
            gtk_main ();
            return 0;
        }
    }

    desktop = egg_get_desktop_file ();
    egg_desktop_file_set_boolean (desktop, "X-GNOME-AutoRestart", restart);

    client = egg_sm_client_get ();
    egg_sm_client_set_mode (EGG_SM_CLIENT_MODE_NORMAL);

#ifdef ENABLE_GOBJECT_INTROSPECTION
    if (gir_output)
    {
        if (!g_irepository_dump (gir_output, &error))
        {
            g_print ("%s\n", error->message);
            return 1;
        }
        return 0;
    }
#endif

    if (!no_tray_icon)
    {
        trayicon = ccm_tray_icon_new ();
        gtk_main ();
        g_object_unref (trayicon);
    }
    else
    {
        gboolean val;
        CCMConfig* config = ccm_config_new (-1, NULL, "enable");

        g_signal_connect (config, "changed",
                          (GCallback) on_enable_ccm_changed,
                          NULL);
        val = ccm_config_get_boolean (config, NULL);

        if (val)
            display = ccm_display_new (NULL);

        gtk_main ();

        if (display) g_object_unref (display);
        g_object_unref (config);
    }
    g_option_context_free(option_context);

    return 0;
}
