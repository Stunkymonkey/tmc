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

#include "rdsqoptions.h"
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>


namespace std {

RdsqOptions::RdsqOptions()
  : record_count(1), conn_type(CONN_TYPE_UNIX),
    server_name("/var/tmp/rdsd.sock"),
    tcpip_port(4321), source_num(0),
    event_mask(0), freq_to_set(-1.0),
    have_opt_e(false), have_opt_f(false),
    have_opt_s(false), have_opt_p(false),
    have_opt_t(false), have_opt_u(false),
    have_opt_i(false)
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

  while ( (option = getopt(argc,argv,"c:eif:hvn:s:u:t:p:")) != EOF ) {
    switch (option){
      case 'c' :  if (try_str_to_int(optarg,itmp)) record_count=itmp;
                  else {
                    cerr << "Illegal or missing argument for option -c." << endl;
                    show_usage();
                    return false;
                  }
                  break;
      case 'i' :  have_opt_i = true;
                  break;
      case 'e' :  have_opt_e = true;
                  break;
      case 'f' :  have_opt_f = true;
                  if (try_str_to_int(optarg,itmp)) freq_to_set=(double) itmp * 1000.0;
                  else {
                    cerr << "Illegal or missing argument for option -n." << endl;
                    show_usage();
                    return false;
                  }
                  break;
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
      case 't' :  if (try_parse_types(optarg,evnt_tmp)) event_mask=evnt_tmp;
      		  else {
      		    cerr << "Illegal or missing argument for option -t." << endl;
      		    show_usage();
      		    return false;
      		  }
      		  have_opt_t = true;
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
  if ((! have_opt_t)&&(!(have_opt_e||have_opt_f))) { show_usage(); return false; }
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
  cerr << "Record count: " << record_count << endl;
  cerr << "Source: " << source_num << endl;
}


void RdsqOptions::show_usage()
{
  cerr << "Usage:" << endl;
  cerr << "rdsquery [-s|-u <server>] <options>" << endl;
  cerr << "-h : Show this help and exit." << endl;
  cerr << "-v : Show version information and exit." << endl;
  cerr << "-s <TCP/IP-Server>: Address/name of the machine where rdsd is running." << endl;
  cerr << "-p <portnum>: TCP/IP port where rdsd is listening (default 4321)." << endl;
  cerr << "-u <Unix socket>: Socket where rdsd is listening (default /var/tmp/rdsd.sock)" << endl;
  cerr << "-e : Enumerate RDS sources and exit." << endl;
  cerr << "-i : Immediately request data from rdsd instead of waiting for an event." << endl;
  cerr << "-n <srcnum>: Specify the RDS source number (see -e), default 0." << endl;
  cerr << "-f <freqkhz> : Set tuner frequency to freqkhz and exit." << endl;
  cerr << "-t <type1,type2...>: Comma separated list of data types. Valid types are:" << endl;
  cerr << "   rxfre,rxsig,rflags,picode,ptype,pname,locdt,utcdt,rtxt,lrtxt,tmc,aflist,gstat" << endl;
  cerr << "-c <count>: Number of data records that rdsquery should wait for." << endl;
  cerr << "   (default: 1, -c 0 means continous operation, use STRG+C to stop)." << endl;
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

int RdsqOptions::find_cmd_num(const string& S)
{
  int i=1;
  while (i<RDS_CMD_COUNT){
    if (RdsCommands[i]==S) return i;
    i++;
  }
  return 0;
}

bool RdsqOptions::try_parse_types(char *s, rds_events_t &result)
{
  result = 0;
  if (! s) return false;
  string S(s);
  string cmd;
  unsigned int i=0;
  while (i<S.size()){
    if (S[i] != ',') cmd.push_back(S[i]);
    if ((S[i] == ',')||(i==(S.size()-1))) {
      int cmd_num = find_cmd_num(cmd);
      if (cmd_num == RDS_CMD_NONE) return false;
      switch (cmd_num){
        case RDS_CMD_GET_RX_FREQ   : result |= RDS_EVENT_RX_FREQ; break;
        case RDS_CMD_GET_RX_SIGNAL : result |= RDS_EVENT_RX_SIGNAL; break;
        case RDS_CMD_FLAGS         : result |= RDS_EVENT_FLAGS; break;
        case RDS_CMD_PI_CODE       : result |= RDS_EVENT_PI_CODE; break;
        case RDS_CMD_PTY_CODE      : result |= RDS_EVENT_PTY_CODE; break;
        case RDS_CMD_PROGRAMNAME   : result |= RDS_EVENT_PROGRAMNAME; break;
	case RDS_CMD_UTCDATETIME   : result |= RDS_EVENT_UTCDATETIME; break;
        case RDS_CMD_LOCDATETIME   : result |= RDS_EVENT_LOCDATETIME; break;
        case RDS_CMD_RADIOTEXT     : result |= RDS_EVENT_RADIOTEXT; break;
        case RDS_CMD_LAST_RADIOTEXT: result |= RDS_EVENT_LAST_RADIOTEXT; break;
        case RDS_CMD_TMC           : result |= RDS_EVENT_TMC; break;
        case RDS_CMD_ALT_FREQ      : result |= RDS_EVENT_AF_LIST; break;
        case RDS_CMD_GROUP_STAT    : result |= RDS_EVENT_GROUP_STAT; break;
      }
      cmd = "";
    }
    i++;
  }
  return true;
}

}
