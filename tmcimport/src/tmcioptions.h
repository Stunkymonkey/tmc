#ifndef STDRDSQOPTIONS_H
#define STDRDSQOPTIONS_H

#include <librds.h>
#include <string>

using namespace std;

class RdsqOptions{
public:
	RdsqOptions();
	~RdsqOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	void ShowOptions();
	int GetConnectionType() { return conn_type; }
	const string& GetServerName() { return server_name; }
	int GetPort() { return tcpip_port; }
	int GetSourceNum() { return source_num; }
	rds_events_t GetEventMask() { return event_mask; }
	const string& GetFileName() { return file_name; }
private:
	int conn_type;
	string server_name;
	int tcpip_port;
	int source_num;
	rds_events_t event_mask;
	bool have_opt_s;
	bool have_opt_p;
	bool have_opt_u;
	string file_name;
	void show_usage();
	void show_version();
	bool try_str_to_int(char *s, int &result);
};

#endif
