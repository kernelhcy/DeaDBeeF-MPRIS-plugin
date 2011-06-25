lib_LTLIBRARIES = mpris.la
mpris_la_SOURCES = mpris.c mpris_v1.h mpris_v1.c introspection_xml.h\
				   mpris_common.h mpris_common.c deadbeef.h
mpris_la_LDFLAGS = -module
mpris_la_LIBADD = $(MPRIS_LIBS)
mpris_la_CFLAGS = $(CFLAGS) $(MPRIS_CFLAGS) -std=c99 -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
