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
#include "rdsqueryhandler.h"
#include <iostream>
#include <cstdlib>

namespace std {

RdsQueryHandler::RdsQueryHandler()
{
  handle = 0;
  src_num = -1;
  show_debug_on_error = true;
  event_counts.resize(sizeof(rds_events_t)*8,0);
  infinite_record_count = false;
}


RdsQueryHandler::~RdsQueryHandler()
{
}

void RdsQueryHandler::InitRecordCounters(rds_events_t events, int count)
{
  infinite_record_count = (count == 0);
  int i = 0;
  rds_events_t mask = 1;
  int bit_count = sizeof(rds_events_t)*8;
  while (i<bit_count){
    if (events & mask) event_counts[i]=count; else event_counts[i]=0;
    mask = (mask << 1);
    ++i;
  }
}

bool RdsQueryHandler::DecRecordCounters(rds_events_t events)
{
  if (infinite_record_count) return true;
  int i = 0;
  rds_events_t mask = 1;
  int bit_count = sizeof(rds_events_t)*8;
  bool result = false;
  while (i<bit_count){
    if (events & mask){
      if (event_counts[i]>0) event_counts[i] = event_counts[i]-1;
    }
    if (event_counts[i]>0) result = true;
    mask = (mask << 1);
    ++i;
  }
  return result;
}

void RdsQueryHandler::ShowError(int rds_err_num)
{
  if ((rds_err_num>=0)&&(rds_err_num<=RDS_ILLEGAL_TIMEOUT))
    cout << RdsErrorStrings[rds_err_num] << endl;
  else
    cout << "Error #" << rds_err_num << endl;
  if (show_debug_on_error) show_debug();
}

void RdsQueryHandler::ShowEnumSrc()
{
  const size_t buf_size = 2048;
  vector<char> buf(buf_size);
  int ret = rds_enum_sources(handle,&buf[0],buf_size);
  cout << "esrc:";
  if (ret) ShowError(ret);
  else {
    string s(&buf[0]);
    cout << s << endl;
  }
}

void RdsQueryHandler::ShowFlags()
{
  rds_flags_t flags;
  int ret = rds_get_flags(handle,src_num, &flags);
  cout << "rflags:";
  if (ret) ShowError(ret);
  else {
    if (flags & RDS_FLAG_IS_TP) cout << "TP=1 "; else cout << "TP=0 ";
    if (flags & RDS_FLAG_IS_TA) cout << "TA=1 "; else cout << "TA=0 ";
    if (flags & RDS_FLAG_IS_MUSIC) cout << "MUSIC=1 "; else cout << "MUSIC=0 ";
    if (flags & RDS_FLAG_IS_STEREO) cout << "STEREO=1 "; else cout << "STEREO=0 ";
    if (flags & RDS_FLAG_IS_ARTIFICIAL_HEAD) cout << "AH=1 "; else cout << "AH=0 ";
    if (flags & RDS_FLAG_IS_COMPRESSED) cout << "COMP=1 "; else cout << "COMP=0 ";
    if (flags & RDS_FLAG_IS_DYNAMIC_PTY) cout << "DPTY=1 "; else cout << "DPTY=0 ";
    if (flags & RDS_FLAG_TEXT_AB) cout << "AB=1"; else cout << "AB=0";
    cout << endl;
  }
}

void RdsQueryHandler::ShowPIcode()
{
  int result;
  int ret = rds_get_pi_code(handle,src_num, &result);
  cout << "picode:";
  if (ret) ShowError(ret);
  else cout << result << endl;
}

void RdsQueryHandler::ShowPTYcode()
{
  int result;
  int ret = rds_get_pty_code(handle,src_num, &result);
  cout << "ptype:";
  if (ret) ShowError(ret);
  else cout << result << endl;
}

void RdsQueryHandler::ShowProgramName()
{
  char buf[9];
  int ret = rds_get_program_name(handle,src_num,buf);
  buf[8] = 0;
  cout << "pname:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowRadioText()
{
  char buf[65];
  int ret = rds_get_radiotext(handle,src_num,buf);
  buf[64] = 0;
  cout << "rtxt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowLastRadioText()
{
  char buf[65];
  int ret = rds_get_last_radiotext(handle,src_num,buf);
  buf[64] = 0;
  cout << "lrtxt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}


void RdsQueryHandler::ShowLocalDateTime()
{
  char buf[256];
  int ret = rds_get_local_datetime_string(handle,src_num,buf);
  buf[255] = 0;
  cout << "locdt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowUTCdateTime()
{
  char buf[256];
  int ret = rds_get_utc_datetime_string(handle,src_num,buf);
  buf[255] = 0;
  cout << "utcdt:";
  if (ret) ShowError(ret);
  else {
    string result(buf);
    cout << result << endl;
  }
}

void RdsQueryHandler::ShowGroupStatistics()
{
  cout << "gstat:" << endl;
  size_t bufsize=0;
  int ret = rds_get_group_stat_buffer(handle,src_num,0, &bufsize);
  if (ret){
    ShowError(ret);
    return;
  }
  if (bufsize>0){
    vector<char> buf(bufsize);
    ret = rds_get_group_stat_buffer(handle,src_num,&buf[0], &bufsize);
    if (ret){
      ShowError(ret);
      return;
    }
    string s(buf.begin(),buf.begin()+bufsize);
    cout << s << endl;
  }
}

void RdsQueryHandler::ShowAltFreqList()
{
  cout << "aflist:" << endl;
  size_t bufsize=0;
  int ret = rds_get_af_buffer(handle,src_num,0, &bufsize);
  if (ret){
    ShowError(ret);
    return;
  }
  if (bufsize>0){
    vector<char> buf(bufsize);
    ret = rds_get_af_buffer(handle,src_num,&buf[0], &bufsize);
    if (ret){
      ShowError(ret);
      return;
    }
    string s(buf.begin(),buf.begin()+bufsize);
    cout << s << endl;
  }
}

void RdsQueryHandler::ShowTMCList()
{
  cout << "tmc:" << endl;
  size_t bufsize=0;
  int ret = rds_get_tmc_buffer(handle,src_num,0, &bufsize);
  if (ret){
    ShowError(ret);
    return;
  }
  if (bufsize>0){
    vector<char> buf(bufsize);
    ret = rds_get_tmc_buffer(handle,src_num,&buf[0], &bufsize);
    if (ret){
      ShowError(ret);
      return;
    }
    string s(buf.begin(),buf.begin()+bufsize);
    cout << s << endl;
  }
}

void RdsQueryHandler::ShowTunerFrequency()
{
  cout << "rxfre:";
  double freq;
  int ret = rds_get_rx_frequency(handle,src_num, &freq);
  if (ret){
    ShowError(ret);
    return;
  }
  int freq_khz = (int)(freq/1000.0 + 0.5);
  cout << freq_khz << endl;
}

void RdsQueryHandler::ShowSetTunerFrequency(double FreqToSet)
{
  cout << "srxfre:";
  int ret = rds_set_rx_frequency(handle,src_num,FreqToSet);
  if (ret){
    ShowError(ret);
    return;
  }
  int freq_khz = (int)(FreqToSet/1000.0 + 0.5);
  cout << freq_khz << endl;
}

void RdsQueryHandler::ShowSignalStrength()
{
  cout << "rxsig:";
  int strength;
  int ret = rds_get_rx_signal_strength(handle,src_num, &strength);
  if (ret){
    ShowError(ret);
    return;
  }
  cout << strength << endl;
}

void RdsQueryHandler::show_debug()
{
  size_t buf_size = 0;
  int ret = rds_get_debug_text(handle,0, &buf_size); // query required size
  if (ret){
    ShowError(ret);
    return;
  }
  if (buf_size>0){
    vector<char> buf(buf_size);
    rds_get_debug_text(handle,&buf[0], &buf_size);
    string s(buf.begin(),buf.begin()+buf_size);
    cerr << s << endl;
  }
  else cerr << "(No debug messages)" << endl;
}


}
