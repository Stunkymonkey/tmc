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
#ifndef STDRDSDECODER_H
#define STDRDSDECODER_H

#include <vector>
#include <string>
#include <sstream>
#include <librds.h>
#include "rdsgroup.h"
#include "altfreqlist.h"
#include "radiotext.h"
#include "tmclist.h"
#include "rdschanneldata.h"
#include "loghandler.h"

namespace std {

/**
This class is used to decode raw RDS data received from a RDS source.

@author Hans J. Koch
*/

typedef vector<unsigned char> CharBuf;

class RDSchanneldata;

class RDSdecoder{
public:
  RDSdecoder();
  ~RDSdecoder();
  void SetLogHandler(LogHandler *loghandler);
  void LogMsg(LogLevel prio, string msg);
  void LogMsg(LogLevel prio);
  rds_events_t GetAllEvents();
  void SetAllEvents(rds_events_t evts);
  void AddEvents(rds_events_t evts);
  void ClearEvents(rds_events_t evts);
  void FreqChanged();
  void AddBytes(CharBuf* Buf);
  rds_flags_t GetRDSFlags();
  const string& GetRadioText();
  const string& GetLastRadioText();
  const string& GetProgramName();
  const string& GetUTCDateTimeString();
  const string& GetLocalDateTimeString();
  const string& GetGroupStatistics();
  const string& GetAltFreqList();
  const string& GetTMCList();
  int GetPIcode();
  int GetPTYcode();
private:
  LogHandler *log;
  ostringstream logstr;
  
  RDSgroup group;
  vector<unsigned long> good_group_counters;
  vector<unsigned long> bad_group_counters;
  string group_stat_data;
  int group_counters_cnt;
  vector <RDSchanneldata> chlist;
  int curchind;
  rds_events_t events;
  
  string utc_datetime_str;
  string local_datetime_str;
  
  int jul_date;
  int utc_hour;
  int utc_minute;
  int utc_offset;
  
  int last_pi_code;
  AltFreqList tmpAFlist;
  AltFreqList tmpEONAFlist;

  void set_event(rds_events_t evnt);

  void set_rds_flag(int channeldata, rds_flags_t flag, bool new_state);
  void set_pi_code(int new_code);
  void set_pty_code(int channeldata, int new_code);
  void set_prog_name(int channeldata, int first_index, char c1, char c2);
  void set_datetime_strings();

  int lookup_pi_code(int pi_code);
  int add_pi_code(int pi_code);
  int block1_lower5;
};


};

#endif
