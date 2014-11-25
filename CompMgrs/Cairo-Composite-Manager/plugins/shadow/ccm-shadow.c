/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-shadow.c
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

#include <math.h>
#include <string.h>

#include "ccm-property-async.h"
#include "ccm-drawable.h"
#include "ccm-display.h"
#include "ccm-screen.h"
#include "ccm-window.h"
#include "ccm-pixmap.h"
#include "ccm-shadow.h"
#include "ccm-config.h"
#include "ccm-cairo-utils.h"
#include "ccm-debug.h"
#include "ccm-preferences-page-plugin.h"
#include "ccm-config-adjustment.h"
#include "ccm-config-color-button.h"
#include "ccm.h"

enum
{
    CCM_SHADOW_SIDE_TOP,
    CCM_SHADOW_SIDE_RIGHT,
    CCM_SHADOW_SIDE_BOTTOM,
    CCM_SHADOW_SIDE_LEFT,
    CCM_SHADOW_SIDE_N
};

enum
{
    CCM_SHADOW_OFFSET,
    CCM_SHADOW_RADIUS,
    CCM_SHADOW_BORDER,
    CCM_SHADOW_COLOR,
    CCM_SHADOW_OPTION_N
};

static const gchar *CCMShadowOptionKeys[CCM_SHADOW_OPTION_N] = {
    "offset",
    "radius",
    "border",
    "color"
};

typedef struct
{
    CCMPluginOptions parent;

    int offset;
    int radius;
    int border;
    GdkColor *color;
} CCMShadowOptions;

static GQuark CCMShadowPixmapQuark;
static GQuark CCMShadowQuark;

static void ccm_shadow_on_property_changed (CCMShadow * self,
                                            CCMPropertyType changed,
                                            CCMWindow * window);
static void ccm_shadow_on_event (CCMShadow * self, XEvent * event);
static void ccm_shadow_on_option_changed (CCMPlugin * plugin, int index);

static void ccm_shadow_window_iface_init (CCMWindowPluginClass * iface);
static void ccm_shadow_screen_iface_init (CCMScreenPluginClass * iface);
static void ccm_shadow_preferences_page_iface_init (CCMPreferencesPagePluginClass * iface);

CCM_DEFINE_PLUGIN_WITH_OPTIONS (CCMShadow, ccm_shadow, CCM_TYPE_PLUGIN,
                                CCM_IMPLEMENT_INTERFACE (ccm_shadow, CCM_TYPE_SCREEN_PLUGIN,
                                                         ccm_shadow_screen_iface_init);
                                CCM_IMPLEMENT_INTERFACE (ccm_shadow, CCM_TYPE_WINDOW_PLUGIN,
                                                         ccm_shadow_window_iface_init);
                                CCM_IMPLEMENT_INTERFACE (ccm_shadow,
                                                         CCM_TYPE_PREFERENCES_PAGE_PLUGIN,
                                                         ccm_shadow_preferences_page_iface_init))
struct _CCMShadowPrivate
{
    CCMScreen *screen;

    gboolean force_disable;
    gboolean force_enable;

    guint id_check;

    CCMWindow *window;

    gboolean have_shadow;
    CCMPixmap *pixmap;
    CCMPixmap *shadow;

    CCMRegion *geometry;

    GtkBuilder *builder;

    gulong id_event;
    gulong id_property_changed;
};

#define CCM_SHADOW_GET_PRIVATE(o)  \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), CCM_TYPE_SHADOW, CCMShadowPrivate))

static void
ccm_shadow_options_init (CCMShadowOptions * self)
{
    self->offset = 0;
    self->radius = 16;
    self->border = 8;
    self->color = NULL;
}

static void
ccm_shadow_options_finalize (CCMShadowOptions* self)
{
    if (self->color) g_free (self->color);
    self->color = NULL;
}

