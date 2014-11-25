/* Workspace.c- workspace options
 *
 *  WPrefs - Window Maker Preferences Program
 *
 *  Copyright (c) 2012 Daniel Déchelotte (heavily inspired from file (c) Alfredo K. Kojima)
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

#include "WPrefs.h"

typedef struct _Panel {
	WMBox *box;

	char *sectionName;

	char *description;

	CallbackRec callbacks;

	WMWidget *parent;

	WMFrame *autoDelayF[2];
	WMLabel *autoDelayL[4];
	WMButton *autoDelayB[4][5];
	WMTextField *autoDelayT[4];
	WMLabel *autoDelayMsL[4];
	
	WMFrame *dockF;
	WMButton *docksB[3];
} _Panel;

#define ICON_FILE	"dockclipdrawersection"

#define ARQUIVO_XIS	"xis"
#define DELAY_ICON "timer%i"
#define DELAY_ICON_S "timer%is"

static const struct {
	const char *key;
	const char *string;
} auto_delay[] = {
	{ "ClipAutoexpandDelay",   N_("Before auto-expansion") },
	{ "ClipAutocollapseDelay", N_("Before auto-collapsing") },
	{ "ClipAutoraiseDelay",    N_("Before auto-raise") },
	{ "ClipAutolowerDelay",    N_("Before auto-lowering") }
};


static char *autoDelayPresetValues[5] = { "0", "100", "250", "600", "1000" };

static const struct {
	const char *disable_key;
	const char *icon_file;
} dock_config[] = {
	{ "DisableDock",    "dock"   },
	{ "DisableClip",    "clip"   },
	{ "DisableDrawers", "drawer" }
};

static void showData(_Panel *panel);
static void storeData(_Panel *panel);


static void pushAutoDelayButton(WMWidget *w, void *data)
{
	_Panel *panel = (_Panel *) data;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 5; j++)
		{
			if (w == panel->autoDelayB[i][j])
			{
				WMSetTextFieldText(panel->autoDelayT[i], autoDelayPresetValues[j]);
				return;
			}
		}
	}
}

static void adjustButtonSelectionBasedOnValue(_Panel *panel, int row, const char *value)
{
	int j;

	if (!value)
		return;

	for (j = 0; j < 5; j++)
	{
		int isThatOne = !strcmp(autoDelayPresetValues[j], value);
		WMSetButtonSelected(panel->autoDelayB[row][j], isThatOne);
		if (isThatOne)
			return;
	}
}

static void autoDelayChanged(void *observerData, WMNotification *notification)
{
	_Panel *panel = (_Panel *) observerData;
	int row;
	WMTextField *anAutoDelayT = (WMTextField *) WMGetNotificationObject(notification);
	for (row = 0; row < 4; row++)
	{
		if (anAutoDelayT != panel->autoDelayT[row])
		{
			continue;
		}
		char *value = WMGetTextFieldText(anAutoDelayT);
		adjustButtonSelectionBasedOnValue(panel, row, value);
		free(value);
		return;
	}
}

static void pushDockButton(WMWidget *w, void *data)
{
	_Panel *panel = (_Panel *) data;
	WMButton *button = (WMButton *) w;
	if (button == panel->docksB[0] &&
	    !WMGetButtonSelected(panel->docksB[0]))
	{
		WMSetButtonSelected(panel->docksB[2], False);
	}
	if (button == panel->docksB[2] &&
	    WMGetButtonSelected(panel->docksB[2]))
	{
		WMSetButtonSelected(panel->docksB[0], True);
	}
}

static void createPanel(Panel *p)
{
	_Panel *panel = (_Panel *) p;
	WMScreen *scr = WMWidgetScreen(panel->parent);
	WMPixmap *icon1, *icon2;
	RImage *xis = NULL;
	RContext *rc = WMScreenRContext(scr);
	char *path;
	int i, j, k;
	char *buf1, *buf2;
	WMColor *color;
	WMFont *font;

	path = LocateImage(ARQUIVO_XIS);
	if (path) {
		xis = RLoadImage(rc, path, 0);
		if (!xis) {
			wwarning(_("could not load image file %s"), path);
		}
		wfree(path);
	}

	panel->box = WMCreateBox(panel->parent);
	WMSetViewExpandsToParent(WMWidgetView(panel->box), 2, 2, 2, 2);

	/***************** Auto-delays *****************/
	buf1 = wmalloc(strlen(DELAY_ICON) + 1);
	buf2 = wmalloc(strlen(DELAY_ICON_S) + 1);

	for (k = 0; k < 2; k++)
	{
		panel->autoDelayF[k] = WMCreateFrame(panel->box);
		WMResizeWidget(panel->autoDelayF[k], 370, 100);
		WMMoveWidget(panel->autoDelayF[k], 15, 10 + k * 110);
		if (k == 0)
			WMSetFrameTitle(panel->autoDelayF[k], _("Clip autocollapsing delays"));
		else
			WMSetFrameTitle(panel->autoDelayF[k], _("Clip autoraising delays"));

		for (i = 0; i < 2; i++)
		{
			panel->autoDelayL[i + k * 2] = WMCreateLabel(panel->autoDelayF[k]);
			WMResizeWidget(panel->autoDelayL[i + k * 2], 155, 20);
			WMMoveWidget(panel->autoDelayL[i + k * 2], 10, 27 + 40 * i);
			WMSetLabelText(panel->autoDelayL[i + k * 2], _(auto_delay[i + k * 2].string));
			/* WMSetLabelTextAlignment(panel->autoDelayL[i + k * 2], WARight); */

			for (j = 0; j < 5; j++)
			{
				panel->autoDelayB[i + k * 2][j] = WMCreateCustomButton(panel->autoDelayF[k], WBBStateChangeMask);
				WMResizeWidget(panel->autoDelayB[i + k * 2][j], 25, 25);
				WMMoveWidget(panel->autoDelayB[i + k * 2][j], 145 + (28 * j), 25 + 40 * i);
				WMSetButtonBordered(panel->autoDelayB[i + k * 2][j], False);
				WMSetButtonImagePosition(panel->autoDelayB[i + k * 2][j], WIPImageOnly);
				WMSetButtonAction(panel->autoDelayB[i + k * 2][j], pushAutoDelayButton, panel);
				if (j > 0)
					WMGroupButtons(panel->autoDelayB[i + k * 2][0], panel->autoDelayB[i + k * 2][j]);
				sprintf(buf1, DELAY_ICON, j);
				CreateImages(scr, rc, NULL, buf1, &icon1, NULL);
				if (icon1) {
					WMSetButtonImage(panel->autoDelayB[i + k * 2][j], icon1);
					WMReleasePixmap(icon1);
				} else {
					wwarning(_("could not load icon file %s"), buf1);
				}
				sprintf(buf2, DELAY_ICON_S, j);
				CreateImages(scr, rc, NULL, buf2, &icon2, NULL);
				if (icon2) {
					WMSetButtonAltImage(panel->autoDelayB[i + k * 2][j], icon2);
					WMReleasePixmap(icon2);
				} else {
					wwarning(_("could not load icon file %s"), buf2);
				}
			}

			panel->autoDelayT[i + k * 2] = WMCreateTextField(panel->autoDelayF[k]);
			WMResizeWidget(panel->autoDelayT[i + k * 2], 36, 20);
			WMMoveWidget(panel->autoDelayT[i + k * 2], 287, 27 + 40 * i);
			WMAddNotificationObserver(autoDelayChanged, panel, WMTextDidChangeNotification, panel->autoDelayT[i + k * 2]);

			color = WMDarkGrayColor(scr);
			font = WMSystemFontOfSize(scr, 10);
			panel->autoDelayMsL[i + k * 2] = WMCreateLabel(panel->autoDelayF[k]);
			WMResizeWidget(panel->autoDelayMsL[i + k * 2], 36, 16);
			WMMoveWidget(panel->autoDelayMsL[i + k * 2], 327, 33 + 40 *i);
			WMSetLabelText(panel->autoDelayMsL[i + k * 2], _("msec"));
			WMSetLabelTextColor(panel->autoDelayMsL[i + k * 2], color);
			WMSetLabelFont(panel->autoDelayMsL[i + k * 2], font);
			WMReleaseColor(color);
			WMReleaseFont(font);
		}

		WMMapSubwidgets(panel->autoDelayF[k]);
	}
	wfree(buf1);
	wfree(buf2);

	/***************** Enable/disable clip/dock/drawers *****************/
	panel->dockF = WMCreateFrame(panel->box);
	WMResizeWidget(panel->dockF, 115, 210);
	WMMoveWidget(panel->dockF, 390, 10);
	WMSetFrameTitle(panel->dockF, _("Dock/Clip/Drawer"));

	for (i = 0; i < 3; i++)
	{
		panel->docksB[i] = WMCreateButton(panel->dockF, WBTToggle);
		WMResizeWidget(panel->docksB[i], 56, 56);
		WMMoveWidget(panel->docksB[i], 30, 20 + 62 * i);
		WMSetButtonImagePosition(panel->docksB[i], WIPImageOnly);
		CreateImages(scr, rc, xis, dock_config[i].icon_file, &icon1, &icon2);
		if (icon2) {
			WMSetButtonImage(panel->docksB[i], icon2);
			WMReleasePixmap(icon2);
		}
		if (icon1) {
			WMSetButtonAltImage(panel->docksB[i], icon1);
			WMReleasePixmap(icon1);
		}
		switch(i)
		{
		case 0:
			WMSetBalloonTextForView(_("Disable/enable the application Dock (the\n"
						  "vertical icon bar in the side of the screen)."), WMWidgetView(panel->docksB[i]));
			break;
		case 1:
			WMSetBalloonTextForView(_("Disable/enable the Clip (that thing with\n"
						  "a paper clip icon)."), WMWidgetView(panel->docksB[i]));
			break;
		case 2:
			WMSetBalloonTextForView(_("Disable/enable Drawers (a dock that stores\n"
						  "application icons horizontally). The dock is required."), WMWidgetView(panel->docksB[i]));
			break;
		}
		WMSetButtonAction(panel->docksB[i], pushDockButton, panel);
	}
	
	WMMapSubwidgets(panel->dockF);

	if (xis)
		RReleaseImage(xis);

	WMRealizeWidget(panel->box);
	WMMapSubwidgets(panel->box);

	showData(panel);
}

