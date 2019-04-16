
ACLOCAL_AMFLAGS = -I build-aux/m4

bin_PROGRAMS =
CLEANFILES =
include_HEADERS =
lib_LTLIBRARIES =

AM_CPPFLAGS = ${local_cppflags}
AM_LDFLAGS = ${local_ldflags} -L./.libs

if DEBUG
AM_CFLAGS = -DDEBUG -O0 -ggdb
else
AM_CFLAGS = -DNDEBUG -O2
endif

lib_LTLIBRARIES += libskiplist.la
libskiplist_la_SOURCES = \
	skiplist.c
include_HEADERS += \
	skiplist.h
