INC	=	-I/usr/include -I/usr/include/X11
LIBS	=	-lXext -lX11 -lm -L/usr/lib -L/usr/lib/X11 \
		-ltiff  -lXpm -lpng -lz -ljpeg -lungif -lwraster
OBJS	=	src/wmglobe.o src/rend.o src/wmgutil.o \
		src/sunpos.o src/myconvert.o src/mycontext.o 


.c.o :
	gcc -c -O2 -Wall $(INC) $< -o $*.o

all:  wmglobe

clean:
	rm -f src/*.o wmglobe
	
wmglobe :	$(OBJS)
		gcc -O2 -Wall $(OBJS) -o wmglobe $(LIBS)
		strip wmglobe 

install :	
		install wmglobe /usr/bin
		install wmglobe.1 /usr/share/man/man1
