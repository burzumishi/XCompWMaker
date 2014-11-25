/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-window-xrender.h
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

#ifndef _CCM_WINDOW_XRENDER_H_
#define _CCM_WINDOW_XRENDER_H_

#include <glib-object.h>

#include "ccm-window.h"

G_BEGIN_DECLS

#define CCM_TYPE_WINDOW_X_RENDER             (ccm_window_xrender_get_type ())
#define CCM_WINDOW_X_RENDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CCM_TYPE_WINDOW_X_RENDER, CCMWindowXRender))
#define CCM_WINDOW_X_RENDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), CCM_TYPE_WINDOW_X_RENDER, CCMWindowXRenderClass))
#define CCM_IS_WINDOW_X_RENDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CCM_TYPE_WINDOW_X_RENDER))
#define CCM_IS_WINDOW_X_RENDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), CCM_TYPE_WINDOW_X_RENDER))
#define CCM_WINDOW_X_RENDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), CCM_TYPE_WINDOW_X_RENDER, CCMWindowXRenderClass))

typedef struct _CCMWindowXRenderClass CCMWindowXRenderClass;
typedef struct _CCMWindowXRender CCMWindowXRender;

struct _CCMWindowXRenderClass
{
    CCMWindowClass parent_class;
};

typedef struct _CCMWindowXRenderPrivate CCMWindowXRenderPrivate;

struct _CCMWindowXRender
{
    CCMWindow parent_instance;

    CCMWindowXRenderPrivate *priv;
};

GType ccm_window_xrender_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif                          /* _CCM_WINDOW_XRENDER_H_ */
