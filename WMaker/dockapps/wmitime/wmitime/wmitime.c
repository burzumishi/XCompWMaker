/*

 wmitime.c -- internet time clock

 by Dave Clark (clarkd@skynet.ca) (http://www.neotokyo.org/illusion)

 This software is licensed through the GNU General Public Lisence.

*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>

#include "../wmgeneral/wmgeneral.h"
#include "../wmgeneral/misc.h"

#ifdef fr_FR
#define fr
#endif

#ifdef fr
#include "french.h"
#else
#include "language.h"
#endif

#include "wmitime-master.xpm"
char wmitime_mask_bits[64*64];
int  wmitime_mask_width = 64;
int  wmitime_mask_height = 64;

#define WMITIME_VERSION "0.1"

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 7

#define BCHAR_WIDTH 6
#define BCHAR_HEIGHT 9

#define MY_PI (3.14159)

extern	char **environ;

char	*ProgName;

char uconfig_file[256];

time_t		curtime;
time_t		prevtime;

int prevhourx=19;
int prevhoury=33;
int prevminx=19;
int prevminy=33;

static struct tm *clk;

int TwelveHour=0;

void usage(void);
void printversion(void);
void BlitString(char *name, int x, int y);
void BlitNum(int num, int x, int y);
void wmitime_routine(int, char **);
int PortWatch( short port );
int ReadConfigInt(FILE *fp, char *setting, int *value);
int ReadConfigString(FILE *fp, char *setting, char *value);
int Read_Config_File( char *filename );
void DrawInetTime(void);
void DrawStdTime(void);
void DrawDate(void);
void DrawInetWheel(void);
void DrawStdWheel(void);
void DrawLine(int x1, int y1, int x2, int y2, int sourcex, int sourcey);

int main(int argc, char *argv[]) {

	int		i;

    uconfig_file[0] = 0;

	/* Parse Command Line */

	ProgName = argv[0];
	if (strlen(ProgName) >= 5)
		ProgName += (strlen(ProgName) - 5);

	for (i=1; i<argc; i++) {
		char *arg = argv[i];

		if (*arg=='-') {
			switch (arg[1]) {
			case 'd' :
				if (strcmp(arg+1, "display")) {
					usage();
					exit(1);
				}
				break;
			case 'g' :
				if (strcmp(arg+1, "geometry")) {
					usage();
					exit(1);
				}
				break;
			case 'v' :
				printversion();
				exit(0);
				break;
            case 'c' :
                if (argc > (i+1))
                {
                    strcpy(uconfig_file, argv[i+1]);
                    i++;
                }
                break;
            case '1' :
                if (arg[2] == '2')
                {
                    // Twelve-hour mode
                    TwelveHour = 1;
                }
                break;
            default:
				usage();
				exit(0);
				break;
			}
		}
	}

	wmitime_routine(argc, argv);

	return 0;
}

/*******************************************************************************\
|* wmitime_routine														   *|
\*******************************************************************************/

void wmitime_routine(int argc, char **argv)
{
    int			i;
	XEvent		Event;
	int			but_stat = -1;


//    char config_file[512];

    createXBMfromXPM(wmitime_mask_bits, wmitime_master_xpm, wmitime_mask_width, wmitime_mask_height);

	openXwindow(argc, argv, wmitime_master_xpm, wmitime_mask_bits, wmitime_mask_width, wmitime_mask_height);

	AddMouseRegion(0, 5, 6, 58, 16);

    // We don't need a config file (yet)...

#if 0
    // Read config file

    if (uconfig_file[0] != 0)
    {
        // user-specified config file
        fprintf(stderr, "Using user-specified config file '%s'.\n", uconfig_file);
        Read_Config_File(uconfig_file);
    }
    else
    {
        sprintf(config_file, "%s/.wmitimerc", getenv("HOME"));

        if (!Read_Config_File(config_file))
        {
            // Fall back to /etc/wminetrc
            sprintf(config_file, "/etc/wmitimerc");

            Read_Config_File(config_file);
        }
    }
#endif

    RedrawWindow();

    prevtime = time(0) - 1;

    while (1)
    {
		curtime = time(0);

        if ( curtime > prevtime)
        {
            prevtime = curtime;

            clk = localtime(&curtime);

            // Update display

            DrawInetTime();

            DrawStdTime();

            DrawInetWheel();

            DrawDate();

            DrawStdWheel();

            RedrawWindow();

        }

        // X Events
        while (XPending(display))
        {
			XNextEvent(display, &Event);
            switch (Event.type)
            {
			case Expose:
				RedrawWindow();
				break;
			case DestroyNotify:
				XCloseDisplay(display);
				exit(0);
                break;
			case ButtonPress:
				i = CheckMouseRegion(Event.xbutton.x, Event.xbutton.y);

				but_stat = i;
				break;
			case ButtonRelease:
				i = CheckMouseRegion(Event.xbutton.x, Event.xbutton.y);

                if (but_stat == i && but_stat >= 0)
                {
                    switch (but_stat)
                    {
                    case 0 :
                        TwelveHour = (!TwelveHour);
                        prevtime--;
                        break;
                    case 1 :
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;

					}
				}
				but_stat = -1;
//				RedrawWindow();
				break;
			}
		}

		usleep(100000);
	}
}


