/*
 * wmflame is a windowmaker / afterstep dock/wharf applet for GNU/Linux
 *
 * It draws flames which can be adjusted with little buttons
 *
 * Code based on some of the Window maker programs found on the internet
 *
 * Code based on wmpong, by Dag Wieers <dag@digibel.be>
 *
 * Copyright 1999, Dave Turner (turnerd@reed.edu);
 *
 * This program is distributed under the GPL license. 
 *
 * Thanks to Richard Stallman for the GNU system and the GPL.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <X11/xpm.h>
#include <X11/Xlib.h>


#include "../wmgeneral/wmgeneral.h"

#include "wmflame-master.xpm"

#define USLEEP 20000

#define MODE_NORMAL   0
#define MODE_LOAD_1   1 
#define MODE_LOAD_5   2
#define MODE_LOAD_15  3 
#define MODE_GAS      4
#define MODE_ARBNUM   5


#define VERSION  "0.6"

struct xpm_position {
    int x, y;
  };
struct xpm_surface {
    int x, y, w, h;
  };


struct xpm_position pos[] =
{
  {5, 50},
  {16, 50},
  {27, 50},
  {38, 50},
  {49, 50}
};
struct xpm_surface rel[] = {
  {1, 70, 11, 9},
  {12, 70, 11, 9},
  {23, 70, 11, 9},
  {34, 70, 11, 9},
  {45, 70, 11, 9}
};
struct xpm_surface prs[] = {
  {1, 79, 11, 9},
  {12, 79, 11, 9},
  {23, 79, 11, 9},
  {34, 79, 11, 9},
  {45, 79, 11, 9}
};



char seed_line [64];

char wmflame_pm[64][48];

//imported from wmgeneral

extern GC NormalGC;

int bgr [4][64];

/*****************************************************************************/

void init_bgr () {

  int i;

  for (i = 0; i < 64; i ++) {
      bgr [0][i] = i << 2;
      
  }
  
}
/*****************************************************************************/

void init_seeds () {
  
  int i;

  for (i = 0; i < 64; i ++) {
    seed_line [i] = (rand() % 64);
  }

}

/*****************************************************************************/

void permute_seeds () {

  int i;

  for (i = 0; i < 64; i ++) {
    seed_line [i] = seed_line [i] - 5 + (rand() % 11);
    if (seed_line [i] < 0) seed_line [i] = 0;
    if (seed_line [i] > 70) seed_line [i] = 70;
  }

}

/*****************************************************************************/
void loaded_permute () {

  int i;

  float j;

  int total = 0, rand_amt;

  /* since consistency is now important (it's a load meter),
     total how much is added, and add on extra (or take some away) 
     if necessary */

  for (i = 0; i < 64; i ++) {
    rand_amt =  (rand() % 21);
    total += rand_amt;

    seed_line [i] = seed_line [i] - 10 + rand_amt;
    if (seed_line [i] < 0) seed_line [i] = 0;
    if (seed_line [i] > 70) seed_line [i] = 70;
  }
  
  if (total != 640) {
    if (640 > total) {
      for (j = 0; j < 64; j += 64.0 / (640 - total)) {
	seed_line [(int)j] ++;
      }
    } else {
      for (j = 0; j < 64; j += 64.0 / (total - 640)) {
	seed_line [(int)j] --;
      }
    }
  }
}


/*****************************************************************************/
/* the following function is *broken*.  It was planned as a replacement 
   for do_frame, but doesn't yet work.
*/

void do_frame2 (float divisor, int random_amt) {
  
  int x, y;

  char *pix_last, *pix_cur, *pix_next, *pix_right, *pix_down;

  for (x = 1; x < 63; x ++) {
    wmflame_pm [48][x] = ((seed_line [x - 1] + seed_line [x] + seed_line [x + 1]) / 2);
  }
  
  /* this is where the fun goes on.  We sort of "blur up" the flame.  
     There's no color going on yet, just an array of intensity or heat values.
     each pixel gets an average of the point to its right, itself, and the 
     three below it, minus a random little bit.  The addition of the point to 
     the right of the pixel makes the flame appear to move left as though 
     blown by a breeze.  The divisor is always around 5, which is the number 
     of values being averaged.

     */
  
  pix_cur = (wmflame_pm [47]) + sizeof (char);
  pix_last = wmflame_pm [48];
  pix_down = (wmflame_pm [48]) + sizeof (char);
  pix_next = (wmflame_pm [48]) + sizeof (char) * 2;
  pix_right = (wmflame_pm [47]) + sizeof (char) * 2;
  
  for (y = 47; y > 0; y --) {
    for (x = 1; x < 62; x ++) {

       wmflame_pm [y][x] = ((float) (-(rand() % random_amt) + pix_right [0] + pix_down [0] + pix_last [0] + pix_cur [0] + pix_next [0])) / divisor;

      pix_last ++;
      pix_cur ++;
      pix_next ++;
      pix_right ++;
      pix_down ++;
    }

    pix_last += 2;
    pix_cur += 2;
    pix_next += 2;
    pix_right += 2;
    pix_down += 2;
  }
  
  /* Now clean up.  Set values less than zero to zero, and values greater 
     than 63 to 63. */
  
  for (y = 47; y > 1; y --) {
    for (x = 1; x < 62; x ++) {
      if (wmflame_pm [y][x] > 63) {
	wmflame_pm [y][x] = 63;
      } else {
        if (wmflame_pm [y][x] < 0) {
          wmflame_pm [y][x] = 0;
        }
      }
    }
  }

}
/*****************************************************************************/

