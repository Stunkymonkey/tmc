#include "tmcwriter.h"
#include <iostream>
#include <ctime>

using namespace std;

RdsWriter::RdsWriter(string filename, bool append)
{
	if (append) {
		myfile.open (filename, ios_base::app);
	} else {
		myfile.open (filename);
	}
	
}

RdsWriter::~RdsWriter()
{
	myfile.close();
}

bool RdsWriter::write(string s) {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,sizeof(buffer),"%FT%T",timeinfo);
	std::string str_time(buffer);

	if (!myfile.is_open()) return false;
	myfile << "tmc: " << str_time << endl;
	myfile << s << endl;
	return true;
}