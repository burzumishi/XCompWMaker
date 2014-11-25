/* Copyright (c) 1999 Wee Liang (wliang@tartarus.uwa.edu.au)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/videodev.h>
#include <linux/soundcard.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>

#include "wmgeneral/wmgeneral.h"
#include "wmgeneral/misc.h"
#include "wmtv-master.xpm"
#include "channels.h"


/* Defines */
#define VERSION "0.6.5"

#define ON		1
#define OFF		0

#define NTFB	0		/* On/SetTune/Off Button */
#define SCANLB  1		/* Scan Left Button */
#define SCANRB	2		/* Scan Right Button */
#define FULLSB	3		/* Full Screen Toggle Button */

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define SETON		0
#define SETOFF		1
#define SETUNE		2
#define SETSPD		3

#define MAXCHAN		100
#define OPTIONS		"hvd:g:e:b:"

#define TELEVISION	0
#define COMPOSITE   1
#define SVIDEO		2


/* Global Variables */
int tfd;
int card_present = FALSE;
int ntfb_status  = SETOFF;
char *maxpreset = NULL;
char **chan	= NULL;
int mode_present = FALSE;

int chn = 0;
int aud = 0;
int tpst = 0;
int cchannel = 0;
int timebutton = 0;
int tvsource = 0;
int ccapt;
int norcfile = 0;
int ic = 0;
int cnotune = 0;
int vsource = 0;
int isource = 0;
int compchannel = 0;
int vidmode = 0;
int fmode = 0;
int mute = 0;
int dcret;
int tml;
int fswidth = 0;
int fsheight = 0;

unsigned long ccrfreq;
unsigned long rfreq;
unsigned long st;
unsigned long offset;

char *norm = NULL;
char *source;
char *mode = NULL;
char *fullscreen = NULL;
int  freqnorm = -1;
char *cname[MAXCHAN];
char *wcname[MAXCHAN];
int	 ftune[MAXCHAN];
char *progname;
char *dev = "/dev/video";

int  wmtv_mask_width = 64;
int  wmtv_mask_height = 64;
char wmtv_mask_bits[64*64];

int btime = 0;
int but_pressed = FALSE;
int but_clicked = FALSE;
int maxpst;
int x_lc;
int y_lc;
Time t_lc;

rckeys wmtv_keys[] = {
	{ "freqnorm", &norm },
	{ "source", &source },
	{ "maxpreset", &maxpreset },
	{ "mode", &mode },
	{ "fullscreen", &fullscreen },
	{ NULL, NULL }
};

XEvent Event;
XWindowAttributes Winattr;
Window fmwin;
GC	   fmGC;
XF86VidModeModeInfo **modelines, *fullscreenmode = NULL;
XF86VidModeModeLine	scmode;

struct video_capability vcap;
struct video_buffer vfb;
struct video_window vwin;
struct video_window vswin;
struct video_channel vchn, vchns;
struct video_picture vpic;
struct video_tuner vtun;
struct video_audio vaud;
struct video_mbuf vmbuf;
struct video_mmap vmmap;
struct video_clip vclip[2];
struct video_clip vclip2[2];


/* Function prototypes */
void TVOn(void);
void TVOff(void);
void TuneTV(void);
void ChanUp(void);
void ChanDown(void);
void FineTuneUp(void);
void FineTuneDown(void);
void ScanUp(void);
void ScanDown(void);

void ButtonUp(int);
void ButtonDown(int);
void Capture(void);
void MuteAudio(void);
void UnMuteAudio(void);
void VolumeUp(void);
void VolumeDown(void);
void DrawPresetChan(int);
void DoFullScreen(void);
void GetFrameBuffer(void);
void RetScreen(void);
void GrabImage(void);

void ParseRCFile(const char *, rckeys *);
void ParseRCFile2(const char *);
void WriteRCFile(const char *);
void InitConfig(void);
void InitPalette(void);

void Usage(void);
void Version(void);


