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

void TmcData::insertLcd(int id, float x, float y) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"INSERT INTO points (id, point) " \
					"VALUES ( " \
					"" + to_string(id) + ", " \
					"ST_Point(" + to_string(x) + "," + to_string(y) + ")); ";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::startSingleEvent(time_t time, int loc, int event, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"INSERT INTO events (\"start\", \"end\", lcd, event, extension, dir_positive) " \
					"VALUES ( " \
					"to_timestamp(" + to_string(time) + "), " \
					"NULL, " \
					"" + to_string(loc) + ", " \
					"" + to_string(event) + ", " \
					"" + to_string(ext) + ", " \
					"" + to_string(dir) + "::BOOLEAN); ";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::endSingleEvent(time_t time, int loc, int event, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"UPDATE events " \
					"SET \"end\" = to_timestamp(" + to_string(time) + ") " \
					"WHERE (lcd = " + to_string(loc) + ") AND (\"end\" IS NULL) " \
					"AND (extension = " + to_string(ext) + ") " \
					"AND (dir_positive = " + to_string(dir) + "::BOOLEAN);";

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

	string sql = 	"INSERT INTO events (\"start\", \"end\", lcd, event, extension, dir_positive) " \
					"VALUES ( " \
					"to_timestamp(" + to_string(time) + "), " \
					"NULL, " \
					"" + to_string(loc) + ", " \
					"" + to_string(event) + ", " \
					"" + to_string(ext) + ", " \
					"" + to_string(dir) + "::BOOLEAN ) " \
					"RETURNING id;";

	nontransaction N(*C);
	result R( N.exec( sql ));

	int id;
	for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
		id = c[0].as<int>();
	}
	return id;
}

void TmcData::endGroupEvent(time_t time, int loc, int event, int ext, bool dir) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}

	string sql = 	"UPDATE events " \
					"SET \"end\" = to_timestamp(" + to_string(time) + ") " \
					"WHERE (lcd = " + to_string(loc) + ") AND (\"end\" IS NULL);";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::addGroupEventInfo(int id, int f1, int f2) {
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return;
	}
	if (id == 0) {
		return;
	}

	string sql = 	"INSERT INTO events_info (id, f1, f2) " \
					"VALUES ( " \
					"" + to_string(id) + ", " \
					"" + to_string(f1) + ", " \
					"" + to_string(f2) + "); ";

	work W(*C);
	W.exec( sql );
	W.commit();
}
