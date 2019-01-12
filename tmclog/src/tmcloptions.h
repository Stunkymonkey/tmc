#pragma once
#include <librds.h>
#include <string>

using namespace std;

class TmclOptions{
public:
	TmclOptions();
	~TmclOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	int GetConnectionType() { return conn_type; }
	const string& GetServerName() { return server_name; }
	int GetPort() { return tcpip_port; }
	int GetSourceNum() { return source_num; }
	rds_events_t GetEventMask() { return event_mask; }
	const string& GetFileName() { return file_name; }
	bool IsAppendMode() {return append_mode; }
private:
	int conn_type;
	string server_name;
	int tcpip_port;
	int source_num;
	rds_events_t event_mask;
	string file_name;
	bool append_mode;
	void show_version();
};