static void
ccm_shadow_options_changed (CCMShadowOptions* self, CCMConfig* config)
{
    GError *error = NULL;

    if (config == ccm_plugin_options_get_config (CCM_PLUGIN_OPTIONS(self),
                                                 CCM_SHADOW_OFFSET))
    {
        self->offset = ccm_config_get_integer (config, &error);
        if (error)
        {
            g_warning ("Error on get shadow offset configuration value");
            g_error_free (error);
            error = NULL;
            self->offset = 0;
        }
    }

    if (config == ccm_plugin_options_get_config (CCM_PLUGIN_OPTIONS(self),
                                                 CCM_SHADOW_RADIUS))
    {
        self->radius = ccm_config_get_integer (config, &error);
        if (error)
        {
            g_warning ("Error on get shadow radius configuration value");
            g_error_free (error);
            error = NULL;
            self->radius = 16;
        }
    }

    if (config == ccm_plugin_options_get_config (CCM_PLUGIN_OPTIONS(self),
                                                 CCM_SHADOW_BORDER))
    {
        self->border = ccm_config_get_integer (config, &error);
        if (error)
        {
            g_warning ("Error on get shadow radius configuration value");
            g_error_free (error);
            error = NULL;
            self->border = 8;
        }
    }

    if (config == ccm_plugin_options_get_config (CCM_PLUGIN_OPTIONS(self),
                                                 CCM_SHADOW_COLOR))
    {
        if (self->color) g_free (self->color);
        self->color = ccm_config_get_color (config, &error);
        if (error)
        {
            g_warning ("Error on get shadow color configuration value");
            g_error_free (error);
            error = NULL;
            self->color = g_new0 (GdkColor, 1);
        }
    }
}

static void
ccm_shadow_init (CCMShadow * self)
{
    self->priv = CCM_SHADOW_GET_PRIVATE (self);

    self->priv->screen = NULL;
    self->priv->force_disable = FALSE;
    self->priv->force_enable = FALSE;
    self->priv->id_check = 0;
    self->priv->have_shadow = FALSE;
    self->priv->window = NULL;
    self->priv->shadow = NULL;
    self->priv->geometry = NULL;
    self->priv->builder = NULL;
    self->priv->id_event = 0;
    self->priv->id_property_changed = 0;
}

static void
ccm_shadow_finalize (GObject * object)
{
    CCMShadow *self = CCM_SHADOW (object);

    ccm_plugin_options_unload (CCM_PLUGIN (self));

    if (CCM_IS_SCREEN (self->priv->screen) && G_OBJECT (self->priv->screen)->ref_count)
    {
        CCMDisplay *display = ccm_screen_get_display (self->priv->screen);

        g_signal_handler_disconnect (display, self->priv->id_event);
    }
    self->priv->screen = NULL;

    if (CCM_IS_WINDOW (self->priv->window) && G_OBJECT (self->priv->window)->ref_count)
        g_signal_handler_disconnect (self->priv->window, self->priv->id_property_changed);
    self->priv->window = NULL;

    if (self->priv->id_check)
        g_source_remove (self->priv->id_check);

    if (self->priv->geometry)
    {
        ccm_region_destroy (self->priv->geometry);
        self->priv->geometry = NULL;
    }

    if (self->priv->shadow)
        g_object_unref (self->priv->shadow);
    self->priv->shadow = NULL;

    if (self->priv->pixmap)
        g_object_unref (self->priv->pixmap);
    self->priv->pixmap = NULL;

    if (self->priv->builder)
        g_object_unref (self->priv->builder);

    G_OBJECT_CLASS (ccm_shadow_parent_class)->finalize (object);
}

static void
ccm_shadow_class_init (CCMShadowClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (CCMShadowPrivate));

    CCMShadowPixmapQuark = g_quark_from_static_string("CCMShadowPixmap");
    CCMShadowQuark = g_quark_from_static_string("CCMShadow");

    klass->shadow_disable_atom = None;
    klass->shadow_enable_atom = None;

    object_class->finalize = ccm_shadow_finalize;
}

static gboolean
ccm_shadow_need_shadow (CCMShadow * self)
{
    g_return_val_if_fail (self != NULL, FALSE);

    CCMWindow *window = self->priv->window;
    CCMWindowType type = ccm_window_get_hint_type (window);
    const CCMRegion *opaque = ccm_window_get_opaque_region (window);

    return self->priv->force_enable ||
        (!self->priv->force_disable &&
         !ccm_window_is_fullscreen (window) &&
         !ccm_window_is_maximized (window) &&
         !ccm_window_is_input_only (window) &&
         ccm_window_is_viewable (window) &&
         type != CCM_WINDOW_TYPE_DESKTOP &&
         ((ccm_window_is_decorated (window) && type != CCM_WINDOW_TYPE_DOCK) ||
          (!ccm_window_is_decorated (window) && type == CCM_WINDOW_TYPE_DOCK) ||
          (type != CCM_WINDOW_TYPE_NORMAL && type != CCM_WINDOW_TYPE_DIALOG && opaque)) &&
         ((type == CCM_WINDOW_TYPE_DOCK && opaque) || type != CCM_WINDOW_TYPE_DOCK) &&
         (ccm_window_is_managed (window) ||
          type == CCM_WINDOW_TYPE_DOCK ||
          type == CCM_WINDOW_TYPE_DROPDOWN_MENU ||
          type == CCM_WINDOW_TYPE_POPUP_MENU ||
          type == CCM_WINDOW_TYPE_TOOLTIP ||
          type == CCM_WINDOW_TYPE_MENU ||
          type == CCM_WINDOW_TYPE_SPLASH));
}

