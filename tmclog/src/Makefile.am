bin_PROGRAMS = tmclog
tmclog_SOURCES = tmclog.cpp tmcqueryhandler.cpp tmcloptions.cpp tmcwriter.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes)

# the library search path.
tmclog_LDFLAGS = $(all_libraries)

tmclog_LDADD = -lrds -lboost_{iostreams,program_options} -L../../rds/librds/src/.libs/
noinst_HEADERS = tmcqueryhandler.h tmcloptions.h tmcwriter.h
