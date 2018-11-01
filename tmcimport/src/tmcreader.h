#include <iostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

using namespace std;

class TmcReader {
public:
	TmcReader(string filename);
	~TmcReader();
	bool getChunk(string &s);
private:
	boost::iostreams::filtering_istream bunzip2Filter;
	bool read(string &s);
	char peekChar();
};
