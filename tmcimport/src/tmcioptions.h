#pragma once
#include <librds.h>
#include <string>

using namespace std;

class RdsqOptions{
public:
	RdsqOptions();
	~RdsqOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	int GetConnectionType() { return conn_type; }
	const string& GetServerName() { return server_name; }
	int GetPort() { return tcpip_port; }
	int GetSourceNum() { return source_num; }
	rds_events_t GetEventMask() { return event_mask; }
	const string& GetFileName() { return file_name; }
	const bool& GetInitState() { return init; }
	const string& GetPsqlHost() { return psql_host; }
	int GetPsqlPort() { return psql_port; }
	const string& GetPsqlDatabase() { return psql_database; }
	const string& GetPsqlUser() { return psql_user; }
	const string& GetPsqlPassword() { return psql_password; }
private:
	int conn_type;
	string server_name;
	int tcpip_port;
	int source_num;
	rds_events_t event_mask;
	string file_name;
	bool init;
	string psql_host;
	int psql_port;
	string psql_database;
	string psql_user;
	string psql_password;
	void show_version();
};