/* main function */
int
main(int argc, char *argv[])
{
	int i, c, opind;
	// pid_t pid;
	char cfile[128];
	static struct option long_options[] = {
		{"display", 1, 0, 'd'},
		{"geometry", 1, 0, 'g'},
		{"bpp", 1, 0, 'b'},
		{"exe", 1, 0, 'e'},
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{0, 0, 0, 0}
	};

	progname = strdup(argv[0]);

	strncpy(cfile, (char *)getenv("HOME"), sizeof(char)*128);
	strcat(cfile, "/.wmtvrc");

	while (1) {
		opind = 0;
		c = getopt_long (argc, argv, OPTIONS, long_options, &opind);
		if (c == -1)
		break;

		switch(c)
			{
				case 0:
					/*
					fprintf(stderr, "wmtv: option - %s", long_options[opind].name);
					if (optarg)
						fprintf(stderr, " with arg %s", optarg);
					printf("\n");
					*/
					break;
				case 'd':
					display_name = strdup(optarg);
					break;
				case 'g':
					geometry = strdup(optarg);
					break;
				case 'e':
					exe = strdup(optarg);
					strcat(exe, " &");
					break;
				case 'b':
					fprintf(stderr, "wmtv: option not implemented yet\n");
					Usage();
					exit(1);
					break;
				case 'v':
					Version();
					exit(0);
					break;
				default:
					Usage();
					exit(1);
			}
	}

	/* creat windows */
	createXBMfromXPM (wmtv_mask_bits, wmtv_master_xpm,
					wmtv_mask_width, wmtv_mask_height);
	openXwindow (argc, argv, wmtv_master_xpm, wmtv_mask_bits,
					wmtv_mask_width, wmtv_mask_height);

	AddMouseRegion (NTFB, 47, 48, 59, 59);		/* On/SetTune/Off Button */
	AddMouseRegion (SCANLB, 23, 48, 35, 59);    /* Left Preset/Scan Button */
	AddMouseRegion (SCANRB, 35, 48, 47, 59); 	/* Right Preset/Scan Button */
	AddMouseRegion (FULLSB, 5, 5, 59, 44);		/* Toggle FullScreen */

	/* wmtv main loop */
	while (1)
	{
		while (XPending(display))
		{
			XNextEvent(display, &Event);
			switch (Event.type) {
				case Expose:
					RedrawWindow();
					break;
				case DestroyNotify:
					XCloseDisplay(display);
					ccapt = 0;
					if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
						perror("ioctl VIDIOCCAPTURE");
					}
					close(tfd);
					usleep(50000L);
					exit(0);
					break;
				case ButtonPress:
					if ((Event.xany.window == fmwin) && fmode) {
						RetScreen();
					}
					else {
					i = CheckMouseRegion (Event.xbutton.x, Event.xbutton.y);
					switch (i) {
						case NTFB:
							ButtonDown(NTFB);
							t_lc = Event.xbutton.time;
							but_pressed = TRUE;
							break;
						case SCANLB:
							ButtonDown(SCANLB);
							if (ntfb_status == SETUNE) {
								switch (Event.xbutton.button) {
									case 1:
										timebutton = 1;
										while (timebutton == 1)
										if (isource == TELEVISION)
										ScanDown();
										break;
									case 3:
										if (isource == TELEVISION)
										FineTuneDown();
										break;
								}
							}
							else
									ChanDown();
							break;
						case SCANRB:
							ButtonDown(SCANRB);
							if (ntfb_status == SETUNE) {
								switch (Event.xbutton.button) {
									case 1:
										timebutton = 1;
										while (timebutton)
										if (isource == TELEVISION)
										ScanUp();
										break;
									case 3:
										if (isource == TELEVISION)
										FineTuneUp();
										break;
								}
							}
							else
									ChanUp();
							break;
						case FULLSB:
							ButtonDown(FULLSB);
							switch (Event.xbutton.button) {
								case 1:
									but_pressed = TRUE;
									break;
								case 2:
									but_pressed = TRUE;
									break;
								case 3:
									if (!mute) {
										MuteAudio();
									}
									else {
										UnMuteAudio();
									}
									break;
							}
							break;
					}
					RedrawWindow();
					}
					break;
				case ButtonRelease:
					i = CheckMouseRegion (Event.xbutton.x, Event.xbutton.y);
					switch (i) {
						case NTFB:
							ButtonUp(NTFB);
								if (but_pressed) {
									if (Event.xbutton.time - t_lc >= 900) {
										btime = TRUE;
										but_pressed = FALSE;
									}
								}

								if (ntfb_status == SETOFF) {
										ntfb_status = SETON;
										TVOn();
								}
								else if (ntfb_status == SETON) {
										if (!btime) {
										ntfb_status = SETUNE;
										copyXPMArea(96, 79, 11, 7, 6, 50);
										RedrawWindowXYWH(6, 50, 11, 7);
										}
										else if (btime) {
											ntfb_status = SETOFF;
											btime = FALSE;
											ic = TRUE;
											TVOff();
										}
								}
								else if (ntfb_status == SETUNE) {
										if (!btime) {
											offset = (rfreq - ccrfreq);
											// fprintf(stderr, "wmtv: finetune offset = %ld\n", offset);
											WriteRCFile(cfile);
											ntfb_status = SETON;
											DrawPresetChan(cchannel);
										}
										else if (btime) {
											ntfb_status = SETOFF;
											btime = FALSE;
											ic = TRUE;
											TVOff();
										}
								}
							break;
						case SCANLB:
							ButtonUp(SCANLB);
							if (ntfb_status == SETUNE)
								timebutton = 0;
							break;
						case SCANRB:
							ButtonUp(SCANRB);
							if (ntfb_status == SETUNE)
								timebutton = 0;
							break;
						case FULLSB:
							ButtonUp(FULLSB);
								switch (Event.xbutton.button) {
									case 1: {
										if (but_clicked) {
											but_pressed = FALSE;
											but_clicked = FALSE;

											if (((Event.xbutton.time - t_lc) <= 500) && (abs(Event.xbutton.x - x_lc) <= 10) && (abs(Event.xbutton.y - y_lc) <= 10) )
											{
												if ((ntfb_status == SETON) || (ntfb_status == SETUNE)) {
													if (exe) {
														ntfb_status = SETOFF;
														TVOff();
														system(exe);
														/*
														pid = fork();

														// child
														if (pid == (pid_t) 0) {
														execlp("xawtv", "xawtv", "&", (char *) 0);
														}

														else if (pid < (pid_t) 0) {
															perror("fork");
														}

														// parent
														else {
															if (waitpid(pid, NULL, 0) < 0) {
																perror("waitpid");
														*/
													}
													else {
														DoFullScreen();
													}
												}
											}
										}

										if (but_pressed) {
											t_lc = Event.xbutton.time;
											x_lc = Event.xbutton.x;
											y_lc = Event.xbutton.y;
											but_clicked = TRUE;
											but_pressed = FALSE;
										}
									}
										break;
									case 2: {
										if (but_clicked) {
											but_pressed = FALSE;
											but_clicked = FALSE;

											if (((Event.xbutton.time - t_lc) <= 500) && (abs(Event.xbutton.x - x_lc) <= 10) && (abs(Event.xbutton.y - y_lc) <= 10) )
											{
												if ((ntfb_status == SETON) || (ntfb_status == SETUNE)) {
														DoFullScreen();
												}
											}
										}

										if (but_pressed) {
											t_lc = Event.xbutton.time;
											x_lc = Event.xbutton.x;
											y_lc = Event.xbutton.y;
											but_clicked = TRUE;
											but_pressed = FALSE;
										}
									}
									break;
								case 3:
									break;
							}
							break;
					}
					RedrawWindow();
					break;
				case VisibilityNotify:
						switch(Event.xvisibility.state) {
							case VisibilityFullyObscured:
								ccapt = 0;
								if (tfd && (ntfb_status != SETOFF) && !fmode) {
									if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
										perror("ioctl VIDIOCCAPTURE");
									}
								}
								break;
							case VisibilityPartiallyObscured:
								ccapt = 0;
								if (tfd && (ntfb_status != SETOFF) && !fmode) {
									if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
										perror("ioctl VIDIOCCAPTURE");
									}
								}
								break;
							case VisibilityUnobscured:
								ccapt = 1;
								if (tfd && (ntfb_status != SETOFF) && !fmode) {
									if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
										perror("ioctl VIDIOCCAPTURE");
									}
								}
								break;
							}
					RedrawWindow();
					break;
				case KeyPress:
					if ((Event.xany.window == fmwin) && fmode) {
							switch(XKeycodeToKeysym(display, Event.xkey.keycode, 0)) {
								case XK_Up:
									if (isource == TELEVISION)
									ChanUp();
									break;
								case XK_Down:
									if (isource == TELEVISION)
									ChanDown();
									break;
								case XK_Right:
									if (isource == TELEVISION)
									FineTuneUp();
									break;
								case XK_Left:
									if (isource == TELEVISION)
									FineTuneDown();
									break;
								case XK_m:
									if (!mute) {
										MuteAudio();
									}
									else {
										UnMuteAudio();
									}
									break;
								case XK_Escape:
									RetScreen();
									break;
								default:
									break;
							}
					}
				default:
					break;
			}
		XFlush(display);
		}
	    usleep(50000L);
	}
	return(0);
}


