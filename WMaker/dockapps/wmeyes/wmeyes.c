#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <getopt.h>
#include <sys/time.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "XPM/wmeyesout0.xbm"
#include "XPM/wmeyesout1.xbm"
#include "XPM/wmeyesout2.xbm"
#include "XPM/wmeyesout3.xbm"
#include "XPM/wmeyesout4.xbm"
#include "XPM/wmeyesout5.xbm"
#include "XPM/wmeyesout6.xbm"
#include "XPM/wmeyesout7.xbm"
#include "XPM/wmeyesout8.xbm"
#include "XPM/wmeyesout9.xbm"
#include "XPM/wmeyesmask.xbm"
#include "XPM/wmeyespupil.xbm"
#include "XPM/tile.xpm"

Pixmap eyesmask;
Pixmap eyesout[11];
Pixmap eyespupil;
Pixmap currpmap;
Pixmap shapemask;
Pixmap backtile;

unsigned long eyecolor,outcolor,incolor;

typedef char bool;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define EYECOLOR "#009966"
#define OUTCOLOR "black"
#define INCOLOR "white"
#define WINDOWMAKER FALSE
#define USESHAPE FALSE
#define REPTIME  125

bool wmaker = WINDOWMAKER;
bool ushape = USESHAPE;
char dispname[256] = "";
char eyecol[256] = EYECOLOR;
char outcol[256] = OUTCOLOR;
char incol[256] = INCOLOR;
long repmsec = REPTIME;

Atom _XA_GNUSTEP_WM_FUNC;
Atom deleteWin;

Display *dpy;
Window Win[2];
Window Root;
XWMHints *hints;
GC WinGC;
GC mGC;
int activeWin;

void createWin(Window *win);
void repaint(XExposeEvent *ee);
void update();
void scanArgs(int argc,char *argv[]);
unsigned long getColor(char *colorName);

void do_nothing(void)
{
}