void DrawInetTime(void)
{
    int iTime;

    // Compute Inet Time
    iTime=(clk->tm_hour*3600+clk->tm_min*60+clk->tm_sec);
    iTime=iTime+((timezone-1)+3600);
    if (clk->tm_isdst)
        iTime-=3600;
    iTime=(iTime*1000)/86400;

    if (iTime >= 1000)
        iTime-=1000;
    else
        if (iTime < 0)
            iTime+=1000;

    // Blit it

    BlitNum(iTime, 38, 18);

}

void DrawStdTime(void)
{
    int xoff=0, yoff=0;
    int srcx=0, srcy=0;
    int i,j;
    char blitstr[32];
    int len;

    i = clk->tm_hour;

    if (TwelveHour)
    {
        if (i > 12)
            i-=12;

        if (i==0)
            i=12;

        sprintf(blitstr, "%2i:%02i:%02i", i, clk->tm_min, clk->tm_sec);
    }
    else
    {
        sprintf(blitstr, "%02i:%02i:%02i", i, clk->tm_min, clk->tm_sec);
    }



    len = strlen(blitstr);

    // Set starting co-ordinates...
    xoff = 6;
    yoff = 6;

    // Blit it.
    for( i=0;  i<len; i++)
    {
        if (blitstr[i] == ':')
        {
            copyXPMArea(138, 23, 4, BCHAR_HEIGHT, xoff, yoff);
            xoff+=4;
        }
        else if (isdigit(blitstr[i]))
        {
            j = blitstr[i] - '0';
            srcx = 68;
            srcy = 23;

            while (j)
            {
                j--;
                srcx += (BCHAR_WIDTH + 1);
            }

            copyXPMArea(srcx, srcy, BCHAR_WIDTH+1, BCHAR_HEIGHT, xoff, yoff);
            xoff+=(BCHAR_WIDTH + 1);
        }
        else
        {
            copyXPMArea(66, 10, BCHAR_WIDTH+1, BCHAR_HEIGHT, xoff, yoff);
            xoff+=(BCHAR_WIDTH + 1);
        }
    }

}

void DrawDate(void)
{
    char BlitStr[20];

    sprintf(BlitStr, "%s", daynames[clk->tm_wday]);
    BlitString( BlitStr, 6, 50);

#ifdef fr

    // French date model
    sprintf(BlitStr, "%s", monthnames[clk->tm_mon]);
    BlitString( BlitStr, 40, 50);

    sprintf(BlitStr, "%02i", clk->tm_mday);
    BlitString( BlitStr, 25, 50);
#else

    sprintf(BlitStr, "%s", monthnames[clk->tm_mon]);
    BlitString( BlitStr, 25, 50);

    sprintf(BlitStr, "%02i", clk->tm_mday);
    BlitString( BlitStr, 45, 50);

#endif
}

void DrawInetWheel(void)
{
    int WheelPos=0;
    int i;
    int xoff=0, yoff=0;
    int iTime;

    // Calculate Wheel Position...
    iTime=(clk->tm_hour*3600+clk->tm_min*60+clk->tm_sec);
    iTime=iTime+((timezone-1)+3600);
    if (clk->tm_isdst)
        iTime-=3600;
    iTime=(iTime*1000)/8640;

    if (iTime >= 10000)
        iTime-=10000;
    else
        if (iTime < 0)
            iTime+=10000;

    iTime %= 10;

    WheelPos = floor( (iTime *8) / 10);

    // Draw the Wheel...
    i=WheelPos;
    yoff=35;
    xoff=67;

    xoff+=19;

    while(i)
    {
        xoff +=19;
        i--;
    }

    copyXPMArea(xoff, yoff, 19, 19, 39, 29);

}


void DrawStdWheel(void)
{
    //19x33 = center
    //radius of 14

    int sx, sy;
    int cx, cy;
    int dx, dy;
    int hr;
    double psi;

    cx=19;
    cy=33;

    sx = 2;
    sy = 97;

    // Hour Hand...

    DrawLine(cx, cy, prevhourx, prevhoury, 66, 9); // erase old line

    hr = (clk->tm_hour % 12);

    psi = hr * (M_PI / 6.0);
	psi += clk->tm_min * (M_PI / 360);

    dx = floor(sin(psi) * 22 * 0.5 + 0.5);
    dy = floor(-cos(psi) * 16 * 0.5 + 0.5);

    dx += cx;
    dy += cy;

    prevhourx=dx;
    prevhoury=dy;

    DrawLine(cx, cy, dx, dy, sx, sy);

    // Minute Hand...

    DrawLine(cx, cy, prevminx, prevminy, 66, 9); // erase old line

    cx=19;
    cy=33;
    sx = 2;
    sy = 96;

    psi = clk->tm_min * (M_PI / 30.0);
	psi += clk->tm_sec * (M_PI / 1800);

    dx = floor(sin(psi) * 22 * 0.7 + 0.5);
	dy = floor(-cos(psi) * 16 * 0.7 + 0.5);

    dx += cx;
    dy += cy;

    prevminx = dx;
    prevminy = dy;


    DrawLine(cx, cy, dx, dy, sx, sy);
}

