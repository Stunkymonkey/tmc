#include "tmcdata.h"
#include <boost/tokenizer.hpp>
#include <iostream>

using namespace std;

TmcData::TmcData()
{
}

TmcData::~TmcData()
{
	old_strings.clear();
}

void TmcData::addChunk(string new_string) {
	// this function checks what updates are in the new string
	typedef boost::tokenizer<boost::char_separator<char> >tokenizer;
	boost::char_separator<char> sep("\n");
	tokenizer tokens(new_string, sep);

	tokenizer::iterator tok_iter = tokens.begin();
	string header = *tok_iter;
	tok_iter++;

	// TODO find what time struct is wanted and create it
	string time_str;
	if (header.length() ==24) {
		time_str = header.substr(5, 19);
		// cout << time_str << endl;
	} else {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[20];

		time (&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, sizeof(buffer), "%FT%T", timeinfo);
		time_str = buffer;
	}
	// std::get_time()

	vector<string>::iterator it = old_strings.begin();

	while (it != old_strings.end()) {
		if (*it == *tok_iter){
			tok_iter++;
			it++;
		} else {
			// removed lines
			old_line(time_str, *it);
			old_strings.erase(it);
		}
	}
	while (tok_iter != tokens.end()) {
		// new lines
		old_strings.push_back(*tok_iter);
		new_line(time_str, *tok_iter);
		tok_iter++;
	}
}


void TmcData::new_line(std::string time_str, std::string new_line) {
	cout << time_str <<"\tnew:\t\t" << new_line << endl;
}
void TmcData::old_line(std::string time_str, std::string old_line) {
	cout << time_str << "\tremove:\t\t" << old_line << endl;
}
