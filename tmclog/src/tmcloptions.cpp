#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/program_options.hpp>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>

#include "tmcloptions.h"

namespace po = boost::program_options;
using namespace std;

RdslOptions::RdslOptions():
	conn_type(CONN_TYPE_UNIX),
	server_name("/var/tmp/rdsd.sock"),
	tcpip_port(4321),
	source_num(0),
	event_mask(RDS_EVENT_TMC),
	file_name("test.tmc"),
	append_mode(true)
{
}

RdslOptions::~RdslOptions()
{
}

bool RdslOptions::ProcessCmdLine(int argc, char *argv[])
{
	try
	{
		po::options_description desc{"Options"};
		desc.add_options()
		("help,h", "Help screen")
		("version,v", "Show version information and exit")
		("number,n", po::value<int>()->default_value(0), "Specify the RDS source number")
		("server,s", po::value<string>()->default_value("127.0.0.1"), "Address/name of the machine where rdsd is running")
		("port,p", po::value<int>()->default_value(4321), "TCP/IP port where rdsd is listening")
		("unix-socket,u", po::value<string>()->default_value("/var/tmp/rdsd.sock"), "Socket where rdsd is listening")
		("file,f", po::value<string>()->default_value("test.tmc"), "specify file name to read from")
		("clean,c", "clean file before writing");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") || (vm.count("unix-socket") || vm.count("server") || vm.count("port"))) {
			std::cout << desc << '\n';
			exit(0);
		}
		else if (vm.count("version")) {
			show_version();
			exit(0);
		}

		if (vm.count("server")) {
			server_name = vm["server"].as<string>();
			conn_type = CONN_TYPE_TCPIP;
		}
		if (vm.count("port")) {
			tcpip_port = vm["port"].as<int>();
		}
		if (vm.count("unix-socket")) {
			server_name = optarg;
			conn_type = CONN_TYPE_UNIX;
		}
		if (vm.count("number")) {
			source_num = vm["number"].as<int>();
		}

		if (vm.count("filename")) {
			file_name = vm["filename"].as<string>();
		}
		append_mode = !vm.count("clean");

	} catch (const po::error &ex) {
		cerr << ex.what() << endl;
		return false;
	}
	return true;
}

void RdslOptions::show_version()
{
	cout << VERSION << endl;
}