/* ButtonDown function */
void
ButtonDown(int button)
{
	switch (button) {
		case NTFB:
				copyXPMArea(79, 100, 12, 11, 47, 48);
				RedrawWindowXYWH (47, 48, 12, 11);
				break;
		case SCANLB:
				copyXPMArea(55, 100, 12, 11, 23, 48);
				RedrawWindowXYWH(35, 48, 12, 11);
				break;
		case SCANRB:
				copyXPMArea(67, 100, 12, 11, 35, 48);
				RedrawWindowXYWH(35, 48, 12, 11);
				break;
		case FULLSB:
				break;
	}
}


/* ButtonUp function */
void
ButtonUp(int button)
{
	switch (button) {
		case NTFB:
				copyXPMArea(79, 88, 12, 11, 47, 48);
        		RedrawWindowXYWH(47, 48, 12, 11);
				break;
		case SCANLB:
				copyXPMArea(55, 88, 12, 11, 23, 48);
				RedrawWindowXYWH(23, 48, 12, 11);
				break;
		case SCANRB:
				copyXPMArea(67, 88, 12, 11, 35, 48);
				RedrawWindowXYWH(35, 48, 12, 11);
				break;
		case FULLSB:
				break;
	}
}


/* TVOn function */
void
TVOn(void)
{
	int i;
	Window junkwin;
	int rx, ry;
	char *p;


	XWindowAttributes winattr;

	if (!XGetWindowAttributes (display, iconwin, &winattr)) {
		fprintf(stderr, "wmtv: error getting winattr of iconwin\n");
	}

	if (!XTranslateCoordinates(display, iconwin, winattr.root,
							-winattr.border_width,
							-winattr.border_width,
							&rx, &ry, &junkwin)) {
		fprintf(stderr, "wmtv: error translating coordinates\n");
	}

	InitConfig();
	GetFrameBuffer();
	InitPalette();

	ccapt = 1;

	vwin.x		= rx;
	vwin.y		= ry + 5;

	vwin.width  = 64;
	vwin.height = 39;

	vclip[0].x = 0;					/* Clipping Rect 1 */
	vclip[0].y = 0;
	vclip[0].width = 5;
	vclip[0].height = 39;
	vclip[1].x = 59;				/* Clipping Rect 2 */
	vclip[1].y = 0;
	vclip[1].width = 5;
	vclip[1].height = 39;

	vwin.clips  = vclip;
	vwin.clipcount = 2;

	vchn.channel = tvsource;
	vaud.audio = tvsource;

	if (source) {
		p = source;
		if (*p == 'T')
			isource = TELEVISION;
		else if (*p == 'C') {
			isource = COMPOSITE;
			while (isalpha(*p))
				++p;
			compchannel = atoi(p);
		}
		else if (*p == 'S')
			isource = SVIDEO;
	}

	if (!cnotune) {
		if (ioctl(tfd, VIDIOCGTUNER, &vtun) < 0)
			perror("ioctl VIDIOCGTUNER");
		if (vtun.flags & VIDEO_TUNER_LOW)
			st = 16000;
		else
			st = 16;
	}

	if (ioctl(tfd, VIDIOCGCHAN, &vchn) < 0)
		perror("ioctl VIDIOCGCHAN");

	if (vchn.flags & VIDEO_VC_AUDIO) {
		if (ioctl(tfd, VIDIOCGAUDIO, &vaud) < 0)
			perror("ioctl VIDIOCGAUDIO");
	}

		if (vaud.flags & VIDEO_AUDIO_MUTE) {
			vaud.flags &= ~VIDEO_AUDIO_MUTE; 	/* Unmute */
			vaud.volume = (0xFFFF/2)+1;
		}

	if (isource == TELEVISION) {
		for (i=0; i < CHAN_ENTRIES; i++) {
			if (!strcmp(cname[cchannel], tvtuner[i].name)) {
				rfreq = ((tvtuner[i].freq[freqnorm] / 1000) * st) + ftune[cchannel];
				ccrfreq = rfreq - ftune[cchannel];
			break;
			}
		}
		DrawPresetChan(cchannel);
	}
	else if (isource == COMPOSITE) {
		DrawPresetChan(compchannel);
	}
	else if (isource == SVIDEO) {
		DrawPresetChan(-1);
	}

	if (vchn.flags & VIDEO_VC_AUDIO) {
		if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");

	}

	if (ioctl(tfd, VIDIOCSCHAN, &vchn) < 0)
			perror("ioctl VIDIOCSCHAN");

	if (!cnotune) {
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0)
			perror("ioctl VIDIOCSFREQ");
	}

	if (ioctl(tfd, VIDIOCSWIN, &vwin) < 0)
			perror("ioctl VIDIOCSWIN");

	if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0)
			perror("ioctl VIDIOCCAPTURE");
}