int main(int argc, char *argv[])
{
	XWMHints hints;
	XSizeHints shints;
	XGCValues gcv;
	unsigned long gcm;
	bool finished=FALSE;
	XpmAttributes pixatt;
	XEvent event;
	struct itimerval loopdelay;

	scanArgs(argc,argv);
	if((dpy = XOpenDisplay(dispname)) == NULL)  {
		fprintf(stderr,"Unable to open display: %s\n", dispname);
		exit(1);
	}
	_XA_GNUSTEP_WM_FUNC = XInternAtom(dpy, "_GNUSTEP_WM_FUNCTION", False);
	deleteWin = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	Root=DefaultRootWindow(dpy);
	createWin(&Win[0]);
	createWin(&Win[1]);
	hints.window_group = Win[0];
	shints.min_width=64;
	shints.min_height=64;
	shints.max_width=64;
	shints.max_height=64;
	shints.x=0;
	shints.y=0;
	if (wmaker)  {
		hints.initial_state = WithdrawnState;
		hints.icon_window = Win[1];
		hints.flags = WindowGroupHint | StateHint | IconWindowHint;
		shints.flags = PMinSize | PMaxSize | PPosition;
		activeWin=1;
	}
	else  {
		hints.initial_state = NormalState;
		hints.flags = WindowGroupHint | StateHint;
		shints.flags = PMinSize | PMaxSize;
		activeWin=0;
	}
	XSetWMHints(dpy, Win[0], &hints);
	XSetWMNormalHints(dpy, Win[0], &shints);
	XSetCommand(dpy, Win[0], argv, argc);
	XStoreName(dpy, Win[0], "wmeyes");
	XSetIconName(dpy, Win[0], "wmeyes");
	gcm = GCForeground|GCBackground|GCGraphicsExposures;
	gcv.graphics_exposures = False;
	WinGC = XCreateGC(dpy, Root, gcm, &gcv);
	XSelectInput(dpy, Win[activeWin], ExposureMask );
	XSetWMProtocols(dpy, Win[activeWin], &deleteWin, 1);

	eyecolor=getColor(eyecol);
	outcolor=getColor(outcol);
	incolor=getColor(incol);

	pixatt.exactColors=FALSE;
	pixatt.closeness=40000;
	pixatt.valuemask=XpmExactColors | XpmCloseness;
	eyesmask = XCreateBitmapFromData(dpy, Root, wmeyesmask_bits,
	                                 wmeyesmask_width, wmeyesmask_height);
	eyesout[0] = XCreateBitmapFromData(dpy, Root, wmeyesout0_bits,
	                                   wmeyesout0_width, wmeyesout0_height);
	eyesout[1] = XCreateBitmapFromData(dpy, Root, wmeyesout1_bits,
	                                   wmeyesout1_width, wmeyesout1_height);
	eyesout[2] = XCreateBitmapFromData(dpy, Root, wmeyesout2_bits,
	                                   wmeyesout2_width, wmeyesout2_height);
	eyesout[3] = XCreateBitmapFromData(dpy, Root, wmeyesout3_bits,
	                                   wmeyesout3_width, wmeyesout3_height);
	eyesout[4] = XCreateBitmapFromData(dpy, Root, wmeyesout4_bits,
	                                   wmeyesout4_width, wmeyesout4_height);
	eyesout[5] = XCreateBitmapFromData(dpy, Root, wmeyesout5_bits,
	                                   wmeyesout5_width, wmeyesout5_height);
	eyesout[6] = XCreateBitmapFromData(dpy, Root, wmeyesout6_bits,
	                                   wmeyesout6_width, wmeyesout6_height);
	eyesout[7] = XCreateBitmapFromData(dpy, Root, wmeyesout7_bits,
	                                   wmeyesout7_width, wmeyesout7_height);
	eyesout[8] = XCreateBitmapFromData(dpy, Root, wmeyesout8_bits,
	                                   wmeyesout8_width, wmeyesout8_height);
	eyesout[9] = XCreateBitmapFromData(dpy, Root, wmeyesout9_bits,
	                                   wmeyesout9_width, wmeyesout9_height);
	eyesout[10] = eyesmask;
	eyespupil = XCreateBitmapFromData(dpy, Root, wmeyespupil_bits,
	                                  wmeyespupil_width, wmeyespupil_height);
	XpmCreatePixmapFromData(dpy, Root, tile_xpm, &backtile, NULL, &pixatt);
	currpmap = XCreatePixmap(dpy, Root, 64, 64, DefaultDepth(dpy,DefaultScreen(dpy)));
	shapemask = XCreatePixmap(dpy, Root, 64, 64, 1);
	mGC = XCreateGC(dpy, shapemask, gcm, &gcv);

	update();

	XMapWindow(dpy, Win[0]);

	signal(SIGALRM, do_nothing);
	timerclear(&loopdelay.it_interval);
	timerclear(&loopdelay.it_value);
	loopdelay.it_interval.tv_sec = repmsec / 1000;
	loopdelay.it_interval.tv_usec = (repmsec % 1000) * 1000;
	loopdelay.it_value.tv_sec = repmsec / 1000;
	loopdelay.it_value.tv_usec = (repmsec % 1000) * 1000;
	setitimer(ITIMER_REAL, &loopdelay, 0);

	while (!finished)  {
		while (XPending(dpy))  {
			XNextEvent(dpy,&event);
			switch (event.type)  {
				case Expose:
					repaint((XExposeEvent *)&event);
					break;
				case ClientMessage:
					if (event.xclient.data.l[0] == deleteWin)  {
						finished=TRUE;
					}
					break;
			}
		}
		update();
		pause();
	}

	XFreeGC(dpy, WinGC);
	XFreeGC(dpy, mGC);
	XFreePixmap(dpy, currpmap);
	XFreePixmap(dpy, shapemask);
	XFreePixmap(dpy, eyesmask);
	XFreePixmap(dpy, eyesout[0]);
	XFreePixmap(dpy, eyesout[1]);
	XFreePixmap(dpy, eyesout[2]);
	XFreePixmap(dpy, eyesout[3]);
	XFreePixmap(dpy, eyesout[4]);
	XFreePixmap(dpy, eyesout[5]);
	XFreePixmap(dpy, eyesout[6]);
	XFreePixmap(dpy, eyesout[7]);
	XFreePixmap(dpy, eyesout[8]);
	XFreePixmap(dpy, eyesout[9]);
	XFreePixmap(dpy, eyespupil);
	XDestroyWindow(dpy, Win[0]);
	XDestroyWindow(dpy, Win[1]);
	XCloseDisplay(dpy);
	exit(0);
}

