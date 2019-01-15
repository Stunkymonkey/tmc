#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/program_options.hpp>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>

#include "tmcioptions.h"

namespace po = boost::program_options;
using namespace std;

TmciOptions::TmciOptions():
	conn_type(CONN_TYPE_UNIX),
	server_name("/var/tmp/rdsd.sock"),
	tcpip_port(4321),
	source_num(0),
	event_mask(RDS_EVENT_TMC),
	file_name(""),
	init(false),
	drop_additional_data(false),
	psql_host("127.0.0.1"),
	psql_port(5432),
	psql_database("tmc"),
	psql_user("tmc"),
	psql_password("")
{
}

TmciOptions::~TmciOptions()
{
}

bool TmciOptions::ProcessCmdLine(int argc, char *argv[])
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
			("initialize,i", "for initializing the databases")
			("dropgf,d", "drop additional event data for using less space")
			("postgresql-server,S", po::value<string>()->default_value("127.0.0.1"), "IP of PostgreSQL-server")
			("postgresql-port,P", po::value<int>()->default_value(5432), "Port of PostgreSQL")
			("postgresql-database,D", po::value<string>()->default_value("tmc"), "PostgreSQL database-name")
			("postgresql-user,U", po::value<string>()->default_value("tmc"), "PostgreSQL-User")
			("postgresql-password,K", po::value<string>()->default_value(""), "Password of PostgreSQL-User");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") || ((!vm.count("server") && vm.count("port")))) {
			std::cout << desc << '\n';
			exit(0);
		} else if (vm.count("version")) {
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
		init = vm.count("initialize");
		drop_additional_data = vm.count("dropgf");

		if (vm.count("postgre-server")) {
			psql_host = vm["postgre-server"].as<string>();
		}
		if (vm.count("postgre-port")) {
			psql_port = vm["postgre-port"].as<int>();
		}
		if (vm.count("postgre-database")) {
			psql_database = vm["postgre-database"].as<string>();
		}
		if (vm.count("postgre-user")) {
			psql_user = vm["postgre-user"].as<string>();
		}
		if (vm.count("postgre-password")) {
			psql_password = vm["postgre-password"].as<string>();
		}

	} catch (const po::error &ex) {
		cerr << ex.what() << endl;
		return false;
	}
	return true;
}

void TmciOptions::show_version()
{
	cout << VERSION << endl;
}
