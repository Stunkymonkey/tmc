#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <set>

#include "tmcfilter.h"

using namespace std;

// these values are hardcoded when event_type defines end of an event
const set<int> cancel_set = {128, 1589, 334, 2028, 625, 672, 673, 399, 468,
	2029, 801, 971, 2030, 1025, 1127, 1314, 1214, 1585, 2033, 1620, 2034,
	2035, 1703, 1763, 1837, 1857, 1883, 2038,1911, 2039, 2040};

TmcFilter::TmcFilter(TmcData *new_data, bool DropGFData) {
	data = new_data;
	dropGFData = DropGFData;
}

TmcFilter::~TmcFilter() {
	old_strings.clear();
	indexes.clear();
}

void TmcFilter::addChunk(string new_string) {
	// this function checks what updates are in the new string
	typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
	boost::char_separator<char> sep("\n");
	tokenizer tokens(new_string, sep);

	tokenizer::iterator tok_iter = tokens.begin();
	string header = *tok_iter;
	tok_iter++;

	time_t rawtime;
	// if line provides time use it or create it
	if (header.length() ==24) {
		string time_str = header.substr(5, 19);
		struct tm tm = { 0 };
		if (strptime(time_str.c_str(), "%FT%T", &tm)) {
			rawtime = mktime(&tm);
		} else {
			cout << "strptime failed" << endl;
		}
	} else {
		time(&rawtime);
	}

	list<string>::iterator current_old_string = old_strings.begin();
	list<int>::iterator current_id = indexes.begin();

	if (tok_iter != tokens.end()) {
		// events exist
		while (current_old_string != old_strings.end()) {
			// check if strings are equal
			if (*current_old_string == *tok_iter){
				tok_iter++;
				current_old_string++;
				current_id++;
			} else {
				// removed lines
				processLine(rawtime, *current_old_string, false, *current_id);
				current_old_string = old_strings.erase(current_old_string);
				current_id = indexes.erase(current_id);
			}
		}
	} else {
		// no events are available
		while (current_old_string != old_strings.end()) {
			// removed lines
			processLine(rawtime, *current_old_string, false, *current_id);
			current_old_string = old_strings.erase(current_old_string);
			current_id = indexes.erase(current_id);
		}
	}
	
	// if tok_iter is at end all old_strings have to be ended
	while (tok_iter != tokens.end()) {
		// new lines
		processLine(rawtime, *tok_iter, true, 0);
		old_strings.push_back(*tok_iter);
		tok_iter++;
	}
}

void TmcFilter::printEvent(time_t time, std::string line, bool isNew, int index) {
	// debug helper
	struct tm *timeinfo;
	char buffer[80];
	timeinfo = localtime(&time);

	strftime(buffer,sizeof(buffer),"%FT%T",timeinfo);
	std::string time_str(buffer);
	if (isNew) {
		cout << time_str <<"\tnew:\t" << line << " : " << index << endl;
	} else {
		cout << time_str <<"\told:\t" << line << " : " << index << endl;
	}
}


void TmcFilter::processLine(time_t time, std::string line, bool isNew, int index) {
	// Y displays the current sender
	// T is an encrypted message
	if (line.front() == 'Y' || line.front() == 'T') {
		if (isNew) {
			indexes.push_back(0);
		}
		return;
	}
	//printEvent(time, line, isNew, index);

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
				indexes.push_back(data->startSingleEvent(time, loc, event, ext, dir));
			} else {
				data->endSingleEvent(index, time, loc, ext, dir);
			}
		} else {
			// event type is canceling
			if (isNew) {
				data->endSingleEvent(index, time, loc, ext, dir);
				indexes.push_back(0);
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
				int id = data->startGroupEvent(time, loc, event, ext, dir);
				indexes.push_back(id);
				ci_index.at(ci - 1) = id;
			} else {
				data->endGroupEvent(index, time, loc, ext, dir);
			}
		} else {
			if (isNew) {
				data->endGroupEvent(index, time, loc, ext, dir);
				indexes.push_back(0);
			}
		}
	} else if ((strs.size() == 9) && ((strs[0].compare("GS")) == 0)) {
		// following group events
		int ci = stoi(strs[2]);
		int gsi = stoi(strs[4]);
		int f1 = stoi(strs[6]);
		int f2 = stoi(strs[8]);
		if (isNew) {
			if (!dropGFData) {
				data->addGroupEventInfo(ci_index.at(ci - 1), gsi, f1, f2);
			}
			indexes.push_back(0);
		}
	} else {
		if (isNew) {
			cerr << "Invalid format: ";
			printEvent(time, line, isNew, index);
			indexes.push_back(0);
		}
	}
}
