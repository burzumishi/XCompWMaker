CC=gcc
CFLAGS= -I/usr/include -I/usr/include/X11 -g
OBJS=wmmixer-alsa.o mixer.o

all:	${OBJS}
	${CC} -L/usr/lib -L/usr/lib/X11 -g -lm -lXpm -lXext -lX11 -lasound ${OBJS} -o wmmixer-alsa

clean:
	rm -rf *.o *~ wmmixer-alsa
