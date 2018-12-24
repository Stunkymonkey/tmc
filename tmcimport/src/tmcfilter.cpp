#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

#include "tmcfilter.h"

using namespace std;

// these values are hardcoded when event type defines end
set<int> cancel_set = {128, 1589, 334, 2028, 625, 672, 673, 399, 468,
	2029, 801, 971, 2030, 1025, 1127, 1314, 1214, 1585, 2033, 1620, 2034,
	2035, 1703, 1763, 1837, 1857, 1883, 2038,1911, 2039, 2040};

TmcFilter::TmcFilter(TmcData *new_data) {
	data = new_data;
}

TmcFilter::~TmcFilter() {
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
		time(&rawtime);
	}

	vector<string>::iterator it = old_strings.begin();

	while (it != old_strings.end()) {
		// check if strings are equal
		if (*it == *tok_iter){
			tok_iter++;
			it++;
		} else {
			// removed lines
			processLine(rawtime, *it, false);
			old_strings.erase(it);
		}
	}
	// if tok_iter is end all old_strings have to be ended
	while (tok_iter != tokens.end() && *tok_iter != "end") {
		// new lines
		old_strings.push_back(*tok_iter);
		processLine(rawtime, *tok_iter, true);
		tok_iter++;
	}
}

void TmcFilter::printEvent(time_t time, std::string line, bool isNew) {
	// debug helper
	struct tm *timeinfo;
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
	// Y displays the current sender
	// T is an encrypted message
	if (line.front() == 'Y' || line.front() == 'T') {
		return;
	}
	//printEvent(time, line, isNew);

	vector<string> strs;
	// this splits the line into strs by spliting at '=' and spaces
	boost::split(strs, line, boost::is_any_of(" ="));

	// seperating by event
	if ((strs.size() == 13) && ((strs[0].compare("S")) == 0)) {
		// single event
		int event = stoi(strs[2]);
		int loc = stoi(strs[4]);
		int ext = stoi(strs[6]);
		bool dir = stoi(strs[10]);
		if (!(cancel_set.find(event) != cancel_set.end())) {
			// event type is not canceling
			if (isNew) {
				data->startSingleEvent(time, loc, event, ext, dir);
			} else {
				data->endSingleEvent(time, loc, ext, dir);
			}
		} else {
			// event type is canceling
			if (isNew) {
				data->endSingleEvent(time, loc, ext, dir);
			}
		}
		
	} else if ((strs.size() == 11) && ((strs[0].compare("GF")) == 0)) {
		// initial group event
		int event = stoi(strs[2]);
		int loc = stoi(strs[4]);
		int ext = stoi(strs[6]);
		int ci = stoi(strs[8]);
		bool dir = stoi(strs[10]);
		if (!(cancel_set.find(event) != cancel_set.end())) {
			// event type is not canceling
			if (isNew) {
				ci_index[ci - 1] = data->startGroupEvent(time, loc, event, ext, dir);
			} else {
				data->endGroupEvent(time, loc, ext, dir);
			}
		} else {
			if (isNew) {
				data->endGroupEvent(time, loc, ext, dir);
			}
		}
	} else if ((strs.size() == 9) && ((strs[0].compare("GS")) == 0)) {
		// following group events
		int ci = stoi(strs[2]);
		int f1 = stoi(strs[6]);
		int f2 = stoi(strs[8]);
		if (isNew) {
			data->addGroupEventInfo(ci_index[ci - 1], f1, f2);
		}
	} else {
		cerr << "Invalid format" << endl;
	}
}
