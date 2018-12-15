#pragma once
#include <string>

using namespace std;

class RdswOptions{
public:
	RdswOptions();
	~RdswOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	const string& GetServerName() { return web_ip; }
	int GetPort() { return web_port; }
	int GetThreads() { return threads; }
	const string& GetDocRoot() { return doc_root; }
	const string& GetPsqlHost() { return psql_host; }
	int GetPsqlPort() { return psql_port; }
	const string& GetPsqlUser() { return psql_user; }
	const string& GetPsqlPassword() { return psql_password; }
	const string& GetPsqlDatabase() { return psql_database; }
private:
	string web_ip;
	int web_port;
	int threads;
	string doc_root;
	string psql_host;
	int psql_port;
	string psql_user;
	string psql_password;
	string psql_database;
	void show_usage();
	void show_version();
};