static gboolean
ccm_shadow_check_needed (CCMShadow * self)
{
    g_return_val_if_fail (CCM_IS_SHADOW (self), FALSE);

    ccm_debug_window (self->priv->window, "CHECK SHADOW %i", ccm_shadow_need_shadow (self));

    if (self->priv->have_shadow != ccm_shadow_need_shadow (self))
    {
        const CCMRegion* geometry = ccm_drawable_get_device_geometry(CCM_DRAWABLE (self->priv->window));
        if (geometry && !ccm_region_empty((CCMRegion*)geometry))
        {
            if (self->priv->have_shadow)
            {
                ccm_drawable_damage (CCM_DRAWABLE (self->priv->window));
                ccm_debug_window(self->priv->window, "UNSET SHADOW %s", __FUNCTION__);
                ccm_drawable_set_geometry(CCM_DRAWABLE(self->priv->window), self->priv->geometry);

                if (self->priv->pixmap)
                    g_object_unref (self->priv->pixmap);
                self->priv->pixmap = NULL;

                if (self->priv->shadow)
                    g_object_unref (self->priv->shadow);
                self->priv->shadow = NULL;

                if (self->priv->geometry)
                    ccm_region_destroy (self->priv->geometry);
                self->priv->geometry = NULL;
                self->priv->have_shadow = FALSE;
            }
            else
            {
                gint border = ccm_shadow_get_option (self)->border;

                ccm_debug_window(self->priv->window, "SET SHADOW %s", __FUNCTION__);
                CCMRegion* new_geometry = ccm_region_copy((CCMRegion*)geometry);
                self->priv->geometry = ccm_region_copy((CCMRegion*)geometry);
                cairo_rectangle_t area;

                if (self->priv->pixmap)
                    g_object_unref (self->priv->pixmap);
                self->priv->pixmap = NULL;

                if (self->priv->shadow)
                    g_object_unref (self->priv->shadow);
                self->priv->shadow = NULL;

                g_object_set(G_OBJECT(self->priv->window), "pixmap",
                             self->priv->pixmap, NULL);

                ccm_region_get_clipbox (new_geometry, &area);
                ccm_region_offset (new_geometry, -border, -border);
                ccm_region_resize (new_geometry,
                                   area.width + border * 2,
                                   area.height + border * 2);

                ccm_drawable_set_geometry(CCM_DRAWABLE(self->priv->window),
                                          new_geometry);
                ccm_region_destroy(new_geometry);

                self->priv->have_shadow = TRUE;
            }
            ccm_drawable_damage (CCM_DRAWABLE (self->priv->window));
        }
        else
        {
            ccm_drawable_damage (CCM_DRAWABLE (self->priv->window));
            ccm_drawable_query_geometry (CCM_DRAWABLE (self->priv->window));
            ccm_drawable_damage (CCM_DRAWABLE (self->priv->window));
        }
    }

    self->priv->id_check = 0;

    return FALSE;
}

static void
ccm_shadow_on_get_shadow_property (CCMShadow * self, guint n_items,
                                   gchar * result, CCMPropertyASync * prop)
{
    g_return_if_fail (CCM_IS_PROPERTY_ASYNC (prop));

    if (!CCM_IS_SHADOW (self))
    {
        g_object_unref (prop);
        return;
    }

    Atom property = ccm_property_async_get_property (prop);

    if (result)
    {
        if (property == CCM_SHADOW_GET_CLASS (self)->shadow_enable_atom)
        {
            gulong enable;
            gboolean force_enable;
            memcpy (&enable, result, sizeof (gulong));

            ccm_debug_window (self->priv->window, "ENABLE SHADOW = %i", enable);
            force_enable = enable == 0 ? FALSE : TRUE;
            if (force_enable != self->priv->force_enable)
            {
                if (self->priv->id_check)
                    g_source_remove (self->priv->id_check);
                self->priv->force_enable = force_enable;
                ccm_shadow_check_needed (self);
            }
        }
        else if (property == CCM_SHADOW_GET_CLASS (self)->shadow_disable_atom)
        {
            gulong disable;
            gboolean force_disable;
            memcpy (&disable, result, sizeof (gulong));

            ccm_debug_window (self->priv->window, "DISABLE SHADOW = %i",
                              disable);
            force_disable = disable == 0 ? FALSE : TRUE;
            if (force_disable != self->priv->force_disable)
            {
                if (self->priv->id_check)
                    g_source_remove (self->priv->id_check);
                self->priv->force_disable = force_disable;
                ccm_shadow_check_needed (self);
            }
        }
    }

    g_object_unref (prop);
}

