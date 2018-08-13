/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   hjkoch@users.berlios.de                                                *
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
#ifndef STDRDSHANDLER_H
#define STDRDSHANDLER_H

#include "conffile.h"
#include "rdssource.h"
#include "rdsclient.h"
#include "loghandler.h"
#include "rdsd_errors.h"
#include <sys/select.h> 

namespace std {

/**
Main control class of rdsd (Radio Data System demon).

@author Hans J. Koch
*/
class RDShandler{
public:
  RDShandler();
  ~RDShandler();
  int InitConf(string conf_file_name);
  int Init();
  int WorkLoop();
  void Terminate();
  LogHandler log;
  const string& GetPidFilename();
private:
  ConfFile conf;
  RDSsourceList srclist;
  RDSclientList clientlist;
  void clear_srclist();
  void clear_clientlist();
  int init_sources();
  int init_global();
  int init_server();
  int open_sources();
  void accept_unix_client();
  void accept_inet_client();
  void delete_client(RDSclient* cli);
  void calc_maxfd();
  string pid_file_name;
  string unix_sock_name;
  int tcpip_port;
  int unix_sock_fd;
  int tcpip_sock_fd;
  bool terminated;
  fd_set all_fds;
  int maxfd;
};

};

#endif
