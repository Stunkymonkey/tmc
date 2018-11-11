#pragma once
#include <string>
#include <pqxx/pqxx>

class TmcData{
public:
	TmcData(std::string db_name, std::string user, std::string password, std::string hostaddr, std::string port);
	~TmcData();
	bool checkConnection();
	int startGroupEvent(time_t time, int loc, int event, int ext, bool dir);
private:
	std::string dbConfig = "";
	pqxx::connection* C;
};