static void
ccm_shadow_query_force_shadow (CCMShadow * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->priv->window != NULL);

    CCMDisplay *display = ccm_drawable_get_display (CCM_DRAWABLE (self->priv->window));

    Window child = _ccm_window_get_child(self->priv->window);

    if (!child)
    {
        ccm_debug_window (self->priv->window, "QUERY SHADOW 0x%x", child);
        CCMPropertyASync *prop = ccm_property_async_new (display,
                                                         CCM_WINDOW_XWINDOW(self->priv->window),
                                                         CCM_SHADOW_GET_CLASS(self)->shadow_enable_atom,
                                                         XA_CARDINAL, 32);

        g_signal_connect (prop, "error", G_CALLBACK (g_object_unref), NULL);
        g_signal_connect_swapped (prop, "reply",
                                  G_CALLBACK
                                  (ccm_shadow_on_get_shadow_property), self);
    }
    else
    {
        ccm_debug_window (self->priv->window, "QUERY CHILD SHADOW 0x%x", child);
        CCMPropertyASync *prop = ccm_property_async_new (display, child,
                                                         CCM_SHADOW_GET_CLASS(self)->shadow_enable_atom,
                                                         XA_CARDINAL, 32);

        g_signal_connect (prop, "error", G_CALLBACK (g_object_unref), NULL);
        g_signal_connect_swapped (prop, "reply",
                                  G_CALLBACK(ccm_shadow_on_get_shadow_property), self);
    }
}

static void
ccm_shadow_query_avoid_shadow (CCMShadow * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (self->priv->window != NULL);

    CCMDisplay *display = ccm_drawable_get_display (CCM_DRAWABLE (self->priv->window));

    Window child = _ccm_window_get_child(self->priv->window);

    if (!child)
    {
        ccm_debug_window (self->priv->window, "QUERY SHADOW 0x%x", child);
        CCMPropertyASync *prop = ccm_property_async_new (display,
                                                         CCM_WINDOW_XWINDOW(self->priv->window),
                                                         CCM_SHADOW_GET_CLASS(self)->shadow_disable_atom,
                                                         XA_CARDINAL, 32);

        g_signal_connect (prop, "error", G_CALLBACK (g_object_unref), NULL);
        g_signal_connect_swapped (prop, "reply",
                                  G_CALLBACK(ccm_shadow_on_get_shadow_property), self);
    }
    else
    {
        ccm_debug_window (self->priv->window, "QUERY CHILD SHADOW 0x%x", child);
        CCMPropertyASync *prop = ccm_property_async_new (display, child,
                                                         CCM_SHADOW_GET_CLASS(self)->shadow_disable_atom,
                                                         XA_CARDINAL, 32);

        g_signal_connect (prop, "error", G_CALLBACK (g_object_unref), NULL);
        g_signal_connect_swapped (prop, "reply",
                                  G_CALLBACK(ccm_shadow_on_get_shadow_property), self);
    }
}

static void
ccm_shadow_create_atoms (CCMShadow * self)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (CCM_SHADOW_GET_CLASS (self) != NULL);

    CCMShadowClass *klass = CCM_SHADOW_GET_CLASS (self);

    if (!klass->shadow_enable_atom || !klass->shadow_disable_atom)
    {
        CCMDisplay *display = ccm_screen_get_display (self->priv->screen);

        klass->shadow_disable_atom = XInternAtom (CCM_DISPLAY_XDISPLAY (display),
                                                  "_CCM_SHADOW_DISABLED",  False);
        klass->shadow_enable_atom = XInternAtom (CCM_DISPLAY_XDISPLAY (display),
                                                 "_CCM_SHADOW_ENABLED", False);
    }
}

