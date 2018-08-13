/*****************************************************************************
 *   This file is part of librds, a library to fetch data from the           *
 *   Radio Data System Daemon (rdsd). See http://rdsd.berlios.de             * 
 *   Copyright (C) 2005 by Hans J. Koch                                      *
 *   hjkoch@users.berlios.de                                                 *
 *                                                                           *
 *   This library is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public              *
 *   License as published by the Free Software Foundation; either            *
 *   version 2.1 of the License, or (at your option) any later version.      *
 *                                                                           * 
 *   This library is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *   Lesser General Public License for more details.                         *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *    
 *   License along with this library; if not, write to the Free Software     *
 *   Foundation,Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA *
 *****************************************************************************/
 
#ifndef STDRDSCONNECTION_H
#define STDRDSCONNECTION_H

#include "librds.h"
#include "rdsdcommandlist.h"
#include <string>
#include <vector>

namespace std {

/**
Class used in librds. For each client connection to rdsd, one object of this class will be created. A pointer to this object will be returned to the application as a handle to use this connection.

@author Hans J. Koch
*/
class RDSconnection{
public:
  //! The constructor.
  RDSconnection();
  //! The destructor.
  ~RDSconnection();
  //! Set timeout in milliseconds
  int SetTimeoutTime(unsigned int milliseconds);
  //! Set library debugging parameters
  int SetDebugParams(int debug_level, unsigned int max_lines);
  //! Get debug text buffer
  int GetDebugTextBuffer(char* buf, size_t& buf_size);
  //! Open a connection to rdsd.
  int Open(const char* serv_path, int conn_type, int port, const char* my_path);
  //! Close the connection.
  int Close();
  //! Get the list of sources known by rdsd.
  int EnumSources(char* buf, size_t bufsize);
  //! Set the evnt mask for a source.
  int SetEventMask(unsigned int src, rds_events_t evnt_mask);
  //! Get the active event mask for a source.
  int GetEventMask(unsigned int src, rds_events_t &evnt_mask);
  //! Get the events that occured since the last call.
  int GetEvent(unsigned int src, rds_events_t &events);
  //! Get some RDS flags from a source.
  int GetFlags(unsigned int src, rds_flags_t &flags);
  //! Get the program type code.
  int GetPTYcode(unsigned int src, int &pty_code);
  //! Get the program identification code.
  int GetPIcode(unsigned int src, int &pi_code);
  //! Get the program name.
  int GetProgramName(unsigned int src, char* buf);
  //! Get the current radio text buffer.
  int GetRadiotext(unsigned int src, char* buf);
  //! Get the last complete radio text string.
  int GetLastRadiotext(unsigned int src, char* buf);
  //! Get UTC date and time as a string.
  int GetUTCDateTimeString(unsigned int src, char* buf);
  //! Get local date and time as a string.
  int GetLocalDateTimeString(unsigned int src, char* buf);
  //! Get the TMC message buffer.
  int GetTMCBuffer(unsigned int src, char* buf, size_t& buf_size);
  //! Get the alternative frequency list buffer.
  int GetAltFreqBuffer(unsigned int src, char* buf, size_t& buf_size);
  //! Get the RDS group statistics buffer.
  int GetGroupStatisticsBuffer(unsigned int src, char* buf, size_t& buf_size);
  //! Get the tuner frequency of a V4L radio device.
  int GetTunerFrequency(unsigned int src, double& freq);
  //! Set the tuner frequency of a V4L radio device.
  int SetTunerFrequency(unsigned int src, double freq);
  //! Get the receiver signal strength of a V4L radio device.
  int GetTunerSignalStrength(unsigned int src, int& strength);
private:
  RdsdCommandList CmdList;
  int sock_fd;
  vector<char> read_buf;
  string read_str;
  int last_scan_state;
  vector<rds_events_t> rcvd_events;
  vector<string> debug_msg_buf;
  int active_debug_level;
  unsigned int first_debug_line;
  unsigned int next_debug_line;
  unsigned int max_debug_lines;
  unsigned long timeout_time_msec;
  string tmc_data;
  bool have_tmc_data;
  string aflist_data;
  bool have_aflist_data;
  string group_stat_data;
  bool have_group_stat_data;
  unsigned long get_millisec_time();
  void debug_msg(int debug_level, const string& msg);
  int open_tcpip(const char* path, int port);
  int open_unix(const char* serv_path, const char* my_path);
  int send_command(int src, const string& cmd, const string& opt_param);
  int wait_for_data(int src, const string& cmd, string& data);
  int process();
  bool StringToEvnt(const string &s, rds_events_t &evnt);
  bool StringToFlags(const string &s, rds_flags_t &flags);
  bool StringToInt(const string &s, int &result);
  bool StringToSrcNum(const string &s, unsigned int &src_num);
  bool process_msg();
  bool process_event_msg();
};

};

#endif
