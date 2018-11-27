#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <ctime>

#include "tmcjson.h"
#include "tmcresult.h"

using namespace std;
namespace pt = boost::property_tree;

std::string TmcJson::min_max_date(std::string *min, std::string *max) {
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

std::string TmcJson::tmc_query(std::vector<struct TmcResult*> out) {
	pt::ptree root;

	pt::ptree events;
	for (std::vector<struct TmcResult*>::iterator it = out.begin() ; it != out.end(); ++it) {
		TmcResult *tmp = *it;
		pt::ptree event;

		event.put("event", tmp->event);
		event.put("start", tmp->start);
		event.put("end", tmp->end);
		event.put("path", tmp->path);
		events.push_back(std::make_pair("", event));
	}
	root.add_child("events", events);

	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}