static cairo_surface_t*
ccm_shadow_create_shadow_image (CCMShadow * self)
{
    g_return_val_if_fail (self != NULL, NULL);

    cairo_t *cr;
    cairo_rectangle_t *rects = NULL;
    gint cpt, nb_rects;
    cairo_rectangle_t clipbox;
    gint border = ccm_shadow_get_option (self)->border;
    cairo_surface_t* surface;

    ccm_region_get_clipbox (self->priv->geometry, &clipbox);

    // Create shadow surface
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                          clipbox.width + border * 2,
                                          clipbox.height + border * 2);

    cr = cairo_create (surface);
    cairo_translate (cr, -clipbox.x, -clipbox.y);
    cairo_translate (cr, border, border);
    ccm_region_get_rectangles (self->priv->geometry, &rects, &nb_rects);
    for (cpt = 0; cpt < nb_rects; ++cpt)
        cairo_rectangle (cr, rects[cpt].x, rects[cpt].y, rects[cpt].width, rects[cpt].height);
    cairo_fill (cr);
    if (rects) cairo_rectangles_free (rects, nb_rects);
    rects = NULL;
    cairo_destroy (cr);

    // Blur surface
    clipbox.x = border * 2;
    clipbox.y = border * 2;
    clipbox.width -= border * 2;
    clipbox.height -= border * 2;
    cairo_blur_image_surface (surface, ccm_shadow_get_option (self)->radius, clipbox);

    return surface;
}

static void
ccm_shadow_on_event (CCMShadow * self, XEvent * event)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (event != NULL);

    switch (event->type)
    {
        case PropertyNotify:
            {
                XPropertyEvent *property_event = (XPropertyEvent *) event;
                CCMWindow *window;

                if (property_event->atom == CCM_SHADOW_GET_CLASS (self)->shadow_disable_atom)
                {
                    window = ccm_screen_find_window_or_child (self->priv->screen,
                                                              property_event->window);
                    if (window)
                    {
                        CCMShadow *plugin = CCM_SHADOW (_ccm_window_get_plugin (window, CCM_TYPE_SHADOW));
                        ccm_shadow_query_avoid_shadow (plugin);
                    }
                }

                if (property_event->atom == CCM_SHADOW_GET_CLASS (self)->shadow_enable_atom)
                {
                    window = ccm_screen_find_window_or_child (self->priv->screen,
                                                              property_event->window);
                    if (window)
                    {
                        CCMShadow *plugin = CCM_SHADOW (_ccm_window_get_plugin (window, CCM_TYPE_SHADOW));
                        ccm_shadow_query_force_shadow (plugin);
                    }
                }
            }
            break;
        default:
            break;
    }
}

static void
ccm_shadow_on_property_changed (CCMShadow * self, CCMPropertyType changed,
                                CCMWindow * window)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (window != NULL);

    if (!self->priv->id_check)
        ccm_shadow_check_needed(self);
}

static void
ccm_shadow_on_shadow_pixmap_destroyed (CCMShadow * self)
{
    g_return_if_fail (self != NULL);

    ccm_debug ("SHADOW PIXMAP DESTROYED");
    self->priv->shadow = NULL;
    if (self->priv->pixmap) g_object_unref (self->priv->pixmap);
    self->priv->pixmap = NULL;
}

static void
ccm_shadow_on_pixmap_destroyed (CCMShadow * self)
{
    g_return_if_fail (self != NULL);

    ccm_debug ("PIXMAP DESTROYED");
    self->priv->pixmap = NULL;
    if (self->priv->shadow) g_object_unref (self->priv->shadow);
    self->priv->shadow = NULL;
}