/* TVOff function */
void
TVOff(void)
{
	ccapt = 0;

	if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0)
			perror("ioctl VIDIOCCAPTURE");

	vaud.audio  = tvsource;
	vaud.volume = 0;
	vaud.flags |= VIDEO_AUDIO_MUTE;

	if (vchn.flags & VIDEO_VC_AUDIO) {
		if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");
	}

	close(tfd);
	copyXPMArea(66, 79, 11, 7, 6, 50);
	RedrawWindowXYWH(6, 50, 11, 7);
}


/* VolumeUp function */
void
VolumeUp(void)
{
	if(vchn.flags & VIDEO_VC_AUDIO) {
		if (vaud.volume <= 0xFFFF) {
			vaud.audio = tvsource;
			vaud.volume += (0xFFFF/10);
			if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");
		}
	}

}

/* VoumeDown function */
void
VolumeDown(void)
{
	if (vchn.flags & VIDEO_VC_AUDIO) {
		if (vaud.volume > 0) {
			vaud.audio = tvsource;
			vaud.volume -= (0xFFFF/10);
			if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");
		}
	}
}

/* MuteAudio function */
void
MuteAudio(void)
{
	if (vchn.flags & VIDEO_VC_AUDIO) {
		vaud.audio = tvsource;
		// vaud.volume = 0;
		vaud.flags |= VIDEO_AUDIO_MUTE;
		if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");
	}
	mute = TRUE;
}


/* UnMuteAudio function */
void
UnMuteAudio(void)
{
	if ((vchn.flags & VIDEO_VC_AUDIO) && (vaud.flags & VIDEO_AUDIO_MUTE)) {
		vaud.audio = tvsource;
		// vaud.volume = (0xFFFF/2)+1;
		vaud.flags &= ~VIDEO_AUDIO_MUTE;
		if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0)
				perror("ioctl VIDIOCSAUDIO");
	}
	mute = FALSE;
}


/* ScanUp function */
void
ScanUp(void)
{
	rfreq += 2;
	usleep(50000L);
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0) {
		perror("ioctl VIDIOCSFREQ");
	}
	usleep(10000L);
	vtun.tuner = 0;
	if (ioctl(tfd, VIDIOCGTUNER, &vtun) < 0) {
		perror("ioctl VIDIOCGTUNER");
	}
	if (vtun.signal == 0xFFFF) {
		timebutton = 0;
	}

}


/* ScanDown function */
void
ScanDown(void)
{
	rfreq -= 2;
	usleep(50000L);
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0) {
		perror("ioctl VIDIOCSFREQ");
	}
	usleep(10000L);
	vtun.tuner = 0;
	if (ioctl(tfd, VIDIOCGTUNER, &vtun) < 0) {
		perror("ioctl VIDIOCGTUNER");
	}
	if (vtun.signal == 0xFFFF) {
		timebutton = 0;
	}
}


/* FineTuneUp function */
void
FineTuneUp(void)
{
	rfreq += 1;
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0) {
		perror("ioctl VIDIOCSFREQ");
	}
}


