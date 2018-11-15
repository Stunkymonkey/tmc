#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <ctime>

#include "tmcjson.h"

using namespace std;
namespace pt = boost::property_tree;

string TmcJson::min_max_date(std::string *min, std::string *max) {
	pt::ptree root;

	root.put("min", *min);
	root.put("max", *max);

	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}

bool TmcJson::tmc_request(string body, double& northEastLat, double& northEastLng, double& southWestLat, double& southWestLng, string& start, string& end) {
	std::stringstream ss;
	pt::ptree root;
	ss << body;
	pt::read_json(ss, root);
	try {
		northEastLat = root.get<double>("view.northeast.lat");
		northEastLng = root.get<double>("view.northeast.lng");
		southWestLat = root.get<double>("view.southwest.lat");
		southWestLng = root.get<double>("view.southwest.lng");
		start =  root.get<string>("date.start");
		end =  root.get<string>("date.end");
	} catch (const boost::property_tree::ptree_bad_path& e) {
		return false;
	} catch (...) {
		// TODO fix
		cout << "unknown error" << endl;
	}
	return true;
}

string TmcJson::tmc_query() {
	pt::ptree root;
	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}
