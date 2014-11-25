AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = b0rken m4

bin_PROGRAMS = wmsystemtray

wmsystemtray_SOURCES = fdtray.c fdtray.h wmsystemtray.c wmsystemtray.h wmsystemtray.xpm
wmsystemtray_CFLAGS = @X_CFLAGS@
wmsystemtray_LDADD = b0rken/libb0rken.a @X_LIBS@

man_MANS = wmsystemtray.1
EXTRA_DIST = bootstrap $(man_MANS)

reallyclean: distclean
	-rm -rf autom4te.cache
	-rm Makefile.in */Makefile.in aclocal.m4 config.h.in* config.guess config.sub configure install-sh ltmain.sh missing mkinstalldirs stamp-h.in