static void
ccm_shadow_on_pixmap_damage (CCMShadow* self, CCMRegion* area)
{
    g_return_if_fail (self != NULL);

    if (self->priv->shadow && self->priv->pixmap)
    {
        CCMDisplay* display = ccm_drawable_get_display (CCM_DRAWABLE (self->priv->pixmap));
        cairo_surface_t *surface;
        cairo_t *ctx;
        cairo_rectangle_t *rects = NULL;
        gint cpt, nb_rects;
        cairo_rectangle_t clipbox;
        gint border = ccm_shadow_get_option (self)->border;

        surface = ccm_drawable_get_surface (CCM_DRAWABLE (self->priv->pixmap));
        if (!surface)
            return;

        ctx = ccm_drawable_create_context (CCM_DRAWABLE (self->priv->shadow));
        if (!ctx)
        {
            cairo_surface_destroy (surface);
            return;
        }

        ccm_region_get_clipbox (self->priv->geometry, &clipbox);

        if (area)
        {
            cairo_translate (ctx, border, border);
            ccm_region_get_rectangles (area, &rects, &nb_rects);
            for (cpt = 0; cpt < nb_rects; ++cpt)
                cairo_rectangle (ctx, rects[cpt].x, rects[cpt].y, rects[cpt].width, rects[cpt].height);
            cairo_clip (ctx);
            if (rects) cairo_rectangles_free (rects, nb_rects);
            rects = NULL;
        }
        else
        {
            gint offset = ccm_shadow_get_option (self)->offset;
            cairo_surface_t* shadow_image = ccm_shadow_create_shadow_image (self);

            cairo_set_operator (ctx, CAIRO_OPERATOR_CLEAR);
            cairo_paint (ctx);
            cairo_set_operator (ctx, CAIRO_OPERATOR_SOURCE);

            cairo_set_source_rgb (ctx,
                                  (double) ccm_shadow_get_option (self)->color->red / 65535.0f,
                                  (double) ccm_shadow_get_option (self)->color->green / 65535.0f,
                                  (double) ccm_shadow_get_option (self)->color->blue / 65535.0f);

            cairo_mask_surface (ctx, shadow_image, 0, 0);

            cairo_translate (ctx, border, border);
            cairo_translate (ctx, -clipbox.x, -clipbox.y);

            ccm_region_get_rectangles (self->priv->geometry, &rects, &nb_rects);
            for (cpt = 0; cpt < nb_rects; ++cpt)
                cairo_rectangle (ctx, rects[cpt].x, rects[cpt].y, rects[cpt].width, rects[cpt].height);
            cairo_clip (ctx);
            if (rects) cairo_rectangles_free (rects, nb_rects);
            rects = NULL;
            cairo_translate (ctx, clipbox.x, clipbox.y);

            cairo_surface_destroy (shadow_image);
        }

        if (!ccm_pixmap_get_freeze(self->priv->shadow))
        {
            cairo_set_operator (ctx, CAIRO_OPERATOR_SOURCE);
            cairo_set_source_surface (ctx, surface, 0, 0);
            cairo_paint (ctx);
        }
        cairo_destroy (ctx);
        cairo_surface_destroy (surface);
        ccm_display_sync (display);
    }
}

static void
ccm_shadow_on_option_changed (CCMPlugin * plugin, int index)
{
    g_return_if_fail (plugin != NULL);

    CCMShadow *self = CCM_SHADOW (plugin);

    if (self->priv->window && ccm_drawable_get_geometry(CCM_DRAWABLE (self->priv->window)))
    {
        ccm_drawable_query_geometry (CCM_DRAWABLE (self->priv->window));
        ccm_drawable_damage (CCM_DRAWABLE (self->priv->window));
    }
}

static void
ccm_shadow_screen_load_options (CCMScreenPlugin * plugin, CCMScreen * screen)
{
    CCMShadow *self = CCM_SHADOW (plugin);
    CCMDisplay *display = ccm_screen_get_display (screen);

    self->priv->screen = screen;

    ccm_screen_plugin_load_options (CCM_SCREEN_PLUGIN_PARENT (plugin), screen);

    ccm_shadow_create_atoms (self);

    self->priv->id_event = g_signal_connect_swapped (display, "event",
                                                     G_CALLBACK (ccm_shadow_on_event), self);
}

static void
ccm_shadow_window_load_options (CCMWindowPlugin * plugin, CCMWindow * window)
{
    CCMShadow *self = CCM_SHADOW (plugin);

    self->priv->window = window;

    ccm_plugin_options_load (CCM_PLUGIN (self), "shadow", CCMShadowOptionKeys,
                             CCM_SHADOW_OPTION_N, ccm_shadow_on_option_changed);
    ccm_window_plugin_load_options (CCM_WINDOW_PLUGIN_PARENT (plugin), window);

    self->priv->id_property_changed = g_signal_connect_swapped (window, "property-changed",
                                                                G_CALLBACK (ccm_shadow_on_property_changed),
                                                                self);

    // We have a geometry it's a plugin reload
    if (ccm_drawable_get_device_geometry(CCM_DRAWABLE(window)) != NULL)
    {
        ccm_drawable_query_geometry (CCM_DRAWABLE (window));
    }
}

