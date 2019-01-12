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

TmclOptions::TmclOptions():
	conn_type(CONN_TYPE_UNIX),
	server_name("/var/tmp/rdsd.sock"),
	tcpip_port(4321),
	source_num(0),
	event_mask(RDS_EVENT_TMC),
	file_name(""),
	append_mode(true)
{
}

TmclOptions::~TmclOptions()
{
}

bool TmclOptions::ProcessCmdLine(int argc, char *argv[])
{
	try
	{
		po::options_description desc{"Options"};
		desc.add_options()
			("help,h", "Help screen")
			("version,v", "Show version information and exit")
			("number,n", po::value<int>()->default_value(0), "Specify the RDS source number")
			("server,s", po::value<string>(), "Address/name of the machine where rdsd is running")
			("port,p", po::value<int>(), "TCP/IP port where rdsd is listening")
			("unix-socket,u", po::value<string>(), "Socket where rdsd is listening")
			("filename,f", po::value<string>(), "specify file name to read from")
			("clean,c", "clean file before writing (default appending)");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") || (!vm.count("server") || vm.count("port"))) {
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
		} else {
			cerr << "no file-path given" << endl;
			exit(0);
		}
		append_mode = !vm.count("clean");

	} catch (const po::error &ex) {
		cerr << ex.what() << endl;
		return false;
	}
	return true;
}

void TmclOptions::show_version()
{
	cout << VERSION << endl;
}
