#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/program_options.hpp>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>

#include "tmcwoptions.h"

namespace po = boost::program_options;
using namespace std;

RdswOptions::RdswOptions():
	threads(1),
	web_port(8081),
	doc_root("../html/"),
	web_ip("127.0.0.1")
{
}

RdswOptions::~RdswOptions() {
}

bool RdswOptions::ProcessCmdLine(int argc, char *argv[]) {
	try
	{
		po::options_description desc{"Options"};
		desc.add_options()
			("help,h", "Help screen")
			("version,v", " Show version information and exit")
			("server,s", po::value<string>(), "IP-Address of the machine where webserver will run")
			("threads,t", po::value<int>(), "amount of threads")
			("port,p", po::value<int>(), "TCP/IP port where webserver will run")
			("doc-root,d", po::value<string>(), "document root, where the html files can be found")
			("postgre-server,", po::value<string>(), "IP of PostgreSQL-server")
			("postgre-port,", po::value<string>(), "Port of PostgreSQL")
			("postgre-user,", po::value<string>(), "PostgreSQL-User")
			("postgre-password", po::value<string>(), "Password of PostgreSQL-User")
			("postgre-database", po::value<string>(), "PostgreSQL database-name");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << '\n';
			exit(0);
		}
		else if (vm.count("version")) {
			show_version();
			exit(0);
		}
		if (vm.count("server")) {
			web_ip = vm["server"].as<string>();
		}
		if (vm.count("threads")) {
			threads = vm["threads"].as<int>();
		}
		if (vm.count("port")) {
			web_port = vm["port"].as<int>();
		}
		if (vm.count("doc-root")) {
			doc_root = vm["doc-root"].as<string>();
		}
	} catch (const po::error &ex) {
		cerr << ex.what() << endl;
		return false;
	}
	return true;
}

void RdswOptions::show_version() {
	cout << VERSION << endl;
}