static CCMRegion *
ccm_shadow_window_query_geometry (CCMWindowPlugin * plugin, CCMWindow * window)
{
    CCMRegion *geometry = NULL;
    cairo_rectangle_t area;
    CCMShadow *self = CCM_SHADOW (plugin);

    if (self->priv->id_check)
        g_source_remove(self->priv->id_check);
    self->priv->id_check = 0;

    if (self->priv->pixmap)
        g_object_unref (self->priv->pixmap);
    self->priv->pixmap = NULL;

    if (self->priv->geometry)
        ccm_region_destroy (self->priv->geometry);
    self->priv->geometry = NULL;

    geometry = ccm_window_plugin_query_geometry (CCM_WINDOW_PLUGIN_PARENT (plugin),
                                                 window);
    if (geometry && ccm_shadow_need_shadow (self))
    {
        gint border = ccm_shadow_get_option (self)->border;

        self->priv->geometry = ccm_region_copy (geometry);
        ccm_region_get_clipbox (geometry, &area);
        ccm_region_offset (geometry, -border, -border);
        ccm_region_resize (geometry, area.width + border * 2, area.height + border * 2);

        self->priv->have_shadow = TRUE;

        ccm_shadow_query_avoid_shadow (self);
    }
    else
    {
        if (self->priv->pixmap)
            g_object_unref (self->priv->pixmap);
        self->priv->pixmap = NULL;

        self->priv->have_shadow = FALSE;
        ccm_shadow_query_force_shadow (self);
    }


    return geometry;
}

static void
ccm_shadow_window_move (CCMWindowPlugin * plugin, CCMWindow * window, int x,
                        int y)
{
    CCMShadow *self = CCM_SHADOW (plugin);

    if (self->priv->geometry)
    {
        cairo_rectangle_t area;

        ccm_region_get_clipbox (self->priv->geometry, &area);
        if (x != area.x || y != area.y)
        {
            gint border = ccm_shadow_get_option (self)->border;

            ccm_region_offset (self->priv->geometry, x - area.x, y - area.y);
            x -= border;
            y -= border;
        }
        else
            return;
    }
    ccm_window_plugin_move (CCM_WINDOW_PLUGIN_PARENT (plugin), window, x, y);
}

static void
ccm_shadow_window_resize (CCMWindowPlugin * plugin, CCMWindow * window,
                          int width, int height)
{
    CCMShadow *self = CCM_SHADOW (plugin);
    int border = 0;

    if (self->priv->geometry)
    {
        cairo_rectangle_t area;

        ccm_region_get_clipbox (self->priv->geometry, &area);
        if (width != area.width || height != area.height)
        {
            ccm_region_resize (self->priv->geometry, width, height);
            border = ccm_shadow_get_option (self)->border;

            if (self->priv->pixmap) g_object_unref (self->priv->pixmap);
            self->priv->pixmap = NULL;
        }
        else
            return;
    }

    ccm_window_plugin_resize (CCM_WINDOW_PLUGIN_PARENT (plugin), window,
                              width + border * 2, height + border * 2);
}


static void
ccm_shadow_window_set_opaque_region (CCMWindowPlugin * plugin,
                                     CCMWindow * window, const CCMRegion * area)
{
    CCMShadow *self = CCM_SHADOW (plugin);

    if (self->priv->geometry && area)
    {
        CCMRegion *opaque = ccm_region_copy (self->priv->geometry);

        ccm_region_intersect (opaque, (CCMRegion *) area);

        ccm_window_plugin_set_opaque_region (CCM_WINDOW_PLUGIN_PARENT (plugin),
                                             window, opaque);
        ccm_region_destroy (opaque);
    }
    else
        ccm_window_plugin_set_opaque_region (CCM_WINDOW_PLUGIN_PARENT (plugin),
                                             window, area);
}

static void
ccm_shadow_window_get_origin (CCMWindowPlugin * plugin, CCMWindow * window,
                              int *x, int *y)
{
    CCMShadow *self = CCM_SHADOW (plugin);
    cairo_rectangle_t geometry;

    if (self->priv->geometry)
    {
        ccm_region_get_clipbox (self->priv->geometry, &geometry);
        *x = geometry.x;
        *y = geometry.y;
    }
    else
    {
        ccm_window_plugin_get_origin (CCM_WINDOW_PLUGIN_PARENT (plugin), window,
                                      x, y);
    }
}