/* FineTuneDown function */
void
FineTuneDown(void)
{
	rfreq -= 1;
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0) {
		perror("ioctl VIDIOCSFREQ");
	}
}


/* ChanUp function */
void
ChanUp(void)
{
	int i;
	if (cchannel != maxpst)
	++cchannel;
	for (i=0; i < CHAN_ENTRIES; i++) {
		if (!strcmp(cname[cchannel], tvtuner[i].name)) {
				rfreq = ((tvtuner[i].freq[freqnorm] / 1000) * st) + ftune[cchannel];
				ccrfreq = rfreq - ftune[cchannel];
		break;
		}
	}

	DrawPresetChan(cchannel);
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0)
		perror("ioctl VIDIOCSFREQ");
}


/* ChanDown function */
void
ChanDown(void)
{
	int i;
	if (cchannel != 0)
	--cchannel;
	for (i=0; i < CHAN_ENTRIES; i++) {
		if (!strcmp(cname[cchannel], tvtuner[i].name)) {
				rfreq = ((tvtuner[i].freq[freqnorm] / 1000) * st) + ftune[cchannel];
				ccrfreq = rfreq - ftune[cchannel];
		break;
		}
	}

	DrawPresetChan(cchannel);
	if (ioctl(tfd, VIDIOCSFREQ, &rfreq) < 0)
		perror("ioctl VIDIOCSFREQ");
}


/* DrawPresetChan function */
void
DrawPresetChan(int cchannel)
{
	char temp[10];
	char *p = temp;
	int j=0;
	int k=6;

	if (isource == TELEVISION) {
		sprintf(temp, "%02d", cchannel);

		if (*p == '0') {
			copyXPMArea(66, 79, 5, 7, k, 50);
			k += 6;
			j++;
			p++;
		}
		while (j < 2) {
			copyXPMArea((*p-'0')*6 + 1, 79, 5, 7, k, 50);
			k += 6;
			p++;
			j++;
		}
	}

	else if (isource == COMPOSITE) {
		copyXPMArea (109, 79, 5, 7, 6, 50);
		copyXPMArea ((compchannel*6) + 1, 79, 5, 7, 12, 50);
	}

	else if (isource == SVIDEO) {
		copyXPMArea(116, 79, 11, 7, 6, 50);
	}

	RedrawWindowXYWH(6, 50, 11, 7);
}


/* ParseRCFile function */
void
ParseRCFile(const char *filename, rckeys *keys)
{
	char	*p,*q;
	char	temp[128];
	char	*tokens = " =\t\n";
	FILE	*fp;
	int		key = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "wmtv: %s\n", strerror(errno));
		norcfile = 1;
		return;
	}
	norcfile = 0;
	while (fgets(temp, 128, fp)) {
		key = 0;
		q = strdup(temp);
		if (*q != '\n') {
		q = strtok(q, tokens);
			while (key >= 0 && keys[key].label) {
				if ((!strcmp(q, keys[key].label))) {
					p = strtok(NULL, tokens);
					free(*keys[key].var);
					*keys[key].var = strdup(p);
					key = -1;
				} else key++;
			}
		}
		free(q);
	}
	fclose(fp);
}


/* ParseRCFile2 function */
void
ParseRCFile2(const char *filename)
{
	int  menu = FALSE;
	char temp[128];
	char tp[10];
	char *tokens = " \t\n()";
	char *q, *p;
	FILE *fp;
	int i = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "wmtv: %s\n", strerror(errno));
		norcfile = 1;
		return;
	}
	norcfile = 0;
	while (fgets(temp, 128, fp)) {
		q = strdup(temp);
		if (*q != '\n') {
			q = strtok(q, tokens);
			if (menu) {
				cname[i] = (char *)malloc(sizeof(q));
				strncpy(cname[i], q, sizeof(q));
				p = q;
				p = strtok(NULL, tokens);
				if (p != NULL) {
					if (*p == '-') {
						p++;
						ftune[i] = -1*atoi(p);
					}
					else if (*p == '+') {
						p++;
						ftune[i] = atoi(p);
					}
					else {
						ftune[i] = atoi(p);
					}
				}
				else {
					ftune[i] = 0;
				}
				wcname[i] = (char *)malloc(sizeof(cname[i])+sizeof(p));
				wcname[i] = strdup(cname[i]);
				sprintf(tp, " (%d) ", ftune[i]);
				strcat(wcname[i], tp);
				i++;
				tpst = i;
			}
			if ((q = strchr(q, '[')) != NULL) {
				menu = TRUE;
			}
		}
	}
	fclose(fp);
}


/* WriteRCFile function */
void
WriteRCFile(const char *filename)
{
	int i;
	char temp[128];
	char tp[10];
	char *tokens = " \t\n()";
	char *q;
	FILE *fp;

	if ((fp = fopen(filename, "r+")) == NULL) {
		fprintf(stderr, "wmtv: %s\n", strerror(errno));
		return;
	}


	while (fgets(temp, 128, fp)) {
		q = strdup(temp);
		if (*q != '\n') {
			q = strtok(temp, tokens);
			if ((q = strchr(q, '[')) != NULL) {
				for (i = 0; i <= maxpst; i++) {
					// fprintf(stderr, "offset is %ld\n", offset);
					sprintf(tp, " (%ld) ", offset);
					strtok(wcname[cchannel], tokens);
					strcat(wcname[cchannel], tp);
					fputs(wcname[i], fp);
					fputs("\n", fp);
				}
			}
		}
	}
	fclose(fp);
}


