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

TmcwOptions::TmcwOptions():
	web_ip("127.0.0.1"),
	web_port(8081),
	threads(1),
	doc_root("../html/"),
	file("")
{
}

TmcwOptions::~TmcwOptions() {
}

bool TmcwOptions::ProcessCmdLine(int argc, char *argv[]) {
	try
	{
		po::options_description desc{"Options"};
		desc.add_options()
			("help,h", "Help screen")
			("version,v", "Show version information and exit")
			("server,s", po::value<string>(), "IP-Address of the machine where webserver will run")
			("threads,t", po::value<int>(), "amount of threads")
			("port,p", po::value<int>(), "TCP/IP port where webserver will run")
			("doc-root,d", po::value<string>(), "document root, where the html files can be found")
			("file,f", po::value<string>(), "generated data file");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") || !vm.count("file")) {
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
		if (vm.count("file")) {
			file = vm["file"].as<string>();
		}
		
	} catch (const po::error &ex) {
		cerr << ex.what() << endl;
		return false;
	}
	return true;
}

void TmcwOptions::show_version() {
	cout << VERSION << endl;
}
