#include "tmcwriter.h"
#include <iostream>
#include <ctime>

using namespace std;

TmcWriter::TmcWriter(string filename, bool append)
{
	std::ios_base::openmode bitmask = std::ios_base::binary;
	if (append) {
		bitmask |= std::ios_base::app;
	} else {
		bitmask |= std::ios_base::out;
	}
	boost::iostreams::file_sink myCprdFile(filename, bitmask);
	bzip2Filter.push(boost::iostreams::bzip2_compressor());
	bzip2Filter.push(myCprdFile);
}

TmcWriter::~TmcWriter()
{
	boost::iostreams::close(bzip2Filter);
}

bool TmcWriter::write(string s) {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,sizeof(buffer),"%FT%T",timeinfo);
	std::string str_time(buffer);

	if (!bzip2Filter.good()) return false;

	// the last char is always some weird char, so remove it
	s.pop_back();
	if (s == last_s) {
		//cout << "dup: " << str_time << endl;
		bzip2Filter << "dup: " << str_time << endl;
	} else {
		//cout << "tmc: " << str_time << endl;
		bzip2Filter << "tmc: " << str_time << endl;
		//cout << s << endl;
		bzip2Filter << s << endl;
	}
	last_s = s;
	return true;
}