static CCMPixmap *
ccm_shadow_window_get_pixmap (CCMWindowPlugin * plugin, CCMWindow * window)
{
    CCMShadow *self = CCM_SHADOW (plugin);
    CCMPixmap *pixmap = NULL;

    pixmap = ccm_window_plugin_get_pixmap (CCM_WINDOW_PLUGIN_PARENT (plugin),
                                           window);

    if (pixmap && self->priv->have_shadow && self->priv->geometry)
    {
        gint swidth, sheight;
        cairo_rectangle_t clipbox;
        gint border = ccm_shadow_get_option (self)->border;

        ccm_region_get_clipbox (self->priv->geometry, &clipbox);
        swidth = clipbox.width + border * 2;
        sheight = clipbox.height + border * 2;

        if (self->priv->pixmap) g_object_unref(self->priv->pixmap);
        self->priv->pixmap = pixmap;

        ccm_debug_window (window, "CREATE SHADOW PIXMAP");
        self->priv->shadow = ccm_window_create_pixmap (window, swidth, sheight, 32);

        g_object_set_qdata_full (G_OBJECT (self->priv->shadow),
                                 CCMShadowPixmapQuark, self,
                                 (GDestroyNotify)ccm_shadow_on_shadow_pixmap_destroyed);

        g_object_set_qdata_full (G_OBJECT (pixmap), CCMShadowQuark, self,
                                 (GDestroyNotify)ccm_shadow_on_pixmap_destroyed);

        g_signal_connect_swapped (pixmap, "damaged",
                                  G_CALLBACK (ccm_shadow_on_pixmap_damage),
                                  self);

        ccm_shadow_on_pixmap_damage (self, NULL);

        pixmap = self->priv->shadow;
    }

    return pixmap;
}

static void
ccm_shadow_preferences_page_init_windows_section (CCMPreferencesPagePlugin *
                                                  plugin,
                                                  CCMPreferencesPage *
                                                  preferences,
                                                  GtkWidget * windows_section)
{
    CCMShadow *self = CCM_SHADOW (plugin);

    self->priv->builder = gtk_builder_new ();

    if (gtk_builder_add_from_file (self->priv->builder, UI_DIR "/ccm-shadow.ui", NULL))
    {
        GtkWidget *widget = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "shadow"));
        if (widget)
        {
            gint screen_num = ccm_preferences_page_get_screen_num (preferences);

            gtk_box_pack_start (GTK_BOX (windows_section), widget, FALSE, TRUE,
                                0);

            CCMConfigColorButton *color =
                CCM_CONFIG_COLOR_BUTTON (gtk_builder_get_object (self->priv->builder,
                                                                 "color"));
            g_object_set (color, "screen", screen_num, NULL);

            CCMConfigAdjustment *radius =
                CCM_CONFIG_ADJUSTMENT (gtk_builder_get_object (self->priv->builder,
                                                               "radius-adjustment"));
            g_object_set (radius, "screen", screen_num, NULL);

            CCMConfigAdjustment *border =
                CCM_CONFIG_ADJUSTMENT (gtk_builder_get_object (self->priv->builder,
                                                               "border-adjustment"));
            g_object_set (border, "screen", screen_num, NULL);

            ccm_preferences_page_section_register_widget (preferences,
                                                          CCM_PREFERENCES_PAGE_SECTION_WINDOW,
                                                          widget, "shadow");
        }
    }
    ccm_preferences_page_plugin_init_windows_section (CCM_PREFERENCES_PAGE_PLUGIN_PARENT (plugin),
                                                      preferences,
                                                      windows_section);
}

static void
ccm_shadow_window_iface_init (CCMWindowPluginClass * iface)
{
    iface->load_options = ccm_shadow_window_load_options;
    iface->query_geometry = ccm_shadow_window_query_geometry;
    iface->paint = NULL;
    iface->map = NULL;
    iface->unmap = NULL;
    iface->query_opacity = NULL;
    iface->move = ccm_shadow_window_move;
    iface->resize = ccm_shadow_window_resize;
    iface->set_opaque_region = ccm_shadow_window_set_opaque_region;
    iface->get_origin = ccm_shadow_window_get_origin;
    iface->get_pixmap = ccm_shadow_window_get_pixmap;
}

static void
ccm_shadow_screen_iface_init (CCMScreenPluginClass * iface)
{
    iface->load_options = ccm_shadow_screen_load_options;
    iface->paint = NULL;
    iface->add_window = NULL;
    iface->remove_window = NULL;
    iface->damage = NULL;
}

static void
ccm_shadow_preferences_page_iface_init (CCMPreferencesPagePluginClass * iface)
{
    iface->init_general_section = NULL;
    iface->init_desktop_section = NULL;
    iface->init_windows_section = ccm_shadow_preferences_page_init_windows_section;
    iface->init_effects_section = NULL;
    iface->init_accessibility_section = NULL;
    iface->init_utilities_section = NULL;
}
