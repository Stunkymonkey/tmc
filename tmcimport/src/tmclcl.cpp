#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "tmclcl.h"

using namespace std;

TmcLCL::TmcLCL(string points, string poffset, TmcData *new_data) {
	data = new_data;

	file_points = std::ifstream(points);

	if(!file_points) {
		cerr << "Cannot open " + points + " file.\n";
	}

	file_poffset = std::ifstream(poffset);

	if(!file_poffset) {
		cerr << "Cannot open " + poffset + " file.\n";
	}

	char header[300];
	file_points.getline(header, sizeof(header));
	file_poffset.getline(header, sizeof(header));
}

TmcLCL::~TmcLCL() {
	file_points.close();
	file_poffset.close();
}

void TmcLCL::readPoints() {
	char line[200];
	while(!file_points.eof()) {
		file_points.getline(line, sizeof(line));
		addPoint(line);
	}
}

void TmcLCL::readOffsets() {
	char line[200];
	while(!file_poffset.eof()) {
		file_poffset.getline(line, sizeof(line));
		addOffset(line);
	}
}

void TmcLCL::addPoint(string new_line) {
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

void TmcLCL::addOffset(string new_line) {
	// checking if line is empty
	if (new_line.size() <= 0) {
		return;
	}

	vector<string> strs;
	// this splits the line into strs by spliting at ';'
	boost::split(strs, new_line, boost::is_any_of(";"));

	int id = 0;
	int neg = 0;
	int pos = 0;
	// remove special chars at end of line
	boost::trim_right(strs.at(4));
	if (! strs.at(2).empty())
	    id = stoi(strs.at(2));
	if (! strs.at(3).empty())
	    neg = stoi(strs.at(3));
	if (! strs.at(4).empty())
	    pos = stoi(strs.at(4));
	// inserting offset
	data->insertOffset(id, neg, pos);
}