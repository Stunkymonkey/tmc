#include <iostream>

#include "tmcdata.h"

using namespace std;
using namespace pqxx;

TmcData::TmcData(string db_name, string user, string password, string hostaddr, string port) {
	// TODO check parameter
	dbConfig = "dbname = " + db_name + " user = " + user + " password = " + password + " hostaddr = " + hostaddr + " port = " + port;
	// cout << dbConfig << endl;
	try {
		C = new pqxx::connection(dbConfig);
		if (C->is_open()) {
			cout << "Opened database successfully: " << C->dbname() << endl;
		} else {
			cout << "Can't open database" << endl;
		}
	} catch (const std::exception &e) {
		cerr << e.what() << std::endl;
	}
}

TmcData::~TmcData()
{
	C->disconnect();
	free(C);
}

bool TmcData::checkConnection() {
	try {
		if (C->is_open()) {
			return true;
		} else {
			return false;
		}
	} catch (const std::exception &e) {
		cerr << e.what() << std::endl;
		return false;
	}
}

tuple<std::string, std::string> TmcData::minMaxDate() {
	// return id of event, because it is needed for additional group infos
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return make_tuple("", "");
	}

	string sql = "SELECT MIN(\"start\"), MAX(\"end\") FROM events;";

	nontransaction N(*C);
	result R( N.exec( sql ));

	std::string min;
	std::string max;
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		min = c[0].as<std::string>();
		max = c[1].as<std::string>();
	}
	return make_tuple(min, max);
}

string TmcData::query(double northEastLat, double  northEastLng, double southWestLat, double southWestLng, string start, string end) {
	// return id of event, because it is needed for additional group infos
	std::vector<int> events;
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return "";
	}

	string sql =	"SELECT ST_AsText(points.point), "
							"events.id, "
							"events.event, "
							"events.start, "
							"events.\"end\", "
							"events.extension, "
							"events.dir_positive "
					"FROM points "
					"INNER JOIN events ON events.lcd = points.id "
					"WHERE ST_Contains(ST_MakeEnvelope( "
						+ to_string(southWestLng) + ", "
						+ to_string(southWestLat) + ", "
						+ to_string(northEastLng) + ", "
						+ to_string(northEastLat) +
					"), points.point) "
					"AND '" + start + "' <= events.start "
					"AND '" + end + "' >= events.\"end\";";
	// TODO check if string-date really works here

	nontransaction N(*C);
	result R( N.exec( sql ));

	std::string point;
	std::string id;
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		point = c[0].as<std::string>();
		id = c[1].as<std::string>();
		cout << point << " " << id << endl;
	}
	return "";
}