void do_frame (float divisor, int random_amt) {

  int x, y;

  for (x = 1; x < 63; x ++) {
    wmflame_pm [48][x] = ((seed_line [x - 1] + seed_line [x] + seed_line [x + 1]) / 2);
  }

  /* this is where the fun goes on.  We sort of "blur up" the flame.  
     There's no color going on yet, just an array of intensity or heat values.
     each pixel gets an average of the point to its right, itself, and the 
     three below it, minus a random little bit.  The addition of the point to 
     the right of the pixel makes the flame appear to move left as though 
     blown by a breeze.  The divisor is always around 5, which is the number 
     of values being averaged.   */


  for (y = 47; y > 0; y --) {
    for (x = 1; x < 62; x ++) {
      wmflame_pm [y][x] = (-(rand() % random_amt) + wmflame_pm [y][x + 1] + wmflame_pm [y][x] + wmflame_pm [y + 1][x - 1] + wmflame_pm [y + 1][x] + wmflame_pm [y + 1][x + 1]) / divisor;
    }
  }

  /* now clean up.  set values less than zero to zero, and values greater 
     than 63 to 63. */

  for (y = 47; y > 1; y --) {
    for (x = 1; x < 62; x ++) {
      if (wmflame_pm [y][x] > 63) {
	wmflame_pm [y][x] = 63;
      } else {
        if (wmflame_pm [y][x] < 0) {
          wmflame_pm [y][x] = 0;
        }
      }
    }
  }

}

/*****************************************************************************/

void loaded_burn (float load) {

  int x, y;

  for (x = 1; x < 63; x ++) {
    wmflame_pm [48][x] = ((seed_line [x - 1] + seed_line [x] + seed_line [x + 1]) / 2);
  }
  
  /* this is where the fun goes on.  We sort of "blur up" the flame.  
     There's no color going on yet, just an array of intensity or heat values.
     each pixel gets an average of the point to its right, itself, and the 
     three below it, minus a random little bit.  The addition of the point to 
     the right of the pixel makes the flame appear to move left as though 
     blown by a breeze.  The divisor is always around 5, which is the number
     of values being averaged.   */

  for (y = 47; y > 0; y --) {
    for (x = 1; x < 62; x ++) {
      wmflame_pm [y][x] = (wmflame_pm [y][x] + wmflame_pm [y + 1][x - 1] + wmflame_pm [y + 1][x] + wmflame_pm [y + 1][x + 1]) / 
     (4.22 - load / 5.5); //4.22 is about 4.
    }
  }

  /* Now clean up.  Set values less than zero to zero, and values greater 
     than 63 to 63. */

  for (y = 47; y > 1; y --) {
    for (x = 1; x < 62; x ++) {
      if (wmflame_pm [y][x] > 63) {
	wmflame_pm [y][x] = 63;
      } else {
        if (wmflame_pm [y][x] < 0) {
          wmflame_pm [y][x] = 0;
        }
      }
    }
  }

}


/*****************************************************************************/

void paint_frame (int cmap, int memory_coloring, int mem_amount) {

  /* I really wish wmgeneral allowed a better way to implement this.
     I have improved speed a little bit by replacing this call to wmgeneral:

        copyXPMArea (wmflame_pm [y][x], 66 + cmap, 1, 1, x, y);
	
     with this call to Xlib:

	XCopyArea(display, wmgen.pixmap, wmgen.pixmap, NormalGC, wmflame_pm [y][x], 66 + cmap, 1, 1, x, y);

     I hope to further improve performance at some point by not copying, but just blitting a pixel like this:

     XSetForeground (display, NormalGC, bgr [0][wmflame_pm[y][x]]);
     XDrawPoint (display, wmgen.pixmap, NormalGC, x, y);

     This seems to provide a ~30% speed improvement.  However, it requires the bgr array to be created.  It also will probably reduce the portability of wmflame. 

  */

  int x, y;

//  if (!memory_coloring) {
    for (y = 47; y > 0; y --) {
      for (x = 1; x < 63; x ++) {
	
	XCopyArea (display, wmgen.pixmap, wmgen.pixmap, NormalGC, 70 + cmap, wmflame_pm [y][x], 1, 1, x, y);
      }
    }
    /*
  } else {
    for (y = 47; y > 0; y --) {
      for (x = 1; x < 63; x ++) {
	
	XCopyArea (display, wmgen.pixmap, wmgen.pixmap, NormalGC, wmflame_pm [y][x] + 10, 90 + mem_amount , 1, 1, x, y);
      }
    }
  }
  */
}

