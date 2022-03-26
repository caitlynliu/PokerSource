
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include

EXTRA_DIST = getopt_w32.c getopt_w32.h $(wildcard *.vcproj) 

noinst_PROGRAMS = \
	test \
	hvh

test_SOURCES = test.c
test_LDADD = $(top_builddir)/lib/libpoker-eval.la
test_CPPFLAGS = ${GCOV_CPPFLAGS}
test_LDFLAGS = ${GCOV_LDFLAGS}

hvh_SOURCES = hvh.c
hvh_LDADD = $(top_builddir)/lib/libpoker-eval.la
hvh_CPPFLAGS = ${GCOV_CPPFLAGS}
hvh_LDFLAGS = ${GCOV_LDFLAGS}


clean-local:
	-rm -f *.gcov *.gcda *.gcno
