#include "tmcreader.h"

using namespace std;

TmcReader::TmcReader(string filename)
{
	boost::iostreams::file_source myCprdFile (filename, std::ios_base::in | std::ios_base::binary);

	bunzip2Filter.push(boost::iostreams::bzip2_decompressor());
	bunzip2Filter.push(myCprdFile);
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
	bool status = false;
	result = "";
	// the next line is the tmc line conataining the timestamp
	read(result);
	string tmp;
	while(peekChar() != 't' && peekChar() != 'd' && peekChar() != EOF) {
		result.append("\n");
		status = read(tmp);
		result.append(tmp);
	}
	// skip all duplicates and next read is at 't' or EOF
	while(peekChar() == 'd' && peekChar() != EOF) {
		read(tmp);
	}
	return status;
}