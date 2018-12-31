#pragma once
#include <string>
#include <pqxx/pqxx>

class TmcData{
public:
	TmcData(std::string db_name, std::string user, std::string password, std::string hostaddr, std::string port);
	~TmcData();
	bool checkConnection();
	void initDatabase();
	// LCL based
	void insertLcd(int id, float x, float y);
	void insertOffset(int lcd, int neg, int pos);
	void insertEventType(int eventcode, std::string desc);
	// event based
	void startSingleEvent(time_t time, int loc, int event, int ext, bool dir);
	void endSingleEvent(time_t time, int loc, int ext, bool dir);
	int startGroupEvent(time_t time, int loc, int event, int ext, bool dir);
	void endGroupEvent(time_t time, int loc, int ext, bool dir);
	void addGroupEventInfo(int id, int ci, int f1, int f2);
private:
	std::string dbConfig = "";
	pqxx::connection* C;
};
