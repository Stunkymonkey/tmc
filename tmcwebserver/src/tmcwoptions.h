#pragma once
#include <string>

using namespace std;

class TmcwOptions{
public:
	TmcwOptions();
	~TmcwOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	const string& GetServerName() { return web_ip; }
	int GetPort() { return web_port; }
	int GetThreads() { return threads; }
	const string& GetDocRoot() { return doc_root; }
	const string& GetFile() { return file; }
private:
	string web_ip;
	int web_port;
	int threads;
	string doc_root;
	string file;
	void show_usage();
	void show_version();
};
