/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-pixmap.h
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

#ifndef _CCM_PIXMAP_H_
#define _CCM_PIXMAP_H_

#include <glib-object.h>

#include "ccm-drawable.h"

G_BEGIN_DECLS

#define CCM_TYPE_PIXMAP             (ccm_pixmap_get_type ())
#define CCM_PIXMAP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CCM_TYPE_PIXMAP, CCMPixmap))
#define CCM_PIXMAP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), CCM_TYPE_PIXMAP, CCMPixmapClass))
#define CCM_IS_PIXMAP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CCM_TYPE_PIXMAP))
#define CCM_IS_PIXMAP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), CCM_TYPE_PIXMAP))
#define CCM_PIXMAP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), CCM_TYPE_PIXMAP, CCMPixmapClass))
#define CCM_PIXMAP_XPIXMAP(obj)     (ccm_drawable_get_xid(CCM_DRAWABLE(obj)))

struct _CCMPixmapClass
{
    CCMDrawableClass parent_class;

    void (*bind) (CCMPixmap * self);
    void (*release) (CCMPixmap * self);
};

typedef struct _CCMPixmapPrivate CCMPixmapPrivate;

struct _CCMPixmap
{
    CCMDrawable parent_instance;

    CCMPixmapPrivate *priv;
};

GType      ccm_pixmap_get_type        (void) G_GNUC_CONST;

CCMPixmap*             ccm_pixmap_new             (CCMDrawable* drawable, Pixmap xpixmap);
CCMPixmap*             ccm_pixmap_image_new       (CCMDrawable* drawable, Pixmap xpixmap);
CCMPixmap*             ccm_pixmap_new_from_visual (CCMScreen* screen, Visual* visual,
                                                   Pixmap xpixmap);
G_GNUC_PURE gboolean   ccm_pixmap_get_foreign     (CCMPixmap* self);
void                   ccm_pixmap_set_foreign     (CCMPixmap* self, gboolean foreign);
G_GNUC_PURE gboolean   ccm_pixmap_get_freeze      (CCMPixmap * self);
void                   ccm_pixmap_set_freeze      (CCMPixmap * self, gboolean freeze);

G_END_DECLS

#endif                          /* _CCM_PIXMAP_H_ */
