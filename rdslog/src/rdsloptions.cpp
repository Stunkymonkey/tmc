/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   hjkoch@users.berlios.de                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdsloptions.h"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>


namespace std {

RdsqOptions::RdsqOptions()
  : conn_type(CONN_TYPE_UNIX),
    server_name("/var/tmp/rdsd.sock"),
    tcpip_port(4321), source_num(0),
    event_mask(RDS_EVENT_TMC), have_opt_s(false),
    have_opt_p(false), have_opt_u(false),
    have_opt_c(false),
    file_name("example.txt")
{
}

RdsqOptions::~RdsqOptions()
{
}

bool RdsqOptions::ProcessCmdLine(int argc, char *argv[])
{
  char option;
  int itmp;
  rds_events_t evnt_tmp;

  while ( (option = getopt(argc,argv,"hvn:s:u:p:c:f:")) != EOF ) {
    switch (option){
      case 'n' :  if (try_str_to_int(optarg,itmp)) source_num=itmp;
                  else {
                    cerr << "Illegal or missing argument for option -n." << endl;
                    show_usage();
                    return false;
                  }
                  break;
      case 's' :  if (have_opt_u){ show_usage(); return false; }
                  server_name = optarg;
                  conn_type = CONN_TYPE_TCPIP;
                  have_opt_s = true;
                  break;
      case 'p' :  if (have_opt_u){ show_usage(); return false; }
                  if (try_str_to_int(optarg,itmp)) tcpip_port=itmp;
                  else {
                    cerr << "Illegal or missing argument for option -p." << endl;
                    show_usage();
                    return false;
                  }
                  have_opt_p = true;
                  break;
      case 'u' :  if (have_opt_s){ show_usage(); return false; }
                  if (have_opt_p){ show_usage(); return false; }
                  server_name = optarg;
                  conn_type = CONN_TYPE_UNIX;
                  have_opt_u = true;
                  break;
      case 'c' :  have_opt_c = true;
                  break;
      case 'f' :  file_name = optarg;
                  have_opt_s = true;
                  break;
      case 'h' :  show_usage();
                  exit(0);
                  break;
      case 'v' :  show_version();
                  exit(0);
                  break;
      default  :  cerr << "Unknown option -" << option << endl;
                  show_usage();
                  return false;
    }
  }
  if ((have_opt_p)&&(!have_opt_s)){ show_usage(); return false; }
  return true;
}

void RdsqOptions::ShowOptions()
{
  cerr << "Mode: ";
  if (conn_type == CONN_TYPE_TCPIP) cerr << "TCP/IP";
  else if (conn_type == CONN_TYPE_UNIX) cerr << "UNIX";
  else cerr << "???";
  cerr << endl;
  cerr << "Server: " << server_name << endl;
  if (conn_type == CONN_TYPE_TCPIP) cerr << "Port: " << tcpip_port << endl;
  cerr << "Source: " << source_num << endl;
}


void RdsqOptions::show_usage()
{
  cerr << "Usage:" << endl;
  cerr << "query-logger [-s|-u <server>] <options>" << endl;
  cerr << "-h : Show this help and exit." << endl;
  cerr << "-v : Show version information and exit." << endl;
  cerr << "-s <TCP/IP-Server>: Address/name of the machine where rdsd is running." << endl;
  cerr << "-p <portnum>: TCP/IP port where rdsd is listening (default 4321)." << endl;
  cerr << "-u <Unix socket>: Socket where rdsd is listening (default /var/tmp/rdsd.sock)" << endl;
  cerr << "-n <srcnum>: Specify the RDS source number (see -e), default 0." << endl;
  cerr << "-c clear file and write in it. Default is append-mode" << endl;
  cerr << "-f specify file name to write in." << endl;
}

void RdsqOptions::show_version()
{
  cout << VERSION << endl;
}

bool RdsqOptions::try_str_to_int(char *s, int &result)
{
  if (! s) return false;
  istringstream iss(s);
  if (iss >> result) return true;
  return false;
}

}
