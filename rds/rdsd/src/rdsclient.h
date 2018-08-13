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
#ifndef STDRDSCLIENT_H
#define STDRDSCLIENT_H

#include <vector>
#include "rdssource.h"
#include "loghandler.h"
#include "rdsdecoder.h"

namespace std {

/**
This class handles requests by clients (via Unix domain socket or TCP/IP). 
It reads the request and sends the answer. One object of this  class is created 
for each client file descriptor. 

@author Hans J. Koch
*/
class RDSclient{
public:
  RDSclient();
  ~RDSclient();
  void SetLogHandler(LogHandler *loghandler);
  void SetSrcList(RDSsourceList* psrclist);
  int GetFd();
  void SetFd(int NewFd);
  void Close();
  int CheckEvents();
  int Process();
private:
  int fd;
  string cmd;
  string text_to_send;
  LogHandler *log;
  RDSsourceList* srclist;
  vector<rds_events_t> event_masks;
  int ExecCmd();
  void split_cmd(int& src_num, string& cmd_str, long& param, bool& par_valid);
  bool send_text();
  rds_events_t get_event_mask(int src_num);
  void LogMsg(LogLevel prio, string msg);
};

typedef vector<RDSclient*> RDSclientList;

};

#endif
