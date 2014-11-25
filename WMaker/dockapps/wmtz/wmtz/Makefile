CC = gcc
LIBDIR = -L/usr/lib -L/usr/lib/X11
LIBS   = -lXpm -lXext -lX11 -lm
FLAGS = -O2
DESTDIR = /usr
OBJS =	wmtz.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o


.c.o:
	$(CC) -I/usr/include -I/usr/include/X11 $(FLAGS) -c -Wall $< -o $*.o

wmtz: $(OBJS)
	$(CC) $(FLAGS) -o wmtz $^ -lXext $(LIBDIR) $(LIBS)

all:: wmtz

clean::
	for i in $(OBJS) ; do \
		rm -f $$i ; \
	done
	rm -f wmtz
	rm -f *~

install:: wmtz
	cp -f wmtz $(DESTDIR)/bin/
	chmod 755 $(DESTDIR)/bin/wmtz
	chown root:root $(DESTDIR)/bin/wmtz
	cp -f wmtz.1 $(DESTDIR)/share/man/man1
	chmod 644 $(DESTDIR)/share/man/man1/wmtz.1
	chown root:root $(DESTDIR)/share/man/man1/wmtz.1
	cp -f wmtzrc $(HOME)/.wmtzrc
	chmod 600 $(HOME)/.wmtzrc
	cp -f wmtzrc /etc/wmtzrc
	chmod 644 /etc/wmtzrc
	chown root:root /etc/wmtzrc
	@echo "wmtz: Installation finished..."

uninstall::
	rm -i /usr/bin/wmtz
	rm -i /etc/wmtzrc
	chmod 666 $(HOME)/.wmtzrc
	rm -i $(HOME)/.wmtzrc
	@echo "wmtz: Uninstall completed..."
