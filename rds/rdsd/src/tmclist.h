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
#ifndef STDTMCLIST_H
#define STDTMCLIST_H

#include <string>
#include <ctime>
#include <list>
#include "rdsgroup.h"


namespace std {

/**
Class to decode TMC messages.

@author Hans J. Koch
*/

enum TMCtype {TMC_GROUP=0, TMC_SINGLE, TMC_SYSTEM, TMC_TUNING};

class TMCinfo{
public:
  TMCinfo() : rx_time(0) {}
  TMCinfo(const TMCinfo& other) : rx_time(other.rx_time), data(other.data) {}
  time_t rx_time;
  string data;
  TMCinfo& operator=(const TMCinfo& rhs)
  {
    if (this == &rhs) return *this;
    rx_time = rhs.rx_time;
    data = rhs.data;
    return *this;
  }
};

class TMClist{
public:
  TMClist();
  ~TMClist();
  void AddGroup(RDSgroup& group);
  const string& AsString();
  bool IsChanged() { return is_changed; }
  time_t GetTimeToLive() { return time_to_live; }
  void SetTimeToLive(time_t TTL);
private:
  bool is_changed;
  time_t time_to_live;
  string tmc_provider;
  string list_string;
  list<TMCinfo> tmc_list;
  bool add_string(const string& tmc_string);
  bool check_timeouts();
};

}

#endif
