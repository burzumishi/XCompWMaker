/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-pixmap-buffered-image.c
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

#include <X11/Xlib.h>
#include <stdlib.h>
#include <cairo.h>

#include "ccm-pixmap-buffered-image.h"
#include "ccm-window.h"
#include "ccm-screen.h"
#include "ccm-display.h"
#include "ccm-debug.h"

G_DEFINE_TYPE (CCMPixmapBufferedImage, ccm_pixmap_buffered_image,
               CCM_TYPE_PIXMAP_IMAGE);

enum
{
    PROP_0,
    PROP_BUFFERED
};

struct _CCMPixmapBufferedImagePrivate
{
    gboolean buffered;
    cairo_surface_t *surface;
    CCMRegion *need_to_sync;
};

#define CCM_PIXMAP_BUFFERED_IMAGE_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), CCM_TYPE_PIXMAP_BUFFERED_IMAGE, CCMPixmapBufferedImagePrivate))

static gboolean ccm_pixmap_buffered_image_repair (CCMDrawable * drawable,
                                                  CCMRegion * area);
static cairo_surface_t *ccm_pixmap_buffered_image_get_surface (CCMDrawable *
                                                               drawable);

static void
ccm_pixmap_buffered_image_set_property (GObject * object, guint prop_id,
                                        const GValue * value,
                                        GParamSpec * pspec)
{
    CCMPixmapBufferedImage *self = CCM_PIXMAP_BUFFERED_IMAGE (object);

    switch (prop_id)
    {
        case PROP_BUFFERED:
            {
                self->priv->buffered = g_value_get_boolean (value);
            }
            break;
        default:
            break;
    }
}

static void
ccm_pixmap_buffered_image_init (CCMPixmapBufferedImage * self)
{
    self->priv = CCM_PIXMAP_BUFFERED_IMAGE_GET_PRIVATE (self);

    self->priv->buffered = FALSE;
    self->priv->surface = NULL;
    self->priv->need_to_sync = NULL;
}

static void
ccm_pixmap_buffered_image_finalize (GObject * object)
{
    CCMPixmapBufferedImage *self = CCM_PIXMAP_BUFFERED_IMAGE (object);

    if (self->priv->surface)
    {
        ccm_debug ("FINALIZE BUFFERED IMAGE : %i",
                   cairo_surface_get_reference_count (self->priv->surface));
        cairo_surface_destroy (self->priv->surface);
        self->priv->surface = NULL;
    }
    if (self->priv->need_to_sync)
    {
        ccm_region_destroy (self->priv->need_to_sync);
        self->priv->need_to_sync = NULL;
    }

    G_OBJECT_CLASS (ccm_pixmap_buffered_image_parent_class)->finalize (object);
}

static void
ccm_pixmap_buffered_image_class_init (CCMPixmapBufferedImageClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (CCMPixmapBufferedImagePrivate));

    object_class->set_property = ccm_pixmap_buffered_image_set_property;

    CCM_DRAWABLE_CLASS (klass)->get_surface = ccm_pixmap_buffered_image_get_surface;
    CCM_DRAWABLE_CLASS (klass)->repair = ccm_pixmap_buffered_image_repair;

    g_object_class_install_property (object_class, PROP_BUFFERED,
                                     g_param_spec_boolean ("buffered",
                                                           "Buffered",
                                                           "Set if pixmap is buffered",
                                                           FALSE,
                                                           G_PARAM_WRITABLE));

    object_class->finalize = ccm_pixmap_buffered_image_finalize;
}

static gboolean
ccm_pixmap_buffered_image_repair (CCMDrawable * drawable, CCMRegion * area)
{
    g_return_val_if_fail (drawable != NULL, FALSE);
    g_return_val_if_fail (area != NULL, FALSE);

    CCMPixmapBufferedImage *self = CCM_PIXMAP_BUFFERED_IMAGE (drawable);
    gboolean ret;

    ret = CCM_DRAWABLE_CLASS (ccm_pixmap_buffered_image_parent_class)->repair (drawable, area);

    if (self->priv->need_to_sync)
        ccm_region_union (self->priv->need_to_sync, area);
    else
        self->priv->need_to_sync = ccm_region_copy (area);

    return ret;
}

static void
ccm_pixmap_buffered_image_sync (CCMPixmapBufferedImage * self,
                                cairo_surface_t * surface)
{
    g_return_if_fail (self != NULL);
    g_return_if_fail (surface != NULL);

    gboolean sync_all = FALSE;


    if (!self->priv->surface)
    {
        CCMScreen *screen = ccm_drawable_get_screen (CCM_DRAWABLE (self));
        CCMWindow *overlay = ccm_screen_get_overlay_window (screen);

        cairo_surface_t *target = ccm_drawable_get_surface (CCM_DRAWABLE (overlay));

        if (target)
        {
            cairo_content_t content;
            gdouble width, height;

            width = cairo_image_surface_get_width (surface);
            height = cairo_image_surface_get_height (surface);

            if (ccm_drawable_get_format (CCM_DRAWABLE (self)) == CAIRO_FORMAT_ARGB32)
                content = CAIRO_CONTENT_COLOR_ALPHA;
            else
                content = CAIRO_CONTENT_COLOR;

            self->priv->surface = cairo_surface_create_similar (target, content,
                                                                width, height);
            cairo_surface_destroy (target);
            sync_all = TRUE;
        }
    }

    if (self->priv->surface && (self->priv->need_to_sync || sync_all))
    {
        cairo_t *cr;

        cr = cairo_create (self->priv->surface);

        if (!sync_all)
        {
            cairo_rectangle_t clipbox;

            ccm_region_get_clipbox (self->priv->need_to_sync, &clipbox);
            cairo_rectangle (cr, clipbox.x, clipbox.y, clipbox.width,
                             clipbox.height);
            cairo_clip (cr);
        }
        ccm_region_destroy (self->priv->need_to_sync);
        self->priv->need_to_sync = NULL;

        if (ccm_drawable_get_format (CCM_DRAWABLE (self)) == CAIRO_FORMAT_ARGB32)
        {
            cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
            cairo_paint (cr);
        }
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_surface (cr, surface, 0, 0);
        cairo_paint (cr);
        cairo_destroy (cr);
    }
}

static cairo_surface_t *
ccm_pixmap_buffered_image_get_surface (CCMDrawable * drawable)
{
    g_return_val_if_fail (drawable != NULL, NULL);

    CCMPixmapBufferedImage *self = CCM_PIXMAP_BUFFERED_IMAGE (drawable);
    cairo_surface_t *surface = NULL;

    if (self->priv->buffered && !ccm_drawable_is_damaged (drawable))
    {
        surface = CCM_DRAWABLE_CLASS (ccm_pixmap_buffered_image_parent_class)->get_surface (drawable);
        if (surface)
            ccm_pixmap_buffered_image_sync (self, surface);
        if (self->priv->surface)
        {
            cairo_surface_destroy (surface);
            surface = cairo_surface_reference (self->priv->surface);
        }
    }
    else
        surface = CCM_DRAWABLE_CLASS (ccm_pixmap_buffered_image_parent_class)->get_surface (drawable);

    return surface;
}