void DrawLine(int x1, int y1, int x2, int y2, int sourcex, int sourcey)
{
    int x, y;
    int deltax, deltay;
    int xs, ys;

    float xd=0, yd=0;
    float xi, yi;

    x = x1;
    y = y1;


    if ( (x2-x1) < 0)
        xs = -1;
    else
        xs = 1;

    if ( (y2-y1) < 0)
        ys = -1;
    else
        ys = 1;

    deltax = abs( x2 - x1 );
    deltay = abs( y2 - y1 );

    if (deltay !=0)
        xi = (float) ((float)deltax / (float) deltay);
    else
        xi=0;

    if (deltax !=0)
        yi = (float) ((float)deltay / (float) deltax);
    else
        yi=0;

    if ( deltax > deltay )
    {
        for (x=x1; x!= x2; x+= xs)
        {
            yd += yi;
            y += (int) (yd * ys);

            copyXPMArea(sourcex, sourcey, 1, 1, x, y);

            yd -= (int) yd;
        }
    }
    else
    {
        for (y=y1; y!= y2; y+= ys)
        {
            xd += xi;
            x += (int) (xd * xs);

            copyXPMArea(sourcex, sourcey, 1, 1, x, y);

            xd -= (int) xd;
        }
    }
}



// Blits a string at given co-ordinates
void BlitString(char *name, int x, int y)
{
    int		i;
	int		c;
    int		k;

	k = x;
    for (i=0; name[i]; i++)
    {

        c = toupper(name[i]);
        if (c >= 'A' && c <= 'Z')
        {   // its a letter
			c -= 'A';
			copyXPMArea(c * 6, 74, 6, 8, k, y);
			k += 6;
        }
        else
        {   // its a number or symbol
			c -= '0';
			copyXPMArea(c * 6, 64, 6, 8, k, y);
			k += 6;
		}
	}

}

void BlitNum(int num, int x, int y)
{
    char buf[1024];
    int newx=x;

    sprintf(buf, "%03i", num);

    BlitString(buf, newx, y);
}


// ReadConfigSetting
int ReadConfigString(FILE *fp, char *setting, char *value)
{
    char str[1024];
    char buf[1024];
    int i;
    int len;
    int slen;
    char *p=NULL;


    if (!fp)
    {
        return 0;
    }

    sprintf(str, "%s=", setting);
    slen = strlen(str);

    fseek(fp, 0, SEEK_SET);

    while ( !feof(fp) )
    {

        if (!fgets(buf, 512, fp))
            break;

        len = strlen(buf);

        // strip linefeed
        for (i=0; i!=len; i++)
        {
            if (buf[i] == '\n')
            {
                buf[i] = 0;
            }
        }

        //printf("Scanning '%s'...\n", buf);
        if ( strncmp(buf, str, strlen(str)) == 0)
        {
            // found our setting

            for(i=0; i!=slen; i++)
            {
                if ( buf[i] == '=' )
                {
                    p=buf+i+1;
                    strcpy(value, p);
                    return 1;
                }
            }

        }
    }

        return 0;
}

int ReadConfigInt(FILE *fp, char *setting, int *value)
{
    char buf[1024];

    if (ReadConfigString(fp, setting, (char *) &buf))
    {
        *value = atoi(buf);
        return 1;
    }

    return 0;
}

int Read_Config_File( char *filename )
{
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp)
    {

        fclose(fp);
        return 1;
    }
    else
    {
        perror("Read_Config_File");
        fprintf(stderr, "Unable to open %s, no settings read.\n", filename);
        return 0;
    }

}







/*******************************************************************************\
|* usage																	   *|
\*******************************************************************************/

void usage(void)
{
    fprintf(stderr, "\nWMiTIME - illusion <clarkd@skynet.ca>  http://www.neotokyo.org/illusion\n\n");
	fprintf(stderr, "usage:\n");
    fprintf(stderr, "    -12                       12-hour mode\n");
	fprintf(stderr, "    -display <display name>\n");
	fprintf(stderr, "    -geometry +XPOS+YPOS      initial window position\n");
//    fprintf(stderr, "    -c <filename>             use specified config file\n");
    fprintf(stderr, "    -h                        this help screen\n");
	fprintf(stderr, "    -v                        print the version number\n");
    fprintf(stderr, "\n");
}

/*******************************************************************************\
|* printversion																   *|
\*******************************************************************************/

void printversion(void)
{
	fprintf(stderr, "wmitime v%s\n", WMITIME_VERSION);
}
