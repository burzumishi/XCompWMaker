
DESTDIR =
prefix =/usr
bindir=${prefix}/bin
datarootdir=${prefix}/share
mandir=${datarootdir}/man
man1dir=${mandir}/man1
CONF=/etc

CC     = gcc
LIBDIR = -L/usr/lib -L/usr/lib/X11
LIBS   = -lXpm -lXext -lX11
OBJS =	wmifs.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o

CFLAGS = -Wall -O2 -g

INSTALL = /usr/bin/install
INSTALL_DIR     = $(INSTALL) -p -d -o root -g root -m 755
INSTALL_PROGRAM = $(INSTALL) -p -o root -g root -m 755
INSTALL_FILE    = $(INSTALL) -p -o root -g root -m 644

.c.o:
	$(CC) -c $(CFLAGS) $< -o $*.o

wmifs: $(OBJS)
	$(CC) -o wmifs $^ -lXext $(LIBDIR) $(LIBS)

all:: wmifs

clean::
	for i in $(OBJS) ; do \
		rm -f $$i ; \
	done
	rm -f wmifs

install::
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_DIR) $(DESTDIR)$(CONF)
	$(INSTALL_DIR) $(DESTDIR)$(man1dir)
	$(INSTALL_PROGRAM) wmifs $(DESTDIR)$(bindir)
	$(INSTALL_FILE) sample.wmifsrc $(DESTDIR)$(CONF)/wmifsrc
	$(INSTALL_FILE) wmifs.1 $(DESTDIR)$(man1dir)
	@echo "WMiFS-1.3beta installation finished..."
	@echo " "
	@echo "have fun! ;-)"
