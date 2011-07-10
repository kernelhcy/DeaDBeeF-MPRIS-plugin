#!/bin/bash
make distclean 
rm -rf autom4te.cache\
       aclocal.m4\
       test/.deps\
       configure\
       Makefile.in\
       config.h.in\
       test/Makefile.in
