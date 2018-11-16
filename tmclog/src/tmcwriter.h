#include <string>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

#include <fstream>

class TmcWriter{
public:
	TmcWriter(std::string filename, bool append);
	~TmcWriter();
	bool write(std::string s);
private:
	boost::iostreams::filtering_ostream bzip2Filter;
	std::string last_s = "";
};