static void storeData(_Panel *panel)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		char *str;

		str = WMGetTextFieldText(panel->autoDelayT[i]);
		SetStringForKey(str, auto_delay[i].key);
		wfree(str);
	}
	for (i = 0; i < 3; i++)
	{
		SetBoolForKey(!WMGetButtonSelected(panel->docksB[i]), dock_config[i].disable_key);
	}
}

static void showData(_Panel *panel)
{
	char *value;
	int i;
	for (i = 0; i < 4; i++)
	{
		value = GetStringForKey(auto_delay[i].key);
		WMSetTextFieldText(panel->autoDelayT[i], value);
		adjustButtonSelectionBasedOnValue(panel, i, value);
	}
	for (i = 0; i < 3; i++)
	{
		WMSetButtonSelected(panel->docksB[i], !GetBoolForKey(dock_config[i].disable_key));
	}
}

Panel *InitDocks(WMWidget *parent)
{
	_Panel *panel;

	panel = wmalloc(sizeof(_Panel));
	memset(panel, 0, sizeof(_Panel));

	panel->sectionName = _("Dock Preferences");

	panel->description = _("Dock and clip features.\n"
			       "Enable/disable the Dock and Clip, and tune some delays.");

	panel->parent = parent;

	panel->callbacks.createWidgets = createPanel;
	panel->callbacks.updateDomain = storeData;

	AddSection(panel, ICON_FILE);

	return panel;
}
