/* motif.c-- stuff for support for mwm hints
 *
 *  Window Maker window manager
 *
 *  Copyright (c) 1998-2003 Alfredo K. Kojima
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "wconfig.h"

#ifdef MWM_HINTS

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "WindowMaker.h"
#include "framewin.h"
#include "window.h"
#include "properties.h"
#include "icon.h"
#include "client.h"
#include "motif.h"

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS           (1L << 0)
#define MWM_HINTS_DECORATIONS         (1L << 1)

/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL                  (1L << 0)
#define MWM_FUNC_RESIZE               (1L << 1)
#define MWM_FUNC_MOVE                 (1L << 2)
#define MWM_FUNC_MINIMIZE             (1L << 3)
#define MWM_FUNC_MAXIMIZE             (1L << 4)
#define MWM_FUNC_CLOSE                (1L << 5)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL                 (1L << 0)
#define MWM_DECOR_BORDER              (1L << 1)
#define MWM_DECOR_RESIZEH             (1L << 2)
#define MWM_DECOR_TITLE               (1L << 3)
#define MWM_DECOR_MENU                (1L << 4)
#define MWM_DECOR_MINIMIZE            (1L << 5)
#define MWM_DECOR_MAXIMIZE            (1L << 6)

/* Motif  window hints */
typedef struct {
	long flags;
	long functions;
	long decorations;
	long inputMode;
	long unknown;
} MWMHints;

static Atom _XA_MOTIF_WM_HINTS;

static void setupMWMHints(WWindow *wwin, MWMHints *mwm_hints)
{
	/*
	 * We will ignore all decoration hints that have an equivalent as
	 * functions, because wmaker does not distinguish decoration hints
	 */

	if (mwm_hints->flags & MWM_HINTS_DECORATIONS) {
		WSETUFLAG(wwin, no_titlebar, 1);
		WSETUFLAG(wwin, no_close_button, 1);
		WSETUFLAG(wwin, no_miniaturize_button, 1);
		WSETUFLAG(wwin, no_resizebar, 1);

		if (mwm_hints->decorations & MWM_DECOR_ALL) {
			WSETUFLAG(wwin, no_titlebar, 0);
			WSETUFLAG(wwin, no_close_button, 0);
			WSETUFLAG(wwin, no_closable, 0);
			WSETUFLAG(wwin, no_miniaturize_button, 0);
			WSETUFLAG(wwin, no_miniaturizable, 0);
			WSETUFLAG(wwin, no_resizebar, 0);
			WSETUFLAG(wwin, no_resizable, 0);
		}

		if (mwm_hints->decorations & MWM_DECOR_RESIZEH)
			WSETUFLAG(wwin, no_resizebar, 0);

		if (mwm_hints->decorations & MWM_DECOR_TITLE) {
			WSETUFLAG(wwin, no_titlebar, 0);
			WSETUFLAG(wwin, no_close_button, 0);
			WSETUFLAG(wwin, no_closable, 0);
		}

		if (mwm_hints->decorations & MWM_DECOR_MINIMIZE) {
			WSETUFLAG(wwin, no_miniaturize_button, 0);
			WSETUFLAG(wwin, no_miniaturizable, 0);
		}
	}

	if (mwm_hints->flags & MWM_HINTS_FUNCTIONS) {
		WSETUFLAG(wwin, no_closable, 1);
		WSETUFLAG(wwin, no_miniaturizable, 1);
		WSETUFLAG(wwin, no_resizable, 1);

		if (mwm_hints->functions & MWM_FUNC_ALL) {
			WSETUFLAG(wwin, no_closable, 0);
			WSETUFLAG(wwin, no_miniaturizable, 0);
			WSETUFLAG(wwin, no_resizable, 0);
		}
		if (mwm_hints->functions & MWM_FUNC_RESIZE)
			WSETUFLAG(wwin, no_resizable, 0);

		if (mwm_hints->functions & MWM_FUNC_MINIMIZE)
			WSETUFLAG(wwin, no_miniaturizable, 0);

		if (mwm_hints->functions & MWM_FUNC_MAXIMIZE) {
			/* a window must be resizable to be maximizable */
			WSETUFLAG(wwin, no_resizable, 0);
		}
		if (mwm_hints->functions & MWM_FUNC_CLOSE)
			WSETUFLAG(wwin, no_closable, 0);
	}
}

static int getMWMHints(Window window, MWMHints *mwmhints)
{
	unsigned long *data;
	int count;

	if (!_XA_MOTIF_WM_HINTS)
		_XA_MOTIF_WM_HINTS = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);

	data = (unsigned long *)PropGetCheckProperty(window, _XA_MOTIF_WM_HINTS,
						     _XA_MOTIF_WM_HINTS, 32, 0, &count);

	if (!data)
		return 0;

	mwmhints->flags = 0;
	if (count >= 4) {
		mwmhints->flags = data[0];
		mwmhints->functions = data[1];
		mwmhints->decorations = data[2];
		mwmhints->inputMode = data[3];
		if (count > 5)
			mwmhints->unknown = data[4];
	}
	XFree(data);

	return 1;
}

void wMWMCheckClientHints(WWindow *wwin)
{
	MWMHints hints;

	if (getMWMHints(wwin->client_win, &hints))
		setupMWMHints(wwin, &hints);
}

#endif	/* MWM_HINTS */
