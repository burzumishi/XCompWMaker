/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-screen-plugin.h
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

#ifndef _CCM_SCREEN_PLUGIN_H_
#define _CCM_SCREEN_PLUGIN_H_

#include <glib-object.h>

#include "ccm-plugin.h"
#include "ccm.h"

G_BEGIN_DECLS

#define CCM_TYPE_SCREEN_PLUGIN                  (ccm_screen_plugin_get_type ())
#define CCM_SCREEN_PLUGIN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), CCM_TYPE_SCREEN_PLUGIN, CCMScreenPlugin))
#define CCM_IS_SCREEN_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CCM_TYPE_SCREEN_PLUGIN))
#define CCM_SCREEN_PLUGIN_GET_INTERFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), CCM_TYPE_SCREEN_PLUGIN, CCMScreenPluginClass))
#define CCM_SCREEN_PLUGIN_PARENT(obj)           ((CCMScreenPlugin*)ccm_plugin_get_parent((CCMPlugin*)obj))
#define CCM_SCREEN_PLUGIN_ROOT(obj)             ((CCMScreenPlugin*)_ccm_screen_plugin_get_root((CCMScreenPlugin*)obj))

#define CCM_SCREEN_PLUGIN_LOCK_ROOT_METHOD(plugin, func, callback, data) \
{ \
    CCMScreenPlugin* r = (CCMScreenPlugin*)_ccm_screen_plugin_get_root((CCMScreenPlugin*)plugin); \
\
    if (r && CCM_SCREEN_PLUGIN_GET_INTERFACE(r) && \
        CCM_SCREEN_PLUGIN_GET_INTERFACE(r)->func) \
        _ccm_plugin_lock_method ((GObject*)r, CCM_SCREEN_PLUGIN_GET_INTERFACE(r)->func, \
                                 callback, data); \
}

#define CCM_SCREEN_PLUGIN_UNLOCK_ROOT_METHOD(plugin, func) \
{ \
    CCMScreenPlugin* r = (CCMScreenPlugin*)_ccm_screen_plugin_get_root((CCMScreenPlugin*)plugin); \
\
    if (r && CCM_SCREEN_PLUGIN_GET_INTERFACE(r) && \
        CCM_SCREEN_PLUGIN_GET_INTERFACE(r)->func) \
        _ccm_plugin_unlock_method ((GObject*)r, CCM_SCREEN_PLUGIN_GET_INTERFACE(r)->func); \
}

typedef struct _CCMScreenPluginClass CCMScreenPluginClass;
typedef struct _CCMScreenPluginClass CCMScreenPluginIface;
typedef struct _CCMScreenPlugin CCMScreenPlugin;

struct _CCMScreenPluginClass
{
    GTypeInterface base_iface;

    gboolean is_screen;

    void     (*load_options)   (CCMScreenPlugin * self, CCMScreen * screen);
    gboolean (*paint)          (CCMScreenPlugin * self, CCMScreen * screen,
                                cairo_t * ctx);
    gboolean (*add_window)     (CCMScreenPlugin * self, CCMScreen * screen,
                                CCMWindow * window);
    void     (*remove_window)  (CCMScreenPlugin * self, CCMScreen * screen,
                                CCMWindow * window);
    void     (*damage)         (CCMScreenPlugin * self, CCMScreen * screen,
                                CCMRegion * area, CCMWindow * window);
    void     (*on_cursor_move) (CCMScreenPlugin * self, CCMScreen * screen,
                                int x, int y);
    void     (*paint_cursor)   (CCMScreenPlugin * self, CCMScreen * screen,
                                cairo_t * ctx, int x, int y);
};

GType ccm_screen_plugin_get_type (void) G_GNUC_CONST;

CCMScreenPlugin* _ccm_screen_plugin_get_root     (CCMScreenPlugin* self);

void             ccm_screen_plugin_load_options  (CCMScreenPlugin* self,
                                                  CCMScreen* screen);
gboolean         ccm_screen_plugin_paint         (CCMScreenPlugin* self,
                                                  CCMScreen* screen,
                                                  cairo_t* ctx);
gboolean         ccm_screen_plugin_add_window    (CCMScreenPlugin* self,
                                                  CCMScreen* screen,
                                                  CCMWindow* window);
void             ccm_screen_plugin_remove_window (CCMScreenPlugin* self,
                                                  CCMScreen* screen,
                                                  CCMWindow* window);
void             ccm_screen_plugin_damage        (CCMScreenPlugin* self,
                                                  CCMScreen* screen,
                                                  CCMRegion* area,
                                                  CCMWindow* window);
void             ccm_screen_plugin_on_cursor_move (CCMScreenPlugin* self,
                                                   CCMScreen* screen,
                                                   int x, int y);
void             ccm_screen_plugin_paint_cursor   (CCMScreenPlugin* self,
                                                   CCMScreen* screen,
                                                   cairo_t* ctx, int x, int y);

G_END_DECLS

#endif                          /* _CCM_SCREEN_PLUGIN_H_ */
