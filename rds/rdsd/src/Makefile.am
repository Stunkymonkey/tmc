bin_PROGRAMS = rdsd
rdsd_SOURCES = rdsd.cpp conffile.cpp confsection.cpp confvalue.cpp \
							rdshandler.cpp rdssource.cpp rdsclient.cpp loghandler.cpp rdsdecoder.cpp rdschanneldata.cpp rdsgroup.cpp \
			altfreqlist.cpp radiotext.cpp tmclist.cpp

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
rdsd_LDFLAGS = $(all_libraries) 
noinst_HEADERS = conffile.h confsection.h confvalue.h conffile.h confsection.h \
									confvalue.h rdshandler.h rdssource.h rdsclient.h loghandler.h rdsd_errors.h \
					rdsdecoder.h rdschanneldata.h rdsgroup.h altfreqlist.h radiotext.h tmclist.h
