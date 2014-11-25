/*
 *  Window Maker window manager
 *
 *  Copyright (c) 1997-2003 Alfredo K. Kojima
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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdarg.h>
#include <pwd.h>
#include <math.h>
#include <time.h>

#include <X11/XKBlib.h>

#include <WINGs/WUtil.h>
#include <wraster.h>

#include "window.h"
#include "misc.h"
#include "WindowMaker.h"
#include "GNUstep.h"
#include "screen.h"
#include "wcore.h"
#include "window.h"
#include "framewin.h"
#include "dialog.h"
#include "xutil.h"
#include "xmodifier.h"


#define ICON_SIZE wPreferences.icon_size

/**** Local prototypes *****/
static void UnescapeWM_CLASS(const char *str, char **name, char **class);

/* XFetchName Wrapper */
Bool wFetchName(Display *dpy, Window win, char **winname)
{
	XTextProperty text_prop;
	char **list;
	int num;

	if (XGetWMName(dpy, win, &text_prop)) {
		if (text_prop.value && text_prop.nitems > 0) {
			if (text_prop.encoding == XA_STRING) {
				*winname = wstrdup((char *)text_prop.value);
				XFree(text_prop.value);
			} else {
				text_prop.nitems = strlen((char *)text_prop.value);
				if (XmbTextPropertyToTextList(dpy, &text_prop, &list, &num) >=
				    Success && num > 0 && *list) {
					XFree(text_prop.value);
					*winname = wstrdup(*list);
					XFreeStringList(list);
				} else {
					*winname = wstrdup((char *)text_prop.value);
					XFree(text_prop.value);
				}
			}
		} else {
			/* the title is set, but it was set to none */
			*winname = wstrdup("");
		}
		return True;
	} else {
		/* the hint is probably not set */
		*winname = NULL;

		return False;
	}
}

/* XGetIconName Wrapper */
Bool wGetIconName(Display *dpy, Window win, char **iconname)
{
	XTextProperty text_prop;
	char **list;
	int num;

	if (XGetWMIconName(dpy, win, &text_prop) != 0 && text_prop.value && text_prop.nitems > 0) {
		if (text_prop.encoding == XA_STRING)
			*iconname = (char *)text_prop.value;
		else {
			text_prop.nitems = strlen((char *)text_prop.value);
			if (XmbTextPropertyToTextList(dpy, &text_prop, &list, &num) >= Success && num > 0 && *list) {
				XFree(text_prop.value);
				*iconname = wstrdup(*list);
				XFreeStringList(list);
			} else
				*iconname = (char *)text_prop.value;
		}
		return True;
	}
	*iconname = NULL;
	return False;
}

static void eatExpose(void)
{
	XEvent event, foo;

	/* compress all expose events into a single one */

	if (XCheckMaskEvent(dpy, ExposureMask, &event)) {
		/* ignore other exposure events for this window */
		while (XCheckWindowEvent(dpy, event.xexpose.window, ExposureMask, &foo)) ;
		/* eat exposes for other windows */
		eatExpose();

		event.xexpose.count = 0;
		XPutBackEvent(dpy, &event);
	}
}

void move_window(Window win, int from_x, int from_y, int to_x, int to_y)
{
#ifdef ANIMATIONS
	if (wPreferences.no_animations)
		XMoveWindow(dpy, win, to_x, to_y);
	else
		SlideWindow(win, from_x, from_y, to_x, to_y);
#else
	XMoveWindow(dpy, win, to_x, to_y);
#endif
}

void SlideWindow(Window win, int from_x, int from_y, int to_x, int to_y)
{
	Window *wins[1] = { &win };
	SlideWindows(wins, 1, from_x, from_y, to_x, to_y);
}

/* wins is an array of Window, sorted from left to right, the first is
 * going to be moved from (from_x,from_y) to (to_x,to_y) and the
 * following windows are going to be offset by (ICON_SIZE*i,0) */
