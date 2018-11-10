#include <iostream>

#include "tmcdata.h"

using namespace std;
//using namespace pqxx;

TmcData::TmcData(string db_name, string user, string password, string hostaddr, string port) {
	// TODO check parameter
	dbConfig = "dbname = testdb user = postgres password = cohondob hostaddr = 127.0.0.1 port = 5432";
}

TmcData::~TmcData()
{
}

bool TmcData::connect() {
	/*try {
		// TODO use attribute not local var
		connection C(dbConfig);
		if (C.is_open()) {
			cout << "Opened database successfully: " << C.dbname() << endl;
			return true;
		} else {
			cout << "Can't open database" << endl;
			return false;
		}
	} catch (const std::exception &e) {
		cerr << e.what() << std::endl;
		return false;
	}*/
	return false;
}

void TmcData::close() {
	// C.disconnect();
}

bool TmcData::insertLcd() {
	return true;
}

void TmcData::startSingleEvent(time_t time, int loc, int event, int ext, bool dir) {

}

void TmcData::endSingleEvent(time_t time, int loc, int event, int ext, bool dir) {

}

int TmcData::startGroupEvent(time_t time, int loc, int event, int ext, bool dir) {
	return 0;
}

void TmcData::endGroupEvent(time_t time, int loc, int event, int ext, bool dir) {

}

void TmcData::addGroupEventInfo(int id, int f1, int f2) {

}
