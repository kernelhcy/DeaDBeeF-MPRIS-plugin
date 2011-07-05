pkglib_LTLIBRARIES = mpris.la
mpris_la_SOURCES = mpris.c \
                   mpris_v1.h mpris_v1.c \
                   mpris_v2.h mpris_v2.c \
                   introspection_xml.h introspection_xml.c \
                   mpris_common.h mpris_common.c

mpris_la_LDFLAGS = -module

mpris_la_LIBADD = $(LDADD) $(MPRIS_LIBS)
AM_CFLAGS = $(CFLAGS) $(MPRIS_CFLAGS) -std=c99 -Wall -Werror \
                -I/usr/local/include
ACLOCAL_AMFLAGS = -I m4
