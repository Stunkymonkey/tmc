bin_PROGRAMS = tmcimport
tmcimport_SOURCES = tmcimport.cpp tmcqueryhandler.cpp tmcioptions.cpp tmcreader.cpp tmcdata.cpp tmcpsql.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes)

# the library search path.
tmcimport_LDFLAGS = $(all_libraries)

tmcimport_LDADD = -lrds -lboost_iostreams -lpqxx -lpq -L../../rds/librds/src/.libs/
noinst_HEADERS = tmcqueryhandler.h tmcioptions.h tmcreader.h tmcdata.h tmcpsql.h
