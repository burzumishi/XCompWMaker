/* wmsystemtray
 * Copyright © 2009-2010  Brad Jorsch <anomie@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// http://standards.freedesktop.org/systemtray-spec/systemtray-spec-latest.html
// http://standards.freedesktop.org/xembed-spec/xembed-spec-latest.html

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "wmsystemtray.h"
#include "fdtray.h"

static Atom net_system_tray_s;
static Atom net_system_tray_opcode;
static Atom net_system_tray_message_data;
static Atom manager;
static Atom xembed;
static Atom xembed_info;
static Time seltime;
static int my_id;

static Bool get_map(Window w){
    Bool map = True;
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *data;
    int ret = XGetWindowProperty(display, w, xembed_info, 0, 2, False, xembed_info, &type, &format, &nitems, &bytes_after, &data);
    if(type == xembed_info && format == 32 && nitems >= 2){
        map = (((long *)data)[1] & 1)?True:False;
    }
    if(ret == Success) XFree(data);
    return map;
}

static void send_xembed_notify(Window w, Window parent){
    XEvent ev;
    ev.xclient.type = ClientMessage;
    ev.xclient.window = w;
    ev.xclient.message_type = xembed;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = get_X_time();
    ev.xclient.data.l[1] = 0; // XEMBED_EMBEDDED_NOTIFY
    ev.xclient.data.l[2] = 0;
    ev.xclient.data.l[3] = parent;
    ev.xclient.data.l[4] = 0; // version
    void *v=catch_BadWindow_errors();
    XSendEvent(display, w, False, NoEventMask, &ev);
    if(uncatch_BadWindow_errors(v)){
        warn(DEBUG_WARN, "Tray icon %lx is invalid", w);
        icon_remove(w);
    }
}

static void add(Window w){
    warn(DEBUG_INFO, "fdtray: Dock request for window %lx", w);
    int *data = malloc(sizeof(int));
    if(!data){
        warn(DEBUG_ERROR, "memory allocation failed");
        return;
    }
    void *v=catch_BadWindow_errors();
    XWithdrawWindow(display, w, screen);
    XSelectInput(display, w, StructureNotifyMask|PropertyChangeMask);
    Bool map = get_map(w);
    if(uncatch_BadWindow_errors(v)){
        warn(DEBUG_WARN, "fdtray: Dock request for invalid window %lx", w);
        free(data);
        return;
    }
    struct trayicon *icon = icon_add(my_id, w, data);
    if(!icon){
        free(data);
        return;
    }
    icon_set_mapping(icon, map);
}

static void event(XEvent *ev){
    struct trayicon *icon;
    void *v;

    switch(ev->type){
      case SelectionClear:
        if(ev->xselectionclear.selection == net_system_tray_s && XGetSelectionOwner(display, net_system_tray_s) != selwindow){
            warn(DEBUG_ERROR, "fdtray: Another application (window %lx) has forcibly taken the system tray registration", ev->xselectionclear.window);
            exitapp = True;
        }
        break;

      case PropertyNotify:
        if(ev->xproperty.atom != xembed_info) break;
        icon = icon_find(ev->xproperty.window);
        if(!icon || icon->type!=my_id) break;
        v = catch_BadWindow_errors();
        Bool map = get_map(icon->w);
        if(uncatch_BadWindow_errors(v)){
            warn(DEBUG_WARN, "Tray icon %lx is invalid", icon->w);
            icon_remove(icon->w);
        } else {
            icon_set_mapping(icon, map);
        }
        break;

      case ConfigureNotify:
        icon = icon_find(ev->xconfigure.window);
        if(!icon || icon->type!=my_id) break;
        v = catch_BadWindow_errors();
        {
            XWindowAttributes a;
            XGetWindowAttributes(display, icon->w, &a);
            if(a.width != iconsize || a.height != iconsize)
                XResizeWindow(display, icon->w, iconsize, iconsize);
        }
        if(uncatch_BadWindow_errors(v)){
            warn(DEBUG_WARN, "Tray icon %lx is invalid", icon->w);
            icon_remove(icon->w);
        }
        break;

      case ReparentNotify:
        icon = icon_find(ev->xreparent.window);
        if(!icon || icon->type!=my_id) break;
        if(is_icon_parent(ev->xreparent.parent)){
            send_xembed_notify(icon->w, ev->xreparent.parent);
        } else {
            warn(DEBUG_WARN, "Tray icon %lx was reparented, removing", icon->w);
            icon_remove(icon->w);
        }
        break;

      case DestroyNotify:
        icon = icon_find(ev->xdestroywindow.window);
        if(!icon || icon->type!=my_id) break;
        warn(DEBUG_WARN, "Tray icon %lx was destroyed, removing", icon->w);
        icon_remove(icon->w);
        break;

      case ClientMessage:
        if(ev->xclient.message_type == net_system_tray_opcode){
            switch(ev->xclient.data.l[1]){
              case 0: // SYSTEM_TRAY_REQUEST_DOCK
                add(ev->xclient.data.l[2]);
                break;

              case 1: // SYSTEM_TRAY_BEGIN_MESSAGE
                icon = icon_find(ev->xclient.window);
                if(!icon || icon->type!=my_id) break;
                *(int *)icon->data = ev->xclient.data.l[4];
                icon_begin_message(icon->w, ev->xclient.data.l[4], ev->xclient.data.l[3], ev->xclient.data.l[2]);
                break;

              case 2: // SYSTEM_TRAY_CANCEL_MESSAGE
                icon = icon_find(ev->xclient.window);
                if(!icon || icon->type!=my_id) break;
                icon_cancel_message(icon->w, ev->xclient.data.l[2]);
                break;
            }
        } else if(ev->xclient.message_type == net_system_tray_message_data){
            icon = icon_find(ev->xclient.window);
            if(!icon || icon->type!=my_id) break;
            icon_message_data(icon->w, *(int *)icon->data, ev->xclient.data.b, 20);
        }
        break;
    }
}

static void iremove(struct trayicon *icon){
    warn(DEBUG_INFO, "fdtray: Reparenting %lx to the root window", icon->w);
    void *v=catch_BadWindow_errors();
    XSelectInput(display, icon->w, NoEventMask);
    XUnmapWindow(display, icon->w);
    XReparentWindow(display, icon->w, root, 0,0);
    uncatch_BadWindow_errors(v);
    free(icon->data);
}

static void closing(){
    if(XGetSelectionOwner(display, net_system_tray_s) == selwindow){
        warn(DEBUG_INFO, "fdtray: Releasing system tray selection");
        // The ICCCM specifies "and the time specified as the timestamp that
        // was used to acquire the selection", so that what we do here.
        XSetSelectionOwner(display, net_system_tray_s, None, seltime);
    }
}

static struct trayfuncs funcs = {
    .handle_event = event,
    .remove_icon = iremove,
    .closing = closing,
    .deinit = NULL
};

struct trayfuncs *fdtray_init(int id, int argc, char **argv){
    char buf[50];
    XEvent ev;

    // Get the necessary atoms
    my_id = id;
    warn(DEBUG_DEBUG, "fdtray: Loading atoms");
    snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen);
    net_system_tray_s = XInternAtom(display, buf, False);
    net_system_tray_opcode = XInternAtom(display, "_NET_SYSTEM_TRAY_OPCODE", False);
    net_system_tray_message_data = XInternAtom(display, "_NET_SYSTEM_TRAY_MESSAGE_DATA", False);
    manager = XInternAtom(display, "MANAGER", False);
    xembed = XInternAtom(display, "_XEMBED", False);
    xembed_info = XInternAtom(display, "_XEMBED_INFO", False);

    // Try to grab the system tray selection. ICCCM specifies that we first
    // check for an existing owner, then grab it with a non-CurrentTime
    // timestamp, then check again if we now own it.
    if(XGetSelectionOwner(display, net_system_tray_s)){
        warn(DEBUG_WARN, "Another application is already running as the freedesktop.org protocol system tray");
        return NULL;
    }
    warn(DEBUG_INFO, "fdtray: Grabbing system tray selection");
    seltime = get_X_time();
    XSetSelectionOwner(display, net_system_tray_s, selwindow, seltime);
    if(XGetSelectionOwner(display, net_system_tray_s) != selwindow){
        warn(DEBUG_WARN, "Failed to register as the freedesktop.org protocol system tray");
        return NULL;
    }

    // Notify anyone who cares that we are now accepting tray icons
    warn(DEBUG_INFO, "fdtray: Notifying clients that we are now the system tray");
    ev.type = ClientMessage;
    ev.xclient.window = root;
    ev.xclient.message_type = manager;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = seltime;
    ev.xclient.data.l[1] = net_system_tray_s;
    ev.xclient.data.l[2] = selwindow;
    ev.xclient.data.l[3] = 0;
    ev.xclient.data.l[4] = 0;
    XSendEvent(display, root, False, StructureNotifyMask, &ev);

    return &funcs;
}
