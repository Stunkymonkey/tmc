#include <iostream>

#include "tmcdata.h"
#include "tmcresult.h"

using namespace std;
using namespace pqxx;

TmcData::TmcData(string db_name, string user, string password, string hostaddr, string port) {
	// TODO check parameter
	// password not set
	dbConfig = "dbname = " + db_name + " user = " + user;
	if (password != "") {
		dbConfig = dbConfig + " password = " + password;
	}
	dbConfig = dbConfig + " hostaddr = " + hostaddr + " port = " + port;
	
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

void TmcData::query(std::vector<struct TmcResult*>& out, double northEastLat, double  northEastLng, double southWestLat, double southWestLng, string start, string end) {
	// return id of event, because it is needed for additional group infos
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql =	"SELECT event_type.description, result.start, result.\"end\", result.path FROM ("
						"SELECT events.event, "
							"events.start, "
							"events.\"end\", "
							"get_path(events.lcd, events.extension, events.dir_negative) AS \"path\" "
					"FROM points "
					"INNER JOIN events ON events.lcd = points.id "
					"WHERE ST_Contains(ST_MakeEnvelope("
						+ to_string(southWestLng) + ", "
						+ to_string(southWestLat) + ", "
						+ to_string(northEastLng) + ", "
						+ to_string(northEastLat) +
					"), points.point) "
					"AND events.start < '" + end + " 23:59' "
					"AND events.\"end\" >='" + start + " 0:0'"
					") AS result LEFT JOIN event_type on event_type.id = result.event;";
	// TODO use better < and add 1 to date
	// TODO daterange?
	// @> 	contains range 	int4range(2,4) @> int4range(2,3)

	cout << sql << endl;

	nontransaction N(*C);
	result R( N.exec( sql ));

	// reserve size of result
	out.reserve(R.size());
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		TmcResult *tmp = new TmcResult();
		// read all to TmcResult
		tmp->event = c[0].as<std::string>();
		tmp->start = c[1].as<std::string>();
		tmp->end = c[2].as<std::string>();
		tmp->path = c[3].as<std::string>();
		out.push_back(tmp);
	}
	return;
}
