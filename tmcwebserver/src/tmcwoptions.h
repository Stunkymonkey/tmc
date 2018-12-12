#pragma once
#include <string>

using namespace std;

class RdswOptions{
public:
	RdswOptions();
	~RdswOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	int GetThreads() { return threads; }
	int GetPort() { return web_port; }
	const string& GetServerName() { return web_ip; }
	const string& GetDocRoot() { return doc_root; }
private:
	int threads;
	int web_port;
	string web_ip;
	string doc_root;
	void show_usage();
	void show_version();
};
