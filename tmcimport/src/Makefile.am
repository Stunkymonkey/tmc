bin_PROGRAMS = tmcimport
tmcimport_SOURCES = tmcimport.cpp tmcqueryhandler.cpp tmcioptions.cpp tmcreader.cpp tmcfilter.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
tmcimport_LDFLAGS = $(all_libraries)

tmcimport_LDADD = -lrds -lboost_iostreams -L../../rds/librds/src/.libs/
noinst_HEADERS = tmcqueryhandler.h tmcioptions.h tmcreader.h tmcfilter.h
