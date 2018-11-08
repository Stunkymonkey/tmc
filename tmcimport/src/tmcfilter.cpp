#include "tmcfilter.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace std;

TmcFilter::TmcFilter()
{
}

TmcFilter::~TmcFilter()
{
	old_strings.clear();
}

void TmcFilter::addChunk(string new_string) {
	// this function checks what updates are in the new string
	typedef boost::tokenizer<boost::char_separator<char> >tokenizer;
	boost::char_separator<char> sep("\n");
	tokenizer tokens(new_string, sep);

	tokenizer::iterator tok_iter = tokens.begin();
	string header = *tok_iter;
	tok_iter++;

	time_t rawtime;
	// if line provides time use it or create it
	if (header.length() ==24) {
		string time_str = header.substr(5, 19);
		struct tm tm;
		strptime(time_str.c_str(), "%FT%T", &tm);
		rawtime = mktime(&tm);
	} else {
		// TODO test
		time (&rawtime);
	}

	vector<string>::iterator it = old_strings.begin();

	while (it != old_strings.end()) {
		if (*it == *tok_iter){
			tok_iter++;
			it++;
		} else {
			// removed lines
			processLine(rawtime, *it, false);
			old_strings.erase(it);
		}
	}
	while (tok_iter != tokens.end()) {
		// new lines
		old_strings.push_back(*tok_iter);
		processLine(rawtime, *tok_iter, true);
		tok_iter++;
	}
}

void TmcFilter::printEvent(time_t time, std::string line, bool isNew) {
	// debug helper
	struct tm * timeinfo;
	char buffer[80];
	timeinfo = localtime(&time);

	strftime(buffer,sizeof(buffer),"%FT%T",timeinfo);
	std::string time_str(buffer);
	if (isNew) {
		cout << time_str <<"\tnew:\t" << line << endl;
	} else {
		cout << time_str <<"\told:\t" << line << endl;
	}
}


void TmcFilter::processLine(time_t time, std::string line, bool isNew) {
	if (line.front() == 'Y') {
		return;
	}
	printEvent(time, line, isNew);

	vector<string> strs;
	// this splits the line into strs by spliting at '=' and spaces
	boost::split(strs, line, boost::is_any_of(" ="));

	if ((strs.size() == 13) && ((strs[0].compare("S")) == 0)) {
		// single event
		int a = stoi(strs[2]);
		std::cout << a << endl;
	} else if ((strs.size() == 11) && ((strs[0].compare("GF")) == 0)) {
		// initial group event
		int a = stoi(strs[2]);
		std::cout << a << endl;
	} else if ((strs.size() == 9) && ((strs[0].compare("GS")) == 0)) {
		// following group events
		int a = stoi(strs[2]);
		std::cout << a << endl;
	} else {
		cerr << "Invalid format" << endl;
	}
}