void SlideWindows(Window *wins[], int n, int from_x, int from_y, int to_x, int to_y)
{
	time_t time0 = time(NULL);
	float dx, dy, x = from_x, y = from_y, px, py;
	Bool is_dx_nul, is_dy_nul;
	int dx_is_bigger = 0, dx_int, dy_int;
	int slide_delay, slide_steps, slide_slowdown;
	int i;

	/* animation parameters */
	static const struct {
		int delay;
		int steps;
		int slowdown;
	} apars[5] = {
		{ICON_SLIDE_DELAY_UF, ICON_SLIDE_STEPS_UF, ICON_SLIDE_SLOWDOWN_UF},
		{ICON_SLIDE_DELAY_F,  ICON_SLIDE_STEPS_F,  ICON_SLIDE_SLOWDOWN_F},
		{ICON_SLIDE_DELAY_M,  ICON_SLIDE_STEPS_M,  ICON_SLIDE_SLOWDOWN_M},
		{ICON_SLIDE_DELAY_S,  ICON_SLIDE_STEPS_S,  ICON_SLIDE_SLOWDOWN_S},
		{ICON_SLIDE_DELAY_US, ICON_SLIDE_STEPS_US, ICON_SLIDE_SLOWDOWN_US}
	};

	slide_slowdown = apars[(int)wPreferences.icon_slide_speed].slowdown;
	slide_steps = apars[(int)wPreferences.icon_slide_speed].steps;
	slide_delay = apars[(int)wPreferences.icon_slide_speed].delay;

	dx_int = to_x - from_x;
	dy_int = to_y - from_y;
	is_dx_nul = (dx_int == 0);
	is_dy_nul = (dy_int == 0);
	dx = (float) dx_int;
	dy = (float) dy_int;

	if (abs(dx_int) > abs(dy_int)) {
		dx_is_bigger = 1;
	}

	if (dx_is_bigger) {
		px = dx / slide_slowdown;
		if (px < slide_steps && px > 0)
			px = slide_steps;
		else if (px > -slide_steps && px < 0)
			px = -slide_steps;
		py = (is_dx_nul ? 0.0 : px * dy / dx);
	} else {
		py = dy / slide_slowdown;
		if (py < slide_steps && py > 0)
			py = slide_steps;
		else if (py > -slide_steps && py < 0)
			py = -slide_steps;
		px = (is_dy_nul ? 0.0 : py * dx / dy);
	}

	while (((int)x) != to_x ||
			 ((int)y) != to_y) {
		x += px;
		y += py;
		if ((px < 0 && (int)x < to_x) || (px > 0 && (int)x > to_x))
			x = (float)to_x;
		if ((py < 0 && (int)y < to_y) || (py > 0 && (int)y > to_y))
			y = (float)to_y;

		if (dx_is_bigger) {
			px = px * (1.0 - 1 / (float)slide_slowdown);
			if (px < slide_steps && px > 0)
				px = slide_steps;
			else if (px > -slide_steps && px < 0)
				px = -slide_steps;
			py = (is_dx_nul ? 0.0 : px * dy / dx);
		} else {
			py = py * (1.0 - 1 / (float)slide_slowdown);
			if (py < slide_steps && py > 0)
				py = slide_steps;
			else if (py > -slide_steps && py < 0)
				py = -slide_steps;
			px = (is_dy_nul ? 0.0 : py * dx / dy);
		}

		for (i = 0; i < n; i++) {
			XMoveWindow(dpy, *wins[i], (int)x + i * ICON_SIZE, (int)y);
		}
		XFlush(dpy);
		if (slide_delay > 0) {
			wusleep(slide_delay * 1000L);
		} else {
			wusleep(10);
		}
		if (time(NULL) - time0 > MAX_ANIMATION_TIME)
			break;
	}
	for (i = 0; i < n; i++) {
		XMoveWindow(dpy, *wins[i], to_x + i * ICON_SIZE, to_y);
	}

	XSync(dpy, 0);
	/* compress expose events */
	eatExpose();
}

