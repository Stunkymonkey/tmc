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
			cout << "Opened database successfully: " << C->dbname() << endl;
			return true;
		} else {
			cout << "Can't open database" << endl;
			return false;
		}
	} catch (const std::exception &e) {
		cerr << e.what() << std::endl;
		return false;
	}
	return false;
}

void TmcData::insertLcd() {
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
					"" + to_string(dir) + "::BOOLEAN ); ";

	work W(*C);
	W.exec( sql );
	W.commit();
}

void TmcData::endSingleEvent(time_t time, int loc, int event, int ext, bool dir) {

}

int TmcData::startGroupEvent(time_t time, int loc, int event, int ext, bool dir) {
	// return id of event, because it is needed for additional group infos
	if (!C->is_open()) {
		cout << "Database closed unexpected" << endl;
		return 0;
	}

	// TODO get correct id with query
	string sql = 	"INSERT INTO events (\"start\", \"end\", lcd, event, extension, dir_positive) " \
					"VALUES ( " \
					"to_timestamp(" + to_string(time) + "), " \
					"NULL, " \
					"" + to_string(loc) + ", " \
					"" + to_string(event) + ", " \
					"" + to_string(ext) + ", " \
					"" + to_string(dir) + "::BOOLEAN ); ";

	work W(*C);
	W.exec( sql );
	W.commit();
	// TODO get correct id with query
	return 0;
}

void TmcData::endGroupEvent(time_t time, int loc, int event, int ext, bool dir) {

}

void TmcData::addGroupEventInfo(int id, int f1, int f2) {

}
