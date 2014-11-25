XCompWMaker
===========

__Window Maker with Composition Effects__

# Instructions

## 1. Install Window Maker

Install latest version of __Window Maker__ (Window Manager).

You can install it from your distro package manager or build it from this sources (recommended).

### Window Maker

__Window Maker__ is the GNU window manager for the X Window System. It was designed to emulate the look and feel of part of the NEXTSTEP(tm) GUI. It's supposed to be relatively fast and small, feature rich, easy to configure and easy to use, with a simple and elegant appearance borrowed from NEXTSTEP(tm). Window Maker was designed keeping integration with GNUstep in mind and is the "official" window manager for it. It is also part of the GNU project (www.gnu.org) Read more about GNUstep further on this file.

## 2. Install Composite Manager

Then choose a composite manager to install (you can install all of them):

### xcompmgr (Xorg)

__xcompmgr__ is a sample compositing manager for X servers supporting the XFIXES, DAMAGE, RENDER, and COMPOSITE extensions.  It enables basic eye-candy effects.

### Compton (xcompmgr fork)

__Compton__ is a compositor for X, and a fork of __xcompmgr-dana__.

Compton was forked from Dana Jansens' fork of xcompmgr and refactored.  

### Cairo Composite Manager

__Cairo Composite Manager__ is a versatile and extensible composite manager which use cairo for rendering. Plugins can be used to add some cool effects to your desktop.

## 3. Desktop Setup

Edit the file __"~/GNUstep/Library/WindowMaker/autostart"__, and uncomment a composite manager.

```
# xcompmgr -d "$DISPLAY" -f -F -I -O -c -r3 -o.60 -l0 -t0 -D3 &

compton -d "$DISPLAY" \
	-r 6 -o .90 -l -3 -t -3 -I -O -D 5 -e 1.0 -i 0.8 -c \
	--dbus --vsync opengl --xrender-sync \
	--unredir-if-possible --glx-no-stencil \
	--detect-transient --sw-opti --paint-on-overlay \
	--detect-rounded-corners &

# compton -d "$DISPLAY" -r 12 -o .60 -l -5 -t -5 -I -O -D 5 -e 1.0 -i 0.8 -c --dbus &
```

Edit the file __"~/GNUstep/Library/WindowMaker/autoexit"__, and uncomment the same composite manager as before.

Start or restart __WindowMaker__, now you can setup your desktop!

Have fun!