/*****************************************************************************/

float get_load_avg (int which) {

  FILE *fp;
  float ftmp = 0;
  int i;
  static float lastload;

  if ((fp = fopen ("/proc/loadavg", "r")) != NULL) {
    for (i = 0; i < which; i ++) {
      fscanf(fp, "%f", &ftmp);
    }
    lastload = ftmp;
    fclose (fp);
  } else {
    return lastload;
  }


  return ftmp;
}
/*****************************************************************************/

float get_arb_num (char *arbfile, char *arbscanstr) {

  FILE *fp;
  float ftmp = 0;
  static float lastnum;

  if ((fp = fopen (arbfile, "r")) != NULL) {
    fscanf(fp, arbscanstr, &ftmp);

    lastnum = ftmp;
    fclose (fp);
  } else {
    return lastnum;
  }


  return ftmp;
}

/*****************************************************************************/

int get_mem_amt () {

  
  FILE *fp;
  int total, freemem, cachemem;
  static int lastmem;
  
  if ((fp = fopen ("/proc/meminfo", "r")) != NULL) {
    
    fscanf (fp, " %*s %*s %*s %*s %*s %*s %*s %i %*i %i %*i %*i %i", &total, &freemem, &cachemem);

    lastmem = (int)(((freemem + cachemem) * 64.0) / (float) total);
    //printf ("%i %i %i %i\n", total, freemem, cachemem, last_mem);
    fclose (fp);
  }

  return lastmem;
}

/*****************************************************************************/

void print_usage () {

  printf ("\nWmFlame %s  \nBy Dave Turner (turnerd@reed.edu)\n", VERSION);
  printf ("Usage:\n\n");
  printf ("      -h        This screen\n");
  printf ("      -lN       Use as load meter.\n");
  printf ("        N = 1:   Load average over 1 minute\n");
  printf ("        N = 2:   Load average over 5 minutes\n");
  printf ("        N = 3:   Load average over 15 minutes\n\n");
  printf ("      -v        Print version number\n");
  printf ("      -m N      Load max is N.  You usually want N to be equal to the number\n");
  printf ("      -n N      Load min is N.  This is usually 0.\n");
  printf ("                of CPUs you have.\n\n");
  printf ("      -b        Run 1000 frames, then quit.\n");
  printf ("      -g        Gas flame mode.  Try it.\n");
  printf ("      -a X Y    Gets numbers from an arbitrary file X, using scanf string Y.\n");
  printf ("      -s N      Scan frequency.  In -l and -a modes, the file is checked every \n  ");
  printf ("                N frames.\n\n");
}


/*****************************************************************************/

