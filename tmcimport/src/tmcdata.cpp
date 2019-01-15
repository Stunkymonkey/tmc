#include <iostream>

#include "tmcdata.h"

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

void TmcData::initDatabase() {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}
	string postgis = "CREATE EXTENSION postgis;";

	string points = 	"CREATE TABLE IF NOT EXISTS points ("
							"id SERIAL UNIQUE PRIMARY KEY,"
							"point GEOMETRY"
						");";
	string points_index = "CREATE INDEX IF NOT EXISTS areas_points ON points USING GIST (point);";

	string events = 	"CREATE TABLE IF NOT EXISTS events ("
							"id bigserial UNIQUE PRIMARY KEY,"
							"\"start\" TIMESTAMP,"
							"\"end\" TIMESTAMP,"
							"lcd SERIAL,"
							"event SERIAL,"
							"extension SERIAL,"
							"dir_negative BOOLEAN"
						");";
	string time_index = "CREATE INDEX IF NOT EXISTS time_index ON events (start, \"end\");";
	string events_info = 	"CREATE TABLE events_info ("
								"id bigserial,"
								"CI SERIAL,"
								"F1 SERIAL,"
								"F2 SERIAL"
							");";

	string poffset = 	"CREATE TABLE IF NOT EXISTS point_offset ("
							"lcd SERIAL UNIQUE PRIMARY KEY,"
							"positive SERIAL,"
							"negative SERIAL,"
							"positive_path GEOMETRY,"
							"negative_path GEOMETRY"
						");";

	string event_type = "CREATE TABLE  IF NOT EXISTS event_type ("
							"id SERIAL UNIQUE PRIMARY KEY ,"
							"description VARCHAR(200)"
						");";

	string get_path = 	"CREATE OR REPLACE FUNCTION get_path(INTEGER, INTEGER, BOOLEAN)"
						"RETURNS TEXT AS $path$"
						"	DECLARE"
						"		tmp_lcd ALIAS FOR $1;"
						"		extension ALIAS FOR $2;"
						"		dir_negative ALIAS FOR $3;"
						"		result TEXT;"
						"	BEGIN"
						"		SELECT CONCAT(ST_X(points.point), ':' , ST_Y(points.point)) INTO result FROM points WHERE points.id = tmp_lcd;"	
						"		WHILE (0 < extension)"
						"		LOOP"
						"			IF dir_negative THEN"
						"				SELECT negative INTO tmp_lcd FROM point_offset WHERE point_offset.lcd = tmp_lcd;"	
						"			ELSE"
						"				SELECT positive INTO tmp_lcd FROM point_offset WHERE point_offset.lcd = tmp_lcd;"	
						"			END IF;"
						"			IF tmp_lcd = 0 THEN"
						"				EXIT;"
						"			END IF;"
						"			SELECT CONCAT(result, ',' , ST_X(points.point), ':' , ST_Y(points.point)) INTO result FROM points WHERE points.id = tmp_lcd;"	
						"			extension = extension - 1;"
						"		END LOOP;"
						"		RETURN result;"
						"	END;"
						"	$path$"
						"LANGUAGE plpgsql;";

	work W(*C);
	W.exec( postgis );
	W.exec( points );
	W.exec( points_index );
	W.exec( events );
	W.exec( time_index );
	W.exec( events_info );
	W.exec( poffset );
	W.exec( event_type );
	W.exec( get_path );
	W.commit();
	cout << "successfully created databases" << endl;
}