char *ShrinkString(WMFont *font, const char *string, int width)
{
	int w, w1 = 0;
	int p;
	char *pos;
	char *text;
	int p1, p2, t;

	p = strlen(string);
	w = WMWidthOfString(font, string, p);
	text = wmalloc(strlen(string) + 8);
	strcpy(text, string);
	if (w <= width)
		return text;

	pos = strchr(text, ' ');
	if (!pos)
		pos = strchr(text, ':');

	if (pos) {
		*pos = 0;
		p = strlen(text);
		w1 = WMWidthOfString(font, text, p);
		if (w1 > width) {
			p = 0;
			*pos = ' ';
			*text = 0;
		} else {
			*pos = 0;
			width -= w1;
			p++;
		}
		string += p;
		p = strlen(string);
	} else {
		*text = 0;
	}
	strcat(text, "...");
	width -= WMWidthOfString(font, "...", 3);

	p1 = 0;
	p2 = p;
	t = (p2 - p1) / 2;
	while (p2 > p1 && p1 != t) {
		w = WMWidthOfString(font, &string[p - t], t);
		if (w > width) {
			p2 = t;
			t = p1 + (p2 - p1) / 2;
		} else if (w < width) {
			p1 = t;
			t = p1 + (p2 - p1) / 2;
		} else
			p2 = p1 = t;
	}
	strcat(text, &string[p - p1]);

	return text;
}

char *FindImage(const char *paths, const char *file)
{
	char *tmp, *path = NULL;

	tmp = strrchr(file, ':');
	if (tmp) {
		*tmp = 0;
		path = wfindfile(paths, file);
		*tmp = ':';
	}
	if (!tmp || !path)
		path = wfindfile(paths, file);

	return path;
}

static void timeoutHandler(void *data)
{
	*(int *)data = 1;
}

static char *getTextSelection(WScreen * screen, Atom selection)
{
	int buffer = -1;

	switch (selection) {
	case XA_CUT_BUFFER0:
		buffer = 0;
		break;
	case XA_CUT_BUFFER1:
		buffer = 1;
		break;
	case XA_CUT_BUFFER2:
		buffer = 2;
		break;
	case XA_CUT_BUFFER3:
		buffer = 3;
		break;
	case XA_CUT_BUFFER4:
		buffer = 4;
		break;
	case XA_CUT_BUFFER5:
		buffer = 5;
		break;
	case XA_CUT_BUFFER6:
		buffer = 6;
		break;
	case XA_CUT_BUFFER7:
		buffer = 7;
		break;
	}
	if (buffer >= 0) {
		char *data;
		int size;

		data = XFetchBuffer(dpy, &size, buffer);

		return data;
	} else {
		char *data;
		int bits;
		Atom rtype;
		unsigned long len, bytes;
		WMHandlerID timer;
		int timeout = 0;
		XEvent ev;
		static Atom clipboard = 0;

		if (!clipboard)
			clipboard = XInternAtom(dpy, "CLIPBOARD", False);

		XDeleteProperty(dpy, screen->info_window, clipboard);

		XConvertSelection(dpy, selection, XA_STRING, clipboard, screen->info_window, CurrentTime);

		timer = WMAddTimerHandler(1000, timeoutHandler, &timeout);

		while (!XCheckTypedWindowEvent(dpy, screen->info_window, SelectionNotify, &ev) && !timeout) ;

		if (!timeout) {
			WMDeleteTimerHandler(timer);
		} else {
			wwarning("selection retrieval timed out");
			return NULL;
		}

		/* nobody owns the selection or the current owner has
		 * nothing to do with what we need */
		if (ev.xselection.property == None) {
			return NULL;
		}

		if (XGetWindowProperty(dpy, screen->info_window,
				       clipboard, 0, 1024,
				       False, XA_STRING, &rtype, &bits, &len,
				       &bytes, (unsigned char **)&data) != Success) {
			return NULL;
		}
		if (rtype != XA_STRING || bits != 8) {
			wwarning("invalid data in text selection");
			if (data)
				XFree(data);
			return NULL;
		}
		return data;
	}
}

static char *getselection(WScreen * scr)
{
	char *tmp;

	tmp = getTextSelection(scr, XA_PRIMARY);
	if (!tmp)
		tmp = getTextSelection(scr, XA_CUT_BUFFER0);
	return tmp;
}