int main (int argc, char *argv[]) {
  XEvent Event;

  char wmflame_mask_bits [64 * 64];
  int wmflame_mask_width = 64;
  int wmflame_mask_height = 64;

  int random_amt = 22;   // these two values were obtained through trial
  float divisor = 4.79;  // and error.  Change them here, or with the 
                         // little buttons.;			      

  int gas_timer = 0;

  int cmap = 2;

  int mode = MODE_NORMAL; 
  int memory_coloring = 0;
  int mem_amount = 0;
  char *arbfile = NULL, *arbscanstr = NULL;
  float maximum = 1, minimum = 0; /*if it's a load meter, this determines 
				    min and max loads */
  int scanfreq = 1;

  float load = 0;
  int benchmarking = -666;
  int paused = 0;  /* if non-zero, doesn't draw anything */

  int curframe = 0;
  int i;
  char *arg = NULL;

  /*
  This function only needs to be called if we're using the XDrawPoint optimization. As this isn't complete, don't do it.

  init_bgr ();
  */


  for (i = 1; i < argc; i ++) {
    arg = argv[i];
    
    if (*arg=='-') {
      switch (arg[1]) {

      case 'v' :
	printf ("%s\n", VERSION);
	exit (0);
        break;

      case 'l' :
	
	switch (arg[2]) {
	case '1':
	  mode = MODE_LOAD_1;
	  break;
	case '2':
	  mode = MODE_LOAD_5;
	  break;
	case '3':
	  mode = MODE_LOAD_15;
	  break;
	default:
	  mode = MODE_LOAD_1;
	  break;
	}
	break;

      case 'b':	
	benchmarking = 1000;
	break;

      case 'm':
	
	maximum = atof (argv [++i]);
	break;

      case 'n':
	
	minimum = atof (argv [++i]);
	break;

      case 's':
	
        scanfreq = atoi (argv [++i]);
	break;

      case 'g':
	mode = MODE_GAS;
	break;

      //does not work, because Xlib or wmgeneral or libXPM is on crack.
      case 'c':
	memory_coloring = 1;
	break;

      case 'a':
	mode = MODE_ARBNUM;
	arbfile = strdup (argv [++i]);
	arbscanstr = strdup (argv [++i]);
	break;

      default:
	print_usage();
	exit (0);
	break;
      }
    }
  }



  srand (time (NULL));

  init_seeds ();

  createXBMfromXPM (wmflame_mask_bits, wmflame_master_xpm, wmflame_mask_width, wmflame_mask_height);

  openXwindow (argc, argv, wmflame_master_xpm, wmflame_mask_bits, wmflame_mask_width, wmflame_mask_height);
  
  for (i = 0; i < 5; i++)
    AddMouseRegion (i, pos[i].x, pos[i].y, pos[i].x + prs[i].w, pos[i].y + prs[i].h);
  
  AddMouseRegion (5, 0, 0, 64, 47);

  while (benchmarking == -666 || (benchmarking > 0)) {

    if (benchmarking > 0) benchmarking --;

    if ( !paused ) {


      switch (mode) {
	
      case MODE_NORMAL:
        permute_seeds ();
	do_frame (divisor, random_amt);

	break;

      case MODE_LOAD_1:
      case MODE_LOAD_5:
      case MODE_LOAD_15:
	if (++curframe >= scanfreq) {
	  load = get_load_avg (mode);
	  if (load > maximum) load = maximum;
	  if (load < minimum) load = minimum;
	  curframe = 0;
	}
	loaded_permute ();
	loaded_burn (load / maximum);
	break;
	break;
	break;

      case MODE_ARBNUM:
	if (++curframe >= scanfreq) {
	  load = get_arb_num (arbfile, arbscanstr);
	  if (load > maximum) load = maximum;
	  if (load < minimum) load = minimum;
	  curframe = 0;
	}

	loaded_permute ();
	loaded_burn (load / maximum);
	break;       
      case MODE_GAS:

        permute_seeds ();

	gas_timer --;

	if (gas_timer == 0) {

	  do_frame (divisor - 0.15, random_amt);

	} else {
	  if (gas_timer < 0) {

	    gas_timer = rand () % 50 + 5;
	    do_frame (divisor - 0.3, random_amt);
	  }

	  do_frame (divisor, random_amt);
	}

	break;
      }

      if (memory_coloring) mem_amount = get_mem_amt ();
      paint_frame (cmap, memory_coloring, mem_amount);
      RedrawWindow ();    
    } else {
      usleep (USLEEP);
    }

    while (XPending (display)) {
      XNextEvent (display, &Event);
      switch (Event.type) {

      case Expose:
	RedrawWindow ();
	break;
      case DestroyNotify:
	XCloseDisplay (display);
	exit (0);
	break;
      case ButtonPress:
        if ( Event.xbutton.button == 2 ) {
          paused = !paused;
          break;
        }

	i = CheckMouseRegion (Event.xbutton.x, Event.xbutton.y);
	if (i < 5) copyXPMArea (prs[i].x, prs[i].y, prs[i].w, prs[i].h, pos[i].x, pos[i].y);
        switch (i) {
	case 0:
	  divisor -= .01;
	  if (divisor <= 0) divisor = .01;
	  break;
	case 1:
	  divisor += .01;
	  break;
	case 2:
	  random_amt += 1;
	  break;
	case 3:
	  random_amt -= 1;
	  if (random_amt < 1) random_amt = 1;
	  break;
	case 4:
	  init_seeds ();
	  random_amt = 22; 
	  divisor = 4.79;
	  break;
	  
	case 5:
	  
	  cmap = (cmap + 1) % 10; 
	  break;
	}

	break;

      case ButtonRelease:
	i = CheckMouseRegion (Event.xbutton.x, Event.xbutton.y);
	if (i < 5) copyXPMArea (rel[i].x, rel[i].y, rel[i].w, rel[i].h, pos[i].x, pos[i].y);
	break;
      }
    }
    usleep (USLEEP);
  }

  return 0;

}

