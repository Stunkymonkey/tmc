bin_PROGRAMS = rdslog
rdslog_SOURCES = rdslog.cpp rdsqueryhandler.cpp rdsloptions.cpp rdswriter.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes)

# the library search path.
rdslog_LDFLAGS = $(all_libraries)

rdslog_LDADD = -lrds -L../../rds/librds/src/.libs/
noinst_HEADERS = rdsqueryhandler.h rdsloptions.h rdswriter.h
