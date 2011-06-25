#!/bin/bash
make distclean 
rm -rf configure\
       Makefile.in\
       config.h.in\
       autom4te.cache\
       aclocal.m4\
       m4/ltsugar.m4\
       m4/libtool.m4\
       m4/ltversion.m4\
       m4/lt~obsolete.m4\
       m4/ltoptions.m4\
       build-aux