void createWin(Window *win)
{
	XClassHint classHint;
	*win = XCreateSimpleWindow(dpy, Root, 10, 10, 64, 64,0,0,0);
	classHint.res_name = "wmeyes";
	classHint.res_class = "WMeyes";
	XSetClassHint(dpy, *win, &classHint);
}

void repaint(XExposeEvent *ee)
{
	if (ee == NULL)  {
		XCopyArea(dpy,currpmap,Win[activeWin],WinGC,0,0,64,64,0,0);
	}
	else  {
		XCopyArea(dpy,currpmap,Win[activeWin],WinGC,ee->x,ee->y,ee->width,ee->height,ee->x,ee->y);
	}
}

void update()
{
	Window wroot, wchild;
	int    absx, absy;
	int    relx, rely;
	static int oldrelx=-1000;
	static int oldrely=-1000;
	static int idletime=0;
	static int masknum=0;
	unsigned int modmask;
	int    eyex,eyey,len;

	XQueryPointer(dpy,Win[activeWin],&wroot,&wchild,&absx,&absy,&relx,&rely,
	              &modmask);
	if (oldrelx == relx && oldrely == rely)  {
		idletime++;
	}
	else  {
		idletime = 0;
	}
	oldrelx = relx;
	oldrely = rely;
	if (idletime > 480)  {
		if (masknum < 10)  {
		   masknum++;
		}
	}
	else  {
		if (masknum > 0)  {
		   masknum--;
		}
	}
	XCopyArea(dpy, eyesmask, shapemask, mGC, 0,0,64,64,0,0);
	if(!(wmaker || ushape))  {
		XCopyArea(dpy, backtile, currpmap, WinGC, 0,0,64,64,0,0);
		XSetClipMask(dpy, WinGC, shapemask);
	}
	else  {
		XShapeCombineMask(dpy, Win[activeWin], ShapeBounding, 0, 0, shapemask, ShapeSet);
	}
	XSetBackground(dpy, WinGC, incolor);
	XSetForeground(dpy, WinGC, outcolor);
	XCopyPlane(dpy, eyesout[masknum], currpmap, WinGC, 0,0,64,64,0,0,1);
	XSetClipMask(dpy, WinGC, None);
	XSetForeground(dpy, WinGC, eyecolor);

	len = sqrt((relx-17.5)*(relx-17.5) + ((rely-32.5)*(rely-32.5)/4.0));
	if (len > 7.0)  {
		eyex = (int) (17.5 + (7.0*(relx-17.5))/len);
		eyey = (int) (32.5 + (7.0*(rely-32.5))/len);
	}
	else  {
		eyex = relx;
		eyey = rely;
	}
	XCopyPlane(dpy, eyespupil, currpmap, WinGC, 0,0,6,8,eyex-3,eyey-4,1);

	len = (int) sqrt((relx-47.5)*(relx-47.5) + ((rely-32.5)*(rely-32.5)/4.0));
	if (len > 7.0)  {
		eyex = (int) (47.5 + (7.0*(relx-47.5))/len);
		eyey = (int) (32.5 + (7.0*(rely-32.5))/len);
	}
	else  {
		eyex = relx;
		eyey = rely;
	}
	XCopyPlane(dpy, eyespupil, currpmap, WinGC, 0,0,6,8,eyex-3,eyey-4,1);

	XSetClipMask(dpy, WinGC, eyesout[masknum]);
	XSetForeground(dpy, WinGC, outcolor);
	XCopyPlane(dpy, eyesout[masknum], currpmap, WinGC, 0,0,64,64,0,0,1);
	XSetClipMask(dpy, WinGC, None);

	repaint(NULL);
}

