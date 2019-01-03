#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <csignal>
#include <sstream>
#include <stdio.h>

#include "tmcioptions.h"
#include "tmcqueryhandler.h"
#include "tmcreader.h"
#include "tmclcl.h"
#include "tmcecl.h"
#include "tmcfilter.h"

using namespace std;

RDSConnectionHandle hnd = 0;
string my_unix_sock = "";

void show_debug(RDSConnectionHandle hnd)
{
	size_t buf_size = 0;
	rds_get_debug_text(hnd, 0, buf_size); // query required size
	if (buf_size > 0) {
		vector<char> buf(buf_size);
		rds_get_debug_text(hnd, &buf[0], buf_size);
		string s(buf.begin(), buf.begin() + buf_size);
		cerr << s << endl;
	}
	else cerr << "(No debug messages)" << endl;
}

void clean_exit(RDSConnectionHandle hnd)
{
	rds_close_connection(hnd);
	rds_delete_connection_object(hnd);
	remove(my_unix_sock.c_str());
	exit(0);
}

static void sig_proc(int signr)
{
	if (signr == SIGINT) {
		cerr << "Caught SIGINT, ";
		if (hnd) {
			cerr << "closing connection, ";
			rds_close_connection(hnd);
			rds_delete_connection_object(hnd);
		}
		cerr << "exiting." << endl;
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sig_proc);

	RdsqOptions opts;
	if (! opts.ProcessCmdLine(argc, argv)) exit(1);


	string psql_host = opts.GetPsqlHost();
	int psql_port = opts.GetPsqlPort();
	string psql_user = opts.GetPsqlUser();
	string psql_password = opts.GetPsqlPassword();
	string psql_database = opts.GetPsqlDatabase();

	TmcData *data = new TmcData(psql_database, psql_user, psql_password, psql_host, std::to_string(psql_port));
	if (!data->checkConnection()) {
		return 42;
	}

	if (opts.GetInitState()) {
		std::string points = "./POINTS.DAT";
		std::string poffset = "./POFFSETS.DAT";
		std::string eventlist = "./EVENTS.DAT";
		// data init database
		TmcLCL *lclImporter = new TmcLCL(points, poffset, data);
		TmcECL *eclImporter = new TmcECL(eventlist, data);
		lclImporter->readPoints();
		cout << "Successfully imported " << points;
		lclImporter->readOffsets();
		cout << " & " << poffset;
		eclImporter->readEventTypes();
		cout << " & " << eventlist << endl;
		delete lclImporter;
		delete eclImporter;
		return 0;
	}

	TmcFilter *manager = new TmcFilter(data);

	TmcReader *reader = new TmcReader(opts.GetFileName());

	string chunk = "";
	int i = 0;
	while (reader->getChunk(chunk)) {
		// std::cout << "processed chunks: " << i << "\r";
		i ++;
		manager->addChunk(chunk);
	}
	delete reader;

	// if there are events unfinished, this will end them.
	std::istringstream last_chunk(chunk);
	std::string last_timestamp;
	std::getline(last_chunk, last_timestamp);
	last_timestamp += "\nend";
	manager->addChunk(last_timestamp);

	// remove for live
	// TODO fix mem-leak in manager
	return 0;

	RdsQueryHandler rds;

	hnd = rds_create_connection_object();
	if (! hnd) {
		cerr << "FATAL ERROR: No connection object, exiting." << endl;
		exit(2);
	}

	rds.SetHandle(hnd);
	rds.SetSourceNum(opts.GetSourceNum());

	int ret = rds_set_debug_params(hnd, RDS_DEBUG_ALL, 500);

	if (ret) rds.ShowError(ret);

	ostringstream oss;
	oss << "/var/tmp/rdsquery" << getpid() << ".sock";
	my_unix_sock = oss.str();

	ret = rds_open_connection(hnd,
	                          opts.GetServerName().c_str(),
	                          opts.GetConnectionType(),
	                          opts.GetPort(),
	                          my_unix_sock.c_str());

	if (ret) {
		rds.ShowError(ret);
		clean_exit(hnd);
	}

	rds_events_t events = opts.GetEventMask();

	ret = rds_set_event_mask(hnd, opts.GetSourceNum(), events);
	if (RDS_OK != ret) {
		rds.ShowError(ret);
		//show_debug(hnd);
		clean_exit(hnd);
	}

	while (RDS_OK == rds_get_event(hnd,opts.GetSourceNum(),&events)){
		if (events & RDS_EVENT_TMC) {
			string ret = rds.ShowTMCList();
			manager->addChunk(ret);
		}
	}

	clean_exit(hnd);

	return EXIT_SUCCESS;
}
