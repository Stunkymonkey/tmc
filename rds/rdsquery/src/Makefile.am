bin_PROGRAMS = rdsquery
rdsquery_SOURCES = rdsquery.cpp rdsqueryhandler.cpp rdsqoptions.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
rdsquery_LDFLAGS = $(all_libraries)




rdsquery_LDADD = -lrds
noinst_HEADERS = rdsqueryhandler.h rdsqoptions.h
