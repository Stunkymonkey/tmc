#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "tmcecl.h"

using namespace std;

TmcECL::TmcECL(std::string eventTypes, TmcData *new_data) {
	data = new_data;

	file_events = std::ifstream(eventTypes);

	if(!file_events) {
		cerr << "Cannot open " + eventTypes + " file.\n";
	}

	char header[500];
	file_events.getline(header, sizeof(header));
}

TmcECL::~TmcECL() {
	file_events.close();
}

void TmcECL::readEventTypes() {
	char line[1000];
	while(!file_events.eof()) {
		file_events.getline(line, sizeof(line));
		addEventType(line);
	}
}

void TmcECL::addEventType(string new_line) {
	// checking if line is empty
	if (new_line.size() <= 0) {
		return;
	}

	vector<string> strs;
	// this splits the line into strs by spliting at ';'
	boost::split(strs, new_line, boost::is_any_of(";"));

	// some lines do not have an event code. ignoring them by checking if exists
	if (strs[6] != "") {
		strs[3].erase(remove(strs[3].begin(), strs[3].end(), '"'), strs[3].end());
		data->insertEventType(stoi(strs[6]), strs[3]);
	}
}