static char*
parseuserinputpart(const char *line, int *ptr, const char *endchars)
{
	int depth = 0, begin;
	char *value = NULL;
	begin = ++*ptr;

	while(line[*ptr] != '\0') {
		if(line[*ptr] == '(') {
			++depth;
		} else if(depth > 0 && line[*ptr] == ')') {
			--depth;
		} else if(depth == 0 && strchr(endchars, line[*ptr]) != NULL) {
			value = wmalloc(*ptr - begin + 1);
			strncpy(value, line + begin, *ptr - begin);
			value[*ptr - begin] = '\0';
			break;
		}
		++*ptr;
	}

	return value;
}

static char*
getuserinput(WScreen *scr, const char *line, int *ptr, Bool advanced)
{
    char *ret = NULL, *title = NULL, *prompt = NULL, *name = NULL;
    int rv;

    if(line[*ptr] == '(')
	title = parseuserinputpart(line, ptr, ",)");
    if(title != NULL && line[*ptr] == ',')
	prompt = parseuserinputpart(line, ptr, ",)");
    if(prompt != NULL && line[*ptr] == ',')
	name = parseuserinputpart(line, ptr, ")");

    if(advanced)
        rv = wAdvancedInputDialog(scr,
		title ? _(title):_("Program Arguments"),
		prompt ? _(prompt):_("Enter command arguments:"),
		name, &ret);
    else
        rv = wInputDialog(scr,
		title ? _(title):_("Program Arguments"),
		prompt ? _(prompt):_("Enter command arguments:"),
		&ret);

    if(title) wfree(title);
    if(prompt) wfree(prompt);
    if(name) wfree(name);

    return rv ? ret : NULL;
}

#define S_NORMAL 0
#define S_ESCAPE 1
#define S_OPTION 2

/*
 * state    	input   new-state	output
 * NORMAL	%	OPTION		<nil>
 * NORMAL	\	ESCAPE		<nil>
 * NORMAL	etc.	NORMAL		<input>
 * ESCAPE	any	NORMAL		<input>
 * OPTION	s	NORMAL		<selection buffer>
 * OPTION	w	NORMAL		<selected window id>
 * OPTION	a	NORMAL		<input text>
 * OPTION	d	NORMAL		<OffiX DND selection object>
 * OPTION	W	NORMAL		<current workspace>
 * OPTION	etc.	NORMAL		%<input>
 */
