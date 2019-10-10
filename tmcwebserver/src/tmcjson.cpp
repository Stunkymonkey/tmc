#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
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

bool TmcJson::tmc_request(string body, float& northEastLat, float& northEastLng, float& southWestLat, float& southWestLng, string& start_date, string& end_date, string& start_time, string& end_time) {
	std::stringstream ss;
	pt::ptree root;
	ss << body;
	pt::read_json(ss, root);
	try {
		northEastLat = root.get<float>("view.northeast.lat");
		northEastLng = root.get<float>("view.northeast.lng");
		southWestLat = root.get<float>("view.southwest.lat");
		southWestLng = root.get<float>("view.southwest.lng");
		start_date =  root.get<string>("date.start");
		end_date =  root.get<string>("date.end");
		start_time =  root.get<string>("time.start");
		end_time =  root.get<string>("time.end");
	} catch (const boost::property_tree::ptree_bad_path& e) {
		return false;
	} catch (...) {
		cout << "unknown error while parsing json-request" << endl;
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

		pt::ptree path;

		double longitude;
		double latitude;
		for (std::vector<std::pair<double,double>>::iterator i = tmp->path.begin(); i != tmp->path.end(); ++i)
		{
			pt::ptree point;
			pt::ptree cell;
			std::pair<double,double> tmp_point = *i;
			// read point
			longitude = get<1>(tmp_point);
			latitude = get<0>(tmp_point);
			// save it in point and add it
			cell.put_value(longitude);
			point.push_back(std::make_pair("", cell));
			cell.put_value(latitude);
			point.push_back(std::make_pair("", cell));
			// add the point to the path
			path.push_back(std::make_pair("", point));
		}
		event.add_child("path", path);

		// add single event to events
		events.push_back(std::make_pair("", event));
	}
	// make list of events
	root.add_child("events", events);

	std::ostringstream oss;
	pt::write_json(oss, root);
	return oss.str();
}
