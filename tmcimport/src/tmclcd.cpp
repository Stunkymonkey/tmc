#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "tmclcd.h"

using namespace std;

TmcLcd::TmcLcd(string points, string poffset, TmcData* new_data) {
	data = new_data;

	in = std::ifstream(points);

	if(!in) {
		cerr << "Cannot open " + points + " file.\n";
	}

	char header[300];
	in.getline(header, sizeof(header));
}

TmcLcd::~TmcLcd() {
	in.close();
}

void TmcLcd::readLines() {
	char line[300];
	while(!in.eof()) {
		in.getline(line, sizeof(line));
		addLine(line);
	}
}

void TmcLcd::addLine(string new_line) {
	// checking if line is empty
	if (new_line.size() <= 0) {
		return;
	}

	vector<string> strs;
	// this splits the line into strs by spliting at ';'
	boost::split(strs, new_line, boost::is_any_of(";"));

	// inserting dots for floats
	strs.at(22).insert(4,".");
	strs.at(23).insert(3,".");
	data->insertLcd(stoi(strs[2]), stof(strs[22]), stof(strs[23]));
}