#define TMPBUFSIZE 64
char *ExpandOptions(WScreen *scr, const char *cmdline)
{
	int ptr, optr, state, len, olen;
	char *out, *nout;
	char *selection = NULL;
	char *user_input = NULL;
#ifdef XDND
	char *dropped_thing = NULL;
#endif
	char tmpbuf[TMPBUFSIZE];
	int slen;

	len = strlen(cmdline);
	olen = len + 1;
	out = malloc(olen);
	if (!out) {
		wwarning(_("out of memory during expansion of \"%s\""), cmdline);
		return NULL;
	}
	*out = 0;
	ptr = 0;		/* input line pointer */
	optr = 0;		/* output line pointer */
	state = S_NORMAL;
	while (ptr < len) {
		switch (state) {
		case S_NORMAL:
			switch (cmdline[ptr]) {
			case '\\':
				state = S_ESCAPE;
				break;
			case '%':
				state = S_OPTION;
				break;
			default:
				state = S_NORMAL;
				out[optr++] = cmdline[ptr];
				break;
			}
			break;
		case S_ESCAPE:
			switch (cmdline[ptr]) {
			case 'n':
				out[optr++] = 10;
				break;

			case 'r':
				out[optr++] = 13;
				break;

			case 't':
				out[optr++] = 9;
				break;

			default:
				out[optr++] = cmdline[ptr];
			}
			state = S_NORMAL;
			break;
		case S_OPTION:
			state = S_NORMAL;
			switch (cmdline[ptr]) {
			case 'w':
				if (scr->focused_window && scr->focused_window->flags.focused) {
					snprintf(tmpbuf, sizeof(tmpbuf), "0x%x",
						 (unsigned int)scr->focused_window->client_win);
					slen = strlen(tmpbuf);
					olen += slen;
					nout = realloc(out, olen);
					if (!nout) {
						wwarning(_("out of memory during expansion of \"%%w\""));
						goto error;
					}
					out = nout;
					strcat(out, tmpbuf);
					optr += slen;
				} else {
					out[optr++] = ' ';
				}
				break;

			case 'W':
				snprintf(tmpbuf, sizeof(tmpbuf), "0x%x", (unsigned int)w_global.workspace.current + 1);
				slen = strlen(tmpbuf);
				olen += slen;
				nout = realloc(out, olen);
				if (!nout) {
					wwarning(_("out of memory during expansion of \"%%W\""));
					goto error;
				}
				out = nout;
				strcat(out, tmpbuf);
				optr += slen;
				break;

			case 'a':
			case 'A':
				ptr++;
				user_input = getuserinput(scr, cmdline, &ptr, cmdline[ptr-1] == 'A');
				if (user_input) {
					slen = strlen(user_input);
					olen += slen;
					nout = realloc(out, olen);
					if (!nout) {
						wwarning(_("out of memory during expansion of \"%%a\""));
						goto error;
					}
					out = nout;
					strcat(out, user_input);
					optr += slen;
				} else {
					/* Not an error, but user has Canceled the dialog box.
					 * This will make the command to not be performed. */
					goto error;
				}
				break;

#ifdef XDND
			case 'd':
				if (scr->xdestring) {
					dropped_thing = wstrdup(scr->xdestring);
				}
				if (!dropped_thing) {
					scr->flags.dnd_data_convertion_status = 1;
					goto error;
				}
				slen = strlen(dropped_thing);
				olen += slen;
				nout = realloc(out, olen);
				if (!nout) {
					wwarning(_("out of memory during expansion of \"%%d\""));
					goto error;
				}
				out = nout;
				strcat(out, dropped_thing);
				optr += slen;
				break;
#endif				/* XDND */

			case 's':
				if (!selection) {
					selection = getselection(scr);
				}
				if (!selection) {
					wwarning(_("selection not available"));
					goto error;
				}
				slen = strlen(selection);
				olen += slen;
				nout = realloc(out, olen);
				if (!nout) {
					wwarning(_("out of memory during expansion of \"%%s\""));
					goto error;
				}
				out = nout;
				strcat(out, selection);
				optr += slen;
				break;

			default:
				out[optr++] = '%';
				out[optr++] = cmdline[ptr];
			}
			break;
		}
		out[optr] = 0;
		ptr++;
	}
	if (selection)
		XFree(selection);
	return out;

 error:
	wfree(out);
	if (selection)
		XFree(selection);
	return NULL;
}

void ParseWindowName(WMPropList *value, char **winstance, char **wclass, const char *where)
{
	char *name;

	*winstance = *wclass = NULL;

	if (!WMIsPLString(value)) {
		wwarning(_("bad window name value in %s state info"), where);
		return;
	}

	name = WMGetFromPLString(value);
	if (!name || strlen(name) == 0) {
		wwarning(_("bad window name value in %s state info"), where);
		return;
	}

	UnescapeWM_CLASS(name, winstance, wclass);
}

#if 0
static char *keysymToString(KeySym keysym, unsigned int state)
{
	XKeyEvent kev;
	char *buf = wmalloc(20);
	int count;

	kev.display = dpy;
	kev.type = KeyPress;
	kev.send_event = False;
	kev.window = DefaultRootWindow(dpy);
	kev.root = DefaultRootWindow(dpy);
	kev.same_screen = True;
	kev.subwindow = kev.root;
	kev.serial = 0x12344321;
	kev.time = CurrentTime;
	kev.state = state;
	kev.keycode = XKeysymToKeycode(dpy, keysym);
	count = XLookupString(&kev, buf, 19, NULL, NULL);
	buf[count] = 0;

	return buf;
}
#endif