void TmcData::insertLcd(int id, float x, float y) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"INSERT INTO points (id, point) " \
					"VALUES ( " \
					"" + to_string(id) + ", " \
					"ST_Point(" + to_string(x) + "," + to_string(y) + "))" \
					"ON CONFLICT DO NOTHING;";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::insertOffset(int lcd, int neg, int pos) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"INSERT INTO point_offset (lcd, negative, positive) " \
					"VALUES ( " \
					"" + to_string(lcd) + ", " \
					"" + to_string(neg) + ", " \
					"" + to_string(pos) + ")" \
					"ON CONFLICT DO NOTHING;";
	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::insertEventType(int eventcode, string desc) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"INSERT INTO event_type (id, description) " \
					"VALUES ( " \
					"" + to_string(eventcode) + ", " \
					"" + desc + ")" \
					"ON CONFLICT DO NOTHING;";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::startSingleEvent(time_t time, int loc, int event, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}
	string insert =	"INSERT INTO events (\"start\", \"end\", lcd, event, extension, dir_negative) " \
					"SELECT " \
					"to_timestamp(" + to_string(time) + "), " \
					"NULL, " \
					"" + to_string(loc) + ", " \
					"" + to_string(event) + ", " \
					"" + to_string(ext) + ", " \
					"" + to_string(dir) + "::BOOLEAN";
	string update = "UPDATE events " \
					"SET \"end\"=NULL " \
					"WHERE " \
					"\"end\"=to_timestamp(" + to_string(time) + ") AND " \
					"lcd=" + to_string(loc) + " AND " \
					"event=" + to_string(event) + " AND " \
					"extension=" + to_string(ext) + " AND " \
					"dir_negative=" + to_string(dir);
	string sql = "WITH upsert AS (" + update + " RETURNING *) " + insert + " WHERE NOT EXISTS (SELECT * FROM upsert);";

	//cout << sql << endl;

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::endSingleEvent(time_t time, int loc, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"UPDATE events " \
					"SET \"end\" = to_timestamp(" + to_string(time) + ") " \
					"WHERE (lcd = " + to_string(loc) + ") AND (\"end\" IS NULL) " \
					"AND (extension = " + to_string(ext) + ") " \
					"AND (dir_negative = " + to_string(dir) + "::BOOLEAN);";

	work W(*C);
	W.exec( sql );
	W.commit();
}

int TmcData::startGroupEvent(time_t time, int loc, int event, int ext, bool dir) {
	// return id of event, because it is needed for additional group infos
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return 0;
	}

	string insert =	"INSERT INTO events (\"start\", \"end\", lcd, event, extension, dir_negative) " \
					"SELECT " \
					"to_timestamp(" + to_string(time) + "), " \
					"NULL, " \
					"" + to_string(loc) + ", " \
					"" + to_string(event) + ", " \
					"" + to_string(ext) + ", " \
					"" + to_string(dir) + "::BOOLEAN ";
	string update = "UPDATE events " \
					"SET \"end\"=NULL " \
					"WHERE " \
					"lcd=" + to_string(loc) + " AND " \
					"event=" + to_string(event) + " AND " \
					"extension=" + to_string(ext) + " AND " \
					"dir_negative=" + to_string(dir);
	string sql = "WITH upsert AS (" + update + " RETURNING *) " + insert + " WHERE NOT EXISTS (SELECT * FROM upsert);";

	nontransaction N(*C);
	result R( N.exec( sql ));

	// TODO return id with " RETURNING id"

	int id;
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		id = c[0].as<int>();
	}
	return id;
}

void TmcData::endGroupEvent(time_t time, int loc, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"UPDATE events " \
					"SET \"end\" = to_timestamp(" + to_string(time) + ") " \
					"WHERE (lcd = " + to_string(loc) + ") AND (\"end\" IS NULL) " \
					"AND (extension = " + to_string(ext) + ") " \
					"AND (dir_negative = " + to_string(dir) + "::BOOLEAN);";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::addGroupEventInfo(int id, int ci, int f1, int f2) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}
	if (id == 0) {
		return;
	}

	string sql = 	"INSERT INTO events_info (id, ci, f1, f2) " \
					"VALUES ( " \
					"" + to_string(id) + ", " \
					"" + to_string(ci) + ", " \
					"" + to_string(f1) + ", " \
					"" + to_string(f2) + "); ";

	work W(*C);
	W.exec( sql );
	W.commit();
}
