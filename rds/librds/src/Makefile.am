
# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
noinst_HEADERS = rdsconnection.h rdsdcommandlist.h
lib_LTLIBRARIES = librds.la
librds_la_LDFLAGS = -module -shared
librds_la_SOURCES = librds.cpp rdsconnection.cpp rdsdcommandlist.cpp

AM_CXXFLAGS = -fPIC
include_HEADERS = librds.h
