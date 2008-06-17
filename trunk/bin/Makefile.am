bin_PROGRAMS = sendMail
EXTRA_DIST = config.txt tmp
LIBNETWORK_LDADDS = $(top_builddir)/lib/libnetwork.a 
AM_CPPFLAGS = -I$(top_srcdir)/include \
	      -I$(top_builddir)/include \
	      -Wall -O3
sendMail_LDFLAGS = @CFLAGS@ -O3 -Wall
sendMail_SOURCES = $(top_builddir)/src/sendMail.c
sendMail_LDADD = $(LIBNETWORK_LDADDS)
