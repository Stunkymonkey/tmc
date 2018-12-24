#pragma once
#include <string>
#include <pqxx/pqxx>
#include <tuple>

class TmcData{
public:
	TmcData(std::string db_name, std::string user, std::string password, std::string hostaddr, std::string port);
	~TmcData();
	bool checkConnection();
	std::tuple<std::string, std::string> minMaxDate();
	void query(std::vector<struct TmcResult*>& out, double northEastLat, double  northEastLng, double southWestLat, double southWestLng, std::string start_date, std::string end_date, std::string start_time, std::string end_time);
private:
	std::string dbConfig = "";
	pqxx::connection* C;
};
