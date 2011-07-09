noinst_PROGRAMS = test_signal
test_signal_SOURCES = test_signal.c

test_signal_LDADD = $(LDADD) $(MPRIS_LIBS)
AM_CFLAGS = $(CFLAGS) $(MPRIS_CFLAGS) -std=c99 -Wall -Werror \
                -I/usr/local/include
ACLOCAL_AMFLAGS = -I m4
