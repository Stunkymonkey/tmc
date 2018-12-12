bin_PROGRAMS = tmcwebserver
tmcwebserver_SOURCES = tmcwebserver.cpp tmcjson.cpp tmcdata.cpp tmcwoptions.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes)

# the library search path.
tmcwebserver_LDFLAGS = $(all_libraries)

tmcwebserver_LDADD = -lboost_system -lboost_program_options -lpthread -lpqxx -lpq
noinst_HEADERS = tmcjson.h tmcdata.h tmcresult.h tmcwoptions.h
