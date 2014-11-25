#
# Temperature.app Makefile
#

DESTDIR =
GNUSTEP_BINDIR = /usr/GNUstep/Apps/Temperature.app
X11_BINDIR = /usr/bin

CXX=c++
CXXFLAGS += -Wall -pedantic -fno-rtti -fno-exceptions -O2 -I/usr/include/X11
LDFLAGS += -L/usr/lib -L/usr/lib/X11 -lXpm -lXext -lX11

OBJECTS = Main.o Temperature.o Xpm.o

all: Temperature.app

Temperature.app: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

.PHONY:	install install-gnustep install-x11 clean

install: install-gnustep

install-gnustep: all
	install -d  $(DESTDIR)$(GNUSTEP_BINDIR)
	install -m 0755 Temperature.app $(DESTDIR)$(GNUSTEP_BINDIR)/Temperature

install-x11: all
	install -d  $(DESTDIR)$(X11_BINDIR)
	install -m 0755 Temperature.app $(DESTDIR)$(X11_BINDIR)/Temperature.app

clean:
	rm -f *~ *.o

# End of file
