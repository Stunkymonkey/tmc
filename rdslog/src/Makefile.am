bin_PROGRAMS = rdslog
rdslog_SOURCES = rdslog.cpp rdsqueryhandler.cpp rdsloptions.cpp rdswriter.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
rdslog_LDFLAGS = $(all_libraries)

rdslog_LDADD = -lrds
noinst_HEADERS = rdsqueryhandler.h rdsloptions.h rdswriter.h
