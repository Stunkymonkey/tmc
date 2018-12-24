#include <iostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

class TmcReader {
public:
	TmcReader(std::string filename);
	~TmcReader();
	bool getChunk(std::string &s);
private:
	boost::iostreams::filtering_istream bunzip2Filter;
	bool read(std::string &s);
	std::string file_name;
	char peekChar();
};
