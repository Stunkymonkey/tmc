bin_PROGRAMS = tmcimport
tmcimport_SOURCES = tmcimport.cpp tmcqueryhandler.cpp tmcioptions.cpp tmcreader.cpp tmcfilter.cpp tmcdata.cpp tmclcl.cpp tmcecl.cpp

# set the include path found by configure
AM_CPPFLAGS = $(all_includes)

# the library search path.
tmcimport_LDFLAGS = $(all_libraries)

tmcimport_LDADD = -lrds -lboost_{iostreams,serialization,system,program_options} -L../../rds/librds/src/.libs/
noinst_HEADERS = tmcqueryhandler.h tmcioptions.h tmcreader.h tmcfilter.h tmcdata.h tmclcl.h tmcecl.h