/* InitPalette function */
void
InitPalette(void)
{
	if (ioctl(tfd, VIDIOCGFBUF, &vfb) < 0) {
			perror ("ioctl VIDIOCGFBUF");
			exit(-1);
	}
		if (vfb.base == NULL) {
			fprintf(stderr, "wmtv: no physical frame buffer access\n");
			exit(1);
		}


	if (ioctl(tfd, VIDIOCGPICT, &vpic) < 0) {
			perror("ioctl VIDIOCGPICT");
	}
	vpic.depth = vfb.depth;
	switch(vpic.depth) {
		case 8:
			vpic.palette = VIDEO_PALETTE_HI240;
			break;
		case 15:
			vpic.palette = VIDEO_PALETTE_RGB555;
			break;
		case 16:
			vpic.palette = VIDEO_PALETTE_RGB565;
			break;
		case 24:
			vpic.palette = VIDEO_PALETTE_RGB24;
			break;
		case 32:
			vpic.palette = VIDEO_PALETTE_RGB32;
			break;
		default:
			fprintf(stderr, "wmtv: unsupported depth %d\n", vpic.depth);
			exit(-1);
	}
	if (ioctl(tfd, VIDIOCSPICT, &vpic) < 0) {
			perror("ioctl VIDIOCSPICT");
	}
}
/* InitConfig function */
void
InitConfig(void)
{
	int i;
	char temp[128];

	strcpy(temp, "/etc/wmtvrc");
	ParseRCFile(temp, wmtv_keys);
	ParseRCFile2(temp);

	strncpy(temp, (char *)getenv("HOME"), (sizeof(char)*128));
	strcat(temp, "/.wmtvrc");
	ParseRCFile(temp, wmtv_keys);
	ParseRCFile2(temp);

	if (norcfile) {
		fprintf(stderr, "wmtv: error - config file not found\n");
		exit(1);
	}

	if (maxpreset != NULL)
		maxpst = atoi(maxpreset) - 1;
	else
		maxpst = 1;

	if ((tpst) != atoi(maxpreset)) {
		fprintf(stderr, "wmtv: error - maxpreset value does not match total channel value\n");
		exit(1);
	}
	if ((tfd = open(dev, O_RDWR)) < 0) {
		perror("open failed");
	}
	card_present = TRUE;

	if (norm)
	for (i=0; i < CHAN_NAMES; i++) {
		if (!strcmp(chan_names[i].cname, norm)) {
			freqnorm = chan_names[i].cind;
		}
	}

	if (freqnorm == -1) {
		fprintf(stderr, "wmtv: error - set freqnorm in config file\n");
		exit(1);
	}

	if (ioctl(tfd, VIDIOCGCAP, &vcap) < 0) {
		perror("ioctl VIDIOCGCAP");
	}

	if (!(vcap.type & VID_TYPE_SCALES)) {
		fprintf(stderr, "%s: video device does not support scalling\n", progname);
		exit(1);
	}
	if (!(vcap.type & VID_TYPE_CLIPPING)) {
		fprintf(stderr, "%s: video device does not support clipping\n", progname);
	}

	if (fullscreen) {
		fswidth = atoi(strtok(fullscreen, "x\n"));
		fsheight = atoi(strtok(NULL, "x\n"));
	}
	else {
		fswidth = 640;
		fsheight = 480;
	}

	if (source) {
		for (i=0; i < vcap.channels; i++) {
			vchn.channel = i;
			if (ioctl(tfd, VIDIOCGCHAN, &vchn) < 0) {
				perror("ioctl VIDIOCGCHAN");
			}
			if (!strcmp(vchn.name, source)) {
				if (vchn.tuners)
					cnotune = 0;
				else
					cnotune = 1;

				vsource = 1;
				tvsource = vchn.channel;

				if ((ioctl(tfd, VIDIOCSCHAN, &vchn)) < 0)
					perror("ioctl VIDIOCSCHAN");
			}
		}
		if (!vsource)
			fprintf(stderr, "wmtv: invalid source in config file\n");
	}

	if (mode) {
		if (!cnotune) {
			vtun.tuner = 0;
			if (ioctl(tfd, VIDIOCGTUNER, &vtun) < 0) {
				perror("ioctl VIDIOCGTUNER");
			}

				if (!strcmp(mode, "pal")) {
					if (vtun.flags & VIDEO_TUNER_PAL) {
						vtun.mode = VIDEO_MODE_PAL;
						if (ioctl(tfd, VIDIOCSTUNER, &vtun) < 0) {
							perror("ioctl VIDIOCSTUNER");
						}
					}
				}
				if (!strcmp(mode, "ntsc")) {
					if (vtun.flags & VIDEO_TUNER_NTSC) {
						vtun.mode = VIDEO_MODE_NTSC;
						if (ioctl(tfd, VIDIOCSTUNER, &vtun) < 0) {
							perror("ioctl VIDIOCSTUNER");
						}
					}
				}
				if (!strcmp(mode, "secam")) {
					if (vtun.flags & VIDEO_TUNER_SECAM) {
						vtun.mode = VIDEO_MODE_SECAM;
						if (ioctl(tfd, VIDIOCSTUNER, &vtun) < 0) {
							perror("ioctl VIDIOCSTUNER");
						}
					}
				}
		}
	}

	for (i=0; i < vcap.audios; i++) {
		vaud.audio = i;
		if (ioctl(tfd, VIDIOCGAUDIO, &vaud) < 0) {
			perror("ioctl VIDIOCGAUDIO");
		}
		if (!(vaud.flags & VIDEO_AUDIO_MUTE)) {
			vaud.flags |= VIDEO_AUDIO_MUTE;
			if (ioctl(tfd, VIDIOCSAUDIO, &vaud) < 0) {
				perror("ioctl VIDIOCSAUDIO");
			}
		}
	}

	if (ioctl(tfd, VIDIOCGFBUF, &vfb) < 0) {
		perror("ioctl VIDIOCGFBUF");
	}
}

