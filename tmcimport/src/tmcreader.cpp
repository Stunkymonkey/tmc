#include "tmcreader.h"

using namespace std;

TmcReader::TmcReader(string filename)
{
	boost::iostreams::file_source myCprdFile (filename, std::ios_base::in | std::ios_base::binary);

	bunzip2Filter.push (boost::iostreams::bzip2_decompressor());
	bunzip2Filter.push (myCprdFile);
}

TmcReader::~TmcReader()
{
	boost::iostreams::close(bunzip2Filter);
}

bool TmcReader::read(string &s) {
	// get next line and returen status of stream
	return (bool)getline(bunzip2Filter, s);
}

char TmcReader::peekChar() {
	// return next char, but not taking it out of buffer
	return bunzip2Filter.istream::peek();
}

bool TmcReader::getChunk(string &result) {
	// check if filreader is at the end
	if (peekChar() == EOF) {
		cout << "file is read" << endl;
		return false;
	}
	// check if next line begins with "tmc:" (only checking t)
	if (peekChar() != 't') {
		cout << "weird chunk" << endl;
		return false;
	}
	// if it does read until next "t" appears
	result = "";
	bool status = false;
	read(result);
	while(peekChar() != 't' && peekChar() != EOF) {
		result.append("\n");
		string tmp;
		status = read(tmp);
		result.append(tmp);
	}
	return status;
}