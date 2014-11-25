#ifndef WMSYSTEMTRAY_H
#define WMSYSTEMTRAY_H

#include <signal.h>
#include <X11/Xlib.h>

/*
 * Structs
 */
struct trayicon {
    int type;
    Window w;
    void *data;

    /* private */
    Bool mapped;
    Bool visible;
    Window parent;
    int x, y;
    struct trayicon *next;
};

// This struct holds the necessary interface functions
struct trayfuncs {
    // Handle an X event. Or just ignore it, if it doesn't pertain to you.
    void (*handle_event)(XEvent *);

    // Called when a tray icon is being removed.
    void (*remove_icon)(struct trayicon *);

    // Called when the app is closing, before all icons are removed.
    // This should take whatever action is necessary to not accept any more
    // icons.
    void (*closing)(void);

    // Called after all icons have been removed.
    void (*deinit)(void);
};

/*
 * Global variables
 */

// Set this if you want the app to exit
extern volatile sig_atomic_t exitapp;

// Set this if you want to trigger a redraw
extern Bool need_update;

// Read-only data
extern const char *PROGNAME;
extern Bool nonwmaker;
extern Display *display;
extern int screen;
extern Window root;

// Be sure your icons remain this size or smaller.
extern int iconsize;

// This (dummy) window exists for use in holding ICCCM manager selections and
// such. Use the provided utility function instead of XSelectInput
extern Window selwindow;
void selwindow_add_mask(long mask);

/*
 * X functions
 */

// Call this before accessing any window not created by us. MUST be matched by
// a call to uncatch_BadWindow_errors.
void *catch_BadWindow_errors();

// Pass the pointer from catch_BadWindow_errors. Returns True if an error was
// ignored.
Bool uncatch_BadWindow_errors(void *v);

// Get the current X time
Time get_X_time();


/*
 * Icon handling
 */

// Call this to find out if a particular window is one of ours
Bool is_icon_parent(Window w);

// Add an icon for the specified window. Returns NULL on error.
struct trayicon *icon_add(int type, Window w, void *data);

// Remove the icon for the specified window.
void icon_remove(Window w);

// Find the icon struct for the specified window.
struct trayicon *icon_find(Window w);

// Any sane icon protocol will have the icon indicate whether it should
// be mapped or not, rather than just trying to call XMapWindow or XUnmapWindow
// directly. Use this function to indicate that state change.
Bool icon_set_mapping(struct trayicon *icon, Bool map);

// If the icon wants us to display a popup message (rather than just using
// libnotify itself), begin by passing the metadata here. "id" must be unique
// for this window. Returns True if the message may be continued.
Bool icon_begin_message(Window w, int id, int length, int timeout);

// After begin_icon_message(), call this function to pass the text of the
// message. It will automatically be displayed once the full length has been
// sent. No trailing '\0' is necessary.
Bool icon_message_data(Window w, int id, char *data, int datalen);

// If an existing popup is to be cancelled, call this.
void icon_cancel_message(Window w, int id);


/*
 * Error handling functions
 */

#include <stdarg.h>

#define DEBUG_DEBUG 4
#define DEBUG_INFO  3
#define DEBUG_WARN  2
#define DEBUG_ERROR 1

void warn(int level, char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
void die(char *fmt, ...) __attribute__ ((noreturn, format (printf, 1, 2)));

#endif
