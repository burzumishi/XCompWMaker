INCDIR	=	-I/usr/include -I/usr/include/X11
LIBDIR	=	-L/usr/lib -L/usr/lib/X11
LIBS	=	-lXpm -lXext -lX11
OBJS	=	wmflame.o ../wmgeneral/wmgeneral.o 

.c.o:
	cc -c -g -O3 -Wall -D$(shell echo `uname -s`) $< -o $*.o $(INCDIR)

all: wmflame

wmflame: $(OBJS)
	cc -o wmflame $^ $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f wmflame

install:
	mv wmflame /usr/bin
