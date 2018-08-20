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

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <csignal>
#include <sstream>
#include <stdio.h>
#include "rdsloptions.h"
#include "rdsqueryhandler.h"
#include "rdswriter.h"


//#include <dlfcn.h>

using namespace std;

RDSConnectionHandle hnd = 0;
string my_unix_sock = "";

void show_debug(RDSConnectionHandle hnd)
{
  size_t buf_size = 0;
  rds_get_debug_text(hnd,0,buf_size); // query required size
  if (buf_size>0){
    vector<char> buf(buf_size);
    rds_get_debug_text(hnd,&buf[0],buf_size);
    string s(buf.begin(),buf.begin()+buf_size);
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
  if (signr == SIGINT){
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
  signal(SIGINT,sig_proc);

  RdsqOptions opts;
  if (! opts.ProcessCmdLine(argc,argv)) exit(1);
  
  RdsQueryHandler rds;
  
  hnd = rds_create_connection_object();
  if (! hnd){
    cerr << "FATAL ERROR: No connection object, exiting." << endl;
    exit(2);
  }

  rds.SetHandle(hnd);
  rds.SetSourceNum(opts.GetSourceNum());
 
  int ret = rds_set_debug_params(hnd,RDS_DEBUG_ALL,500);

  if (ret) rds.ShowError(ret);

  ostringstream oss;
  oss << "/var/tmp/rdsquery" << getpid() << ".sock";
  my_unix_sock = oss.str();

  ret = rds_open_connection(hnd,
                            opts.GetServerName().c_str(),
                            opts.GetConnectionType(),
                            opts.GetPort(),
                            my_unix_sock.c_str());

  if (ret){
    rds.ShowError(ret);
    clean_exit(hnd);
  }
  
  rds_events_t events = opts.GetEventMask();

  ret = rds_set_event_mask(hnd,opts.GetSourceNum(),events);
  if (RDS_OK != ret){
    rds.ShowError(ret);
    //show_debug(hnd);
    clean_exit(hnd);
  }

  RdsWriter *writer = new RdsWriter("example.txt", true);

  while (RDS_OK == rds_get_event(hnd,opts.GetSourceNum(),&events)){
    if (events & RDS_EVENT_TMC) {
      string ret = rds.ShowTMCList();
      //cout << ret << endl;
      if (!writer->write(ret)) {
        cerr << "Unable to write to file." << endl;
      }
      //delete ret;
    }
  }

  clean_exit(hnd);
  
  return EXIT_SUCCESS;
}