char *GetShortcutString(const char *shortcut)
{
	char *buffer = NULL;
	char *k;
	int control = 0;
	char *tmp, *text;

	tmp = text = wstrdup(shortcut);

	/* get modifiers */
	while ((k = strchr(text, '+')) != NULL) {
		int mod;

		*k = 0;
		mod = wXModifierFromKey(text);
		if (mod < 0) {
			return wstrdup("bug");
		}

		if (strcasecmp(text, "Meta") == 0) {
			buffer = wstrappend(buffer, "M+");
		} else if (strcasecmp(text, "Alt") == 0) {
			buffer = wstrappend(buffer, "A+");
		} else if (strcasecmp(text, "Shift") == 0) {
			buffer = wstrappend(buffer, "Sh+");
		} else if (strcasecmp(text, "Mod1") == 0) {
			buffer = wstrappend(buffer, "M1+");
		} else if (strcasecmp(text, "Mod2") == 0) {
			buffer = wstrappend(buffer, "M2+");
		} else if (strcasecmp(text, "Mod3") == 0) {
			buffer = wstrappend(buffer, "M3+");
		} else if (strcasecmp(text, "Mod4") == 0) {
			buffer = wstrappend(buffer, "M4+");
		} else if (strcasecmp(text, "Mod5") == 0) {
			buffer = wstrappend(buffer, "M5+");
		} else if (strcasecmp(text, "Control") == 0) {
			control = 1;
		} else {
			buffer = wstrappend(buffer, text);
		}
		text = k + 1;
	}

	if (control) {
		buffer = wstrappend(buffer, "^");
	}
	buffer = wstrappend(buffer, text);
	wfree(tmp);

	return buffer;
}

char *GetShortcutKey(WShortKey key)
{
	char *tmp = NULL;
	char *k = XKeysymToString(XkbKeycodeToKeysym(dpy, key.keycode, 0, 0));
	if (!k) return NULL;

	char **m = wPreferences.modifier_labels;
	if (key.modifier & ControlMask) tmp = wstrappend(tmp, m[1] ? m[1] : "Control+");
	if (key.modifier & ShiftMask)   tmp = wstrappend(tmp, m[0] ? m[0] : "Shift+");
	if (key.modifier & Mod1Mask)    tmp = wstrappend(tmp, m[2] ? m[2] : "Mod1+");
	if (key.modifier & Mod2Mask)    tmp = wstrappend(tmp, m[3] ? m[3] : "Mod2+");
	if (key.modifier & Mod3Mask)    tmp = wstrappend(tmp, m[4] ? m[4] : "Mod3+");
	if (key.modifier & Mod4Mask)    tmp = wstrappend(tmp, m[5] ? m[5] : "Mod4+");
	if (key.modifier & Mod5Mask)    tmp = wstrappend(tmp, m[6] ? m[6] : "Mod5+");
	tmp = wstrappend(tmp, k);

	return GetShortcutString(tmp);
}

char *EscapeWM_CLASS(const char *name, const char *class)
{
	char *ret;
	char *ename = NULL, *eclass = NULL;
	int i, j, l;

	if (!name && !class)
		return NULL;

	if (name) {
		l = strlen(name);
		ename = wmalloc(l * 2 + 1);
		j = 0;
		for (i = 0; i < l; i++) {
			if (name[i] == '\\') {
				ename[j++] = '\\';
			} else if (name[i] == '.') {
				ename[j++] = '\\';
			}
			ename[j++] = name[i];
		}
		ename[j] = 0;
	}
	if (class) {
		l = strlen(class);
		eclass = wmalloc(l * 2 + 1);
		j = 0;
		for (i = 0; i < l; i++) {
			if (class[i] == '\\') {
				eclass[j++] = '\\';
			} else if (class[i] == '.') {
				eclass[j++] = '\\';
			}
			eclass[j++] = class[i];
		}
		eclass[j] = 0;
	}

	if (ename && eclass) {
		int len = strlen(ename) + strlen(eclass) + 4;
		ret = wmalloc(len);
		snprintf(ret, len, "%s.%s", ename, eclass);
		wfree(ename);
		wfree(eclass);
	} else if (ename) {
		ret = wstrdup(ename);
		wfree(ename);
	} else {
		ret = wstrdup(eclass);
		wfree(eclass);
	}

	return ret;
}

