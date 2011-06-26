lib_LTLIBRARIES = mpris.la
mpris_la_SOURCES = mpris.c mpris_v1.h mpris_v1.c introspection_xml.h\
				   mpris_common.h mpris_common.c deadbeef.h
mpris_la_LDFLAGS = -module

mpris_la_LIBADD = $(LDADD) $(MPRIS_LIBS)
AM_CFLAGS = $(CFLAGS) $(MPRIS_CFLAGS) -std=c99 -Wall -Werror \
				-I/usr/local/include
ACLOCAL_AMFLAGS = -I m4