/* GetFrameBuffer function */
void
GetFrameBuffer(void)
{
	void *baseaddr = NULL;
	int evbr, erbr, flr = 0;
	int bankr, memr, depth;
	int i, n;
	int bytesperline, bitsperpixel;
	XPixmapFormatValues *pf;

	if (!XGetWindowAttributes(display, DefaultRootWindow(display), &Winattr)) {
		fprintf(stderr, "wmtv: error getting winattr of root\n");
	}

	depth = Winattr.depth;

	if (XF86DGAQueryExtension(display, &evbr, &erbr)) {
			XF86DGAQueryDirectVideo(display, XDefaultScreen(display), &flr);
			if (flr & XF86DGADirectPresent) {
					XF86DGAGetVideoLL(display, XDefaultScreen(display),
							(int *) &baseaddr, &bytesperline, &bankr, &memr);
			}
	}
	else {
			fprintf(stderr, "wmtv: error - XF86DGA Extensions not available\n");
			exit(1);
	}


	pf = XListPixmapFormats(display, &n);
	for (i=0; i < n; i++) {
			if (pf[i].depth == depth) {
				depth = pf[i].bits_per_pixel;
				break;
			}
	}

	bitsperpixel = (depth+7) & 0xf8;				    /* Taken from */
	bytesperline = Winattr.width * bitsperpixel / 8;    /* Gerd Knorr's xawtv */

	vfb.base = baseaddr;
	vfb.height = Winattr.height;
	vfb.width  = Winattr.width;
	vfb.depth  = bitsperpixel;
	vfb.bytesperline = bytesperline;

	if (Winattr.depth == 15)
		vfb.depth = 15;

	if (ioctl(tfd, VIDIOCSFBUF, &vfb) < 0) {
		perror("ioctl VIDIOCSFBUF");
	}
}


/* DoFullScreen function */
void
DoFullScreen(void)
{
	int i;
	int evbr, erbr = 0;
	int rx, ry;
	unsigned long back_pix, fore_pix;
	unsigned int borderwidth = 0;

	Window junkwin;
	XSizeHints fmsizehints;
	XWMHints   fmxwmhints;
	XGCValues  gcv;
	unsigned long gcm;
	unsigned long valuemask;

	XSetWindowAttributes fmWinattr;
	XWindowAttributes fmwinattr;


	ccapt = 0;
	if (ioctl (tfd, VIDIOCCAPTURE, &ccapt) < 0) {
		perror("ioctl VIDIOCCAPTURE");
	}

	fmsizehints.x	= 0;
	fmsizehints.y	= 0;
	fmsizehints.width = fswidth;
	fmsizehints.height = fsheight;
	fmsizehints.max_width = vcap.maxwidth;
	fmsizehints.max_height = vcap.maxheight;
	fmsizehints.min_width = fswidth;
	fmsizehints.min_height = fsheight;
	fmsizehints.flags = (USPosition | USSize | PSize | PMinSize | PMaxSize);

	valuemask = 0;

	back_pix = WhitePixel(display, DefaultScreen(display));
	fore_pix = BlackPixel(display, DefaultScreen(display));

	fmwin = XCreateWindow(display, DefaultRootWindow(display), fmsizehints.x,
					fmsizehints.y, fmsizehints.width, fmsizehints.height,
					borderwidth, CopyFromParent, InputOutput,
					CopyFromParent, valuemask, &fmWinattr);


	XSetWMNormalHints(display, fmwin, &fmsizehints);

	gcm = (GCForeground | GCBackground | GCGraphicsExposures);
	gcv.foreground = fore_pix;
	gcv.background = back_pix;
	gcv.graphics_exposures = 0;

	fmGC = XCreateGC(display, DefaultRootWindow(display), gcm, &gcv);

	XSelectInput(display, fmwin, ButtonPressMask | ExposureMask |
					ButtonReleaseMask | PointerMotionMask |
					StructureNotifyMask | VisibilityChangeMask | KeyPressMask );

	fmxwmhints.flags = StateHint;
	fmxwmhints.initial_state = NormalState;

	XSetWMHints(display, fmwin, &fmxwmhints);


	if (XF86VidModeQueryExtension(display, &evbr, &erbr)) {
		vidmode = TRUE;
	}
	else {
		fprintf(stderr, "wmtv: xf86mode extension not present\n");
		fprintf(stderr, "      switch disabled\n");
	}


	XMapWindow(display, fmwin);

	usleep(50000L);
	if (!XGetWindowAttributes(display, fmwin, &fmwinattr)) {
		fprintf(stderr, "wmtv: error getting winattr of fmwin\n");
	}

	usleep(50000L);
	if (!XTranslateCoordinates(display, fmwin, DefaultRootWindow(display),
					-fmwinattr.border_width,
					-fmwinattr.border_width,
					&rx, &ry, &junkwin)) {
		fprintf(stderr, "wmtv: error translating coordinates for fmwin");
	}

	vswin.width  	= fswidth;
	vswin.height 	= fsheight;
	vswin.x 		= fmsizehints.x + rx;
	vswin.y			= fmsizehints.y + ry;
	vswin.clipcount = 0;

	if (ioctl(tfd, VIDIOCSWIN, &vswin) < 0) {
		perror("ioctl VIDIOCSWIN");
	}

	ccapt = 1;
	if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
		perror("ioctl VIDIOCCAPTURE");
	}
	fmode = TRUE;

	if (vidmode) {
		XF86VidModeGetModeLine(display, XDefaultScreen(display), &dcret, &scmode);
		XF86VidModeGetAllModeLines(display, XDefaultScreen(display), &tml, &modelines);

		for (i=0; i < tml; i++) {
			if ((modelines[i]->hdisplay == fswidth) &&
						(modelines[i]->vdisplay == fsheight)) {
				fullscreenmode = modelines[i];
				mode_present = TRUE;
				break;
			}
		}

		if (!mode_present) {
		fprintf(stderr, "wmtv: mode line for resolution %d x %d not found\n", vcap.maxwidth, vcap.maxheight);
		}

		XRaiseWindow(display, fmwin);
		if (mode_present) {

			XF86VidModeSwitchToMode(display, XDefaultScreen(display), fullscreenmode);
			XF86VidModeSetViewPort(display, XDefaultScreen(display), vswin.x, vswin.y);
			XGrabPointer(display, fmwin, True, 0, GrabModeAsync, GrabModeAsync,
						fmwin, None, CurrentTime);
		}
	}
}