void scanArgs(int argc, char *argv[])
{
	int c;
	int opt_index;
	bool helpflag = FALSE;
	bool errflag = FALSE;

	static struct option long_opts[] = {
		{"help",      no_argument,       NULL, 'h'},
		{"withdrawn", no_argument,       NULL, 'w'},
		{"shape",     no_argument,       NULL, 's'},
		{"pupil",     required_argument, NULL, 'p'},
		{"outside",   required_argument, NULL, 'o'},
		{"inside",    required_argument, NULL, 'i'},
		{"time",      required_argument, NULL, 't'},
		{"display",   required_argument, NULL, 'd'}};

	while(1)  {
		opt_index = 0;
		c = getopt_long_only(argc, argv, "hwsp:o:i:d:", long_opts, &opt_index);
		if (c == -1)  {
			break;
		}
		switch (c)  {
			case 'h':
				helpflag = TRUE;
				break;
			case 'w':
				wmaker = TRUE;
				break;
			case 's':
				ushape = TRUE;
				break;
			case 'p':
				strncpy(eyecol, optarg, 255);
				eyecol[255] = '\0';
				break;
			case 'o':
				strncpy(outcol, optarg, 255);
				outcol[255] = '\0';
				break;
			case 'i':
				strncpy(incol, optarg, 255);
				incol[255] = '\0';
				break;
			case 't':
				repmsec = atol(optarg);
				break;
			case 'd':
				strncpy(dispname, optarg, 255);
				dispname[255] = '\0';
				break;
			default:
				errflag = TRUE;
				break;
		}
	}
	if (errflag)  {
		fprintf(stderr, "Usage: %s [-h] [-ws] [-p pupclr] [-o outclr] [-i inclr] [-display disp]\n", argv[0]);
		exit(1);
	}
	if (helpflag)  {
		fprintf(stderr,"WMeyes 1.0 - The world's most useless WindowMaker dock app.\n");
		fprintf(stderr,"by Bryan Feir (jenora@istar.ca)\n\n");
		fprintf(stderr,"Usage: %s [-h] [-ws] [-p pupclr] [-o outclr] [-i inclr] [-display disp]\n", argv[0]);
		fprintf(stderr,"short  long      argument  description\n");
		fprintf(stderr,"  -h  --help               display this help screen\n");
		fprintf(stderr,"  -w  --withdrawn          withdraw window (for WindowMaker)\n");
		fprintf(stderr,"  -s  --shape              use shaped window\n");
		fprintf(stderr,"  -p  --pupil     pupclr   set the colour of the eyes' pupils (default %s)\n", EYECOLOR);
		fprintf(stderr,"  -o  --outside   outclr   set the outside colour of the eyes (default %s)\n", OUTCOLOR);
		fprintf(stderr,"  -i  --inside    inclr    set the inside colour of the eyes (default %s)\n", INCOLOR);
		fprintf(stderr,"  -t  --time      msec     set the interation time in msec (default %d)\n", REPTIME);
		fprintf(stderr,"  -d  --display   disp     set the X display to use\n");
		fprintf(stderr,"       -display   disp     set the X display to use\n");
		exit(0);
	}
}

unsigned long getColor(char *colorName)
{
	XColor Color;
	XWindowAttributes Attributes;

	XGetWindowAttributes(dpy, Root, &Attributes);
	Color.pixel = 0;

	XParseColor (dpy, Attributes.colormap, colorName, &Color);
	Color.flags=DoRed | DoGreen | DoBlue;
	XAllocColor (dpy, Attributes.colormap, &Color);

	return Color.pixel;
}

