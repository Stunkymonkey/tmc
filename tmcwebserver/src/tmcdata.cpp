#include <iostream>

#include "tmcdata.h"
#include "tmcresult.h"

using namespace std;
using namespace pqxx;

TmcData::TmcData(string db_name, string user, string password, string hostaddr, string port) {
	dbConfig = "dbname = " + db_name + " user = " + user;
	// password not set
	if (password != "") {
		dbConfig = dbConfig + " password = " + password;
	}
	dbConfig = dbConfig + " hostaddr = " + hostaddr + " port = " + port;
	
	try {
		C = new pqxx::connection(dbConfig);
	} catch (const std::exception &e) {
		cerr << e.what() << std::endl;
		exit(1);
	}
	if (C->is_open()) {
		cout << "Opened database successfully: " << C->dbname() << endl;
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

std::tuple<std::string, std::string> TmcData::minMaxDate() {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return make_tuple("", "");
	}

	string sql = "SELECT MIN(\"start\")::date, MAX(\"end\")::date FROM events;";

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

void TmcData::query(std::vector<struct TmcResult*>& out,
					double northEastLat,
					double northEastLng,
					double southWestLat,
					double southWestLng,
					string start_date,
					string end_date,
					string start_time,
					string end_time) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}
	string sql;
	// whether time is defined or not
	if (start_time == end_time) {
		sql =	"SELECT event_type.description, result.start, COALESCE(result.\"end\", NOW()::timestamp), result.path FROM ("
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
				"AND events.start < '" + end_date + " 0:0'::TIMESTAMP + INTERVAL '1 day' "
				"AND events.\"end\" >='" + start_date + " 0:0'"
				") AS result LEFT JOIN event_type on event_type.id = result.event;";
	} else {
		sql =	"SELECT event_type.description, result.start, COALESCE(result.\"end\", NOW()::timestamp), result.path FROM ("
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
				"AND events.start < '" + end_date + " 0:0'::TIMESTAMP + INTERVAL '1 day' "
				"AND events.\"end\" >='" + start_date + " 0:0' "
				"AND EXTRACT(HOUR FROM events.start) >= " + start_time + " "
				"AND EXTRACT(HOUR FROM events.start) < " + end_time + " "
				"AND EXTRACT(HOUR FROM events.\"end\") >= " + start_time + " "
				"AND EXTRACT(HOUR FROM events.\"end\") < " + end_time + " "
				") AS result LEFT JOIN event_type on event_type.id = result.event;";
	}

	//cout << sql << endl;

	nontransaction N(*C);
	result R( N.exec( sql ));

	// reserve size of result
	out.reserve(R.size());
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		TmcResult *tmp = new TmcResult();
		// read all to TmcResult
		tmp->event = (c[0].is_null()) ? "" : c[0].as<std::string>();
		tmp->start = (c[1].is_null()) ? "" : c[1].as<std::string>();
		tmp->end = (c[2].is_null()) ? "" : c[2].as<std::string>();
		tmp->path = (c[3].is_null()) ? "" : c[3].as<std::string>();
		out.push_back(tmp);
	}
	return;
}