/* RetScreen function */
void
RetScreen()
{
	int i;
	XF86VidModeModeInfo *scm = NULL;

	ccapt = 0;
	if (ioctl(tfd, VIDIOCCAPTURE, &ccapt)) {
		perror("ioctl VIDIOCCAPTURE");
	}

	if (mode_present) {
		for (i = 0; i < tml; i++) {
			if ((modelines[i]->hdisplay == Winattr.width) &&
							(modelines[i]->vdisplay==Winattr.height))
				scm = modelines[i];
		}

		scm->dotclock 	= dcret;
		scm->hdisplay 	= scmode.hdisplay;
		scm->hsyncstart = scmode.hsyncstart;
		scm->hsyncend 	= scmode.hsyncend;
		scm->htotal 	= scmode.htotal;
		scm->vdisplay 	= scmode.vdisplay;
		scm->vsyncstart = scmode.vsyncstart;
		scm->vsyncend 	= scmode.vsyncend;
		scm->vtotal 	= scmode.vtotal;
		scm->flags 		= scmode.flags;
		/*
		scm->privsize = scmode.privsize;
		scm->private = scmode.private;
		*/

		XClearWindow(display, fmwin);
		XFreeGC(display, fmGC);
		XMapRaised(display, fmwin);
		XUnmapWindow(display, fmwin);
		XUngrabPointer(display, CurrentTime);
	}
	else {
		XClearWindow(display, fmwin);
		XFreeGC(display, fmGC);
		XMapRaised(display, fmwin);
		XUnmapWindow(display, fmwin);
	}

	if (vidmode && mode_present) {
		XF86VidModeSwitchToMode(display, XDefaultScreen(display), scm);
		vidmode = FALSE;
	}
	fmode = FALSE;
	mode_present = FALSE;

	ccapt = 1;
	if (ioctl(tfd, VIDIOCSWIN, &vwin) < 0) {
		perror("ioctl VIDIOCSWIN");
	}

	if (ioctl(tfd, VIDIOCCAPTURE, &ccapt) < 0) {
		perror("ioctl VIDIOCCAPTURE");
	}
	RedrawWindow();
}

/* GrabImage function */
void
GrabImage(void)
{
}

/* Usage function */
void
Usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "wmtv v%s, Copyright (c) 1999 Wee Liang <wliang@tartarus.uwa.edu.au>\n", VERSION);
	fprintf(stderr, "usage: wmtv [%s]\n", OPTIONS);
	fprintf(stderr, "  -d, --display <host:vs>\tsets display name\n");
	fprintf(stderr, "  -g, --geometry <{+-}XP{+-}YP>\tsets geometry\n");
	fprintf(stderr, "  -b, --bpp\t\t\tdisplay color depth\n");
	fprintf(stderr, "  -e, --exe <filename>\t\tprogram to execute\n");
	fprintf(stderr, "  -v, --version\t\t\tprints version information\n");
	fprintf(stderr, "  -h, --help\t\t\tprints this message\n");
	fprintf(stderr, "\n");
}


/* Version function */
void
Version(void)
{
	fprintf(stderr, "wmtv version %s\n", VERSION);
}
