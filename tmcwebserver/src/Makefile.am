bin_PROGRAMS = tmcwebserver
tmcwebserver_SOURCES = tmcwebserver.cpp tmcjson.cpp tmcpsql.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
tmcwebserver_LDFLAGS = $(all_libraries)

tmcwebserver_LDADD = -lboost_system -lpthread
noinst_HEADERS = tmcjson.h tmcpsql.h
