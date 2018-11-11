#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <ctime>

#include "tmcjson.h"

using namespace std;
namespace pt = boost::property_tree;

string TmcJson::min_max_date(time_t *min, time_t *max) {
	pt::ptree root;

	struct tm * timeinfo;
	char buffer[40];

	timeinfo = localtime(min);
	strftime(buffer,sizeof(buffer),"%FT%T", timeinfo);
	root.put("min", buffer);

	timeinfo = localtime(max);
	strftime(buffer,sizeof(buffer),"%FT%T", timeinfo);
	cout << buffer << endl;

	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}

string TmcJson::query() {
	pt::ptree root;
	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}