static void UnescapeWM_CLASS(const char *str, char **name, char **class)
{
	int i, j, k, dot;

	j = strlen(str);
	*name = wmalloc(j);
	**name = 0;
	*class = wmalloc(j);
	**class = 0;

	/* separate string in 2 parts */
	dot = -1;
	for (i = 0; i < j; i++) {
		if (str[i] == '\\') {
			i++;
			continue;
		} else if (str[i] == '.') {
			dot = i;
			break;
		}
	}

	/* unescape strings */
	for (i = 0, k = 0; i < dot; i++) {
		if (str[i] == '\\') {
			continue;
		} else {
			(*name)[k++] = str[i];
		}
	}
	(*name)[k] = 0;

	for (i = dot + 1, k = 0; i < j; i++) {
		if (str[i] == '\\') {
			continue;
		} else {
			(*class)[k++] = str[i];
		}
	}
	(*class)[k] = 0;

	if (!*name) {
		wfree(*name);
		*name = NULL;
	}
	if (!*class) {
		wfree(*class);
		*class = NULL;
	}
}

void SendHelperMessage(WScreen *scr, char type, int workspace, const char *msg)
{
	char *buffer;
	int len;
	int i;
	char buf[16];

	if (!scr->flags.backimage_helper_launched) {
		return;
	}

	len = (msg ? strlen(msg) : 0) + (workspace >= 0 ? 4 : 0) + 1;
	buffer = wmalloc(len + 5);
	snprintf(buf, sizeof(buf), "%4i", len);
	memcpy(buffer, buf, 4);
	buffer[4] = type;
	i = 5;
	if (workspace >= 0) {
		snprintf(buf, sizeof(buf), "%4i", workspace);
		memcpy(&buffer[i], buf, 4);
		i += 4;
		buffer[i] = 0;
	}
	if (msg)
		strcpy(&buffer[i], msg);

	if (write(scr->helper_fd, buffer, len + 4) < 0) {
		werror(_("could not send message to background image helper"));
	}
	wfree(buffer);
}

Bool UpdateDomainFile(WDDomain * domain)
{
	struct stat stbuf;
	char path[PATH_MAX];
	WMPropList *shared_dict, *dict;
	Bool result, freeDict = False;

	dict = domain->dictionary;
	if (WMIsPLDictionary(domain->dictionary)) {
		/* retrieve global system dictionary */
		snprintf(path, sizeof(path), "%s/WindowMaker/%s", SYSCONFDIR, domain->domain_name);
		if (stat(path, &stbuf) >= 0) {
			shared_dict = WMReadPropListFromFile(path);
			if (shared_dict) {
				if (WMIsPLDictionary(shared_dict)) {
					freeDict = True;
					dict = WMDeepCopyPropList(domain->dictionary);
					WMSubtractPLDictionaries(dict, shared_dict, True);
				}
				WMReleasePropList(shared_dict);
			}
		}
	}

	result = WMWritePropListToFile(dict, domain->path);

	if (freeDict) {
		WMReleasePropList(dict);
	}

	return result;
}

char *StrConcatDot(const char *a, const char *b)
{
	int len;
	char *str;

	if (!a)
		a = "";
	if (!b)
		b = "";

	len = strlen(a) + strlen(b) + 4;
	str = wmalloc(len);

	snprintf(str, len, "%s.%s", a, b);

	return str;
}

static char *getCommandForWindow(Window win, int elements)
{
	char **argv, *command = NULL;
	int argc;

	if (XGetCommand(dpy, win, &argv, &argc)) {
		if (argc > 0 && argv != NULL) {
			if (elements == 0)
				elements = argc;
			command = wtokenjoin(argv, WMIN(argc, elements));
			if (command[0] == 0) {
				wfree(command);
				command = NULL;
			}
		}
		if (argv) {
			XFreeStringList(argv);
		}
	}

	return command;
}

/* Free result when done */
char *GetCommandForWindow(Window win)
{
	return getCommandForWindow(win, 0);
}
