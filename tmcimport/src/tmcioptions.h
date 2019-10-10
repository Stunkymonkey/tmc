#pragma once
#include <librds.h>
#include <string>

using namespace std;

class TmciOptions{
public:
	TmciOptions();
	~TmciOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	int GetConnectionType() { return conn_type; }
	const string& GetServerName() { return server_name; }
	int GetPort() { return tcpip_port; }
	int GetSourceNum() { return source_num; }
	rds_events_t GetEventMask() { return event_mask; }
	const string& GetFileName() { return file_name; }
	const string& GetDataFile() { return data_file; }
	const bool& DropGFData() { return drop_additional_data; }
	const bool& AddDuplicateEvents() { return add_duplicate_events; }
private:
	int conn_type;
	string server_name;
	int tcpip_port;
	int source_num;
	rds_events_t event_mask;
	string file_name;
	string data_file;
	bool init;
	bool drop_additional_data;
	bool add_duplicate_events;
	void show_version();
};
