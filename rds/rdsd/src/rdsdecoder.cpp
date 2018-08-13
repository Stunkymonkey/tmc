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
#include "rdsdecoder.h"
#include "rdschanneldata.h"
#include <sstream>
#include <iostream> //test only

namespace std {

RDSdecoder::RDSdecoder()
{
  events = 0;
  utc_datetime_str   = "2000/1/1 00:00:00";
  local_datetime_str = "2000/1/1 00:00:00";
  good_group_counters.resize(33,0);
  bad_group_counters.resize(33,0);
  group_counters_cnt = 0;
  last_pi_code = -2;
  set_pi_code(-1);
}

RDSdecoder::~RDSdecoder()
{

}

void RDSdecoder::SetLogHandler(LogHandler *loghandler)
{
  log = loghandler;
}

void RDSdecoder::LogMsg(LogLevel prio)
{
  if (log) log->LogMsg(prio,logstr.str());
  logstr.str("");
}

void RDSdecoder::LogMsg(LogLevel prio, string msg)
{
  if (log) log->LogMsg(prio,msg);
}

rds_events_t RDSdecoder::GetAllEvents()
{
  return events;
}
  
void RDSdecoder::SetAllEvents(rds_events_t evts)
{
  events = evts;
}

void RDSdecoder::AddEvents(rds_events_t evts)
{
  events |= evts;
}

void RDSdecoder::ClearEvents(rds_events_t evts)
{
  events &= (~evts);
}
 
void RDSdecoder::FreqChanged()
{
  set_event(RDS_EVENT_RX_FREQ);
  set_pi_code(-1);
}

// AddBytes() assumes that the first byte in Buf is the beginning of a block
// and that Buf.size() is a multiple of 3
void RDSdecoder::AddBytes(CharBuf* Buf)
{
  uint i=0;
  int index;
  while (i<Buf->size()){

    group.AddBlock(Buf->at(i),Buf->at(i+1),Buf->at(i+2)); //LSB,MSB,Status

    i+=3;
    
    switch (group.GetGroupStatus()){
      case GS_EMPTY:      group.Clear();
                          continue;
                          break;
      case GS_ERROR:      index = group.GetGroupType();
                          bad_group_counters[index] = bad_group_counters[index]+1;
                          group_counters_cnt++;
                          if (group_counters_cnt>10){
                            set_event(RDS_EVENT_GROUP_STAT);
                            group_counters_cnt = 0;
                            // interesting information even without valid blocks
                            set_event(RDS_EVENT_RX_SIGNAL);
                            set_event(RDS_EVENT_RX_FREQ);
                          }
                          group.Clear();
                          continue;
                          break;
      case GS_INCOMPLETE: continue;
                          break;
      default: break;
    }

    // At this point, we have a complete, valid RDS group.
    index = group.GetGroupType();
    good_group_counters[index] = good_group_counters[index]+1;
    group_counters_cnt++;
    
    if (group_counters_cnt>10){
      set_event(RDS_EVENT_GROUP_STAT);
      group_counters_cnt = 0;

      set_event(RDS_EVENT_RX_SIGNAL);
      set_event(RDS_EVENT_RX_FREQ);
    }
    
    // Each group contains the PI code in block 0:
    // THIS MUST BE THE FIRST ACTION WITH NEW DATA
    // so we can change the curchind
    set_pi_code((group.GetByte(0,1) << 8) | group.GetByte(0,0));

    //some other info common in all groups:
    set_rds_flag(curchind, RDS_FLAG_IS_TP,(group.GetByte(1,1) & 0x04));
    set_pty_code(curchind, ((group.GetByte(1,1) << 3) & 0x18) | ((group.GetByte(1,0) >> 5) & 0x07));

    //The lower 5 bits of block 1 carry special information in most groups:
    block1_lower5 = (group.GetByte(1,0) & 0x1F);

    // The rest is group specific:
    
    switch (group.GetGroupType()){
      case GROUP_0A:
      case GROUP_0B: set_rds_flag(curchind, RDS_FLAG_IS_TA,(block1_lower5 & 0x10));
                     set_rds_flag(curchind, RDS_FLAG_IS_MUSIC,(block1_lower5 & 0x08));
                     index = (block1_lower5 & 0x03) << 1;
                     set_prog_name(curchind, index,group.GetByte(3,1),group.GetByte(3,0));
                     switch (block1_lower5 & 0x03){
		       case 0: set_rds_flag(curchind, RDS_FLAG_IS_DYNAMIC_PTY,(block1_lower5 & 0x04));
			       break;
		       case 1: set_rds_flag(curchind, RDS_FLAG_IS_COMPRESSED,(block1_lower5 & 0x04));
		               break;
		       case 2: set_rds_flag(curchind, RDS_FLAG_IS_ARTIFICIAL_HEAD,(block1_lower5 & 0x04));
			       break;
		       case 3: set_rds_flag(curchind, RDS_FLAG_IS_STEREO,(block1_lower5 & 0x04));
			       break;
		     }
		     if (group.GetGroupType() == GROUP_0A){
                       tmpAFlist.AddGroup(group);
                       switch (tmpAFlist.GetStatus()){
                         case AS_ERROR:    tmpAFlist.Clear();
                                           break;
                         case AS_COMPLETE: chlist[curchind].AFlist = tmpAFlist;
                                           tmpAFlist.Clear();
                                           set_event(RDS_EVENT_AF_LIST);
                                           break;
                         default: ;
                       }
		     }
		     break;
      case GROUP_2A: set_rds_flag(curchind, RDS_FLAG_TEXT_AB,(block1_lower5 & 0x10));
                     chlist[curchind].radio_text.AddGroup(group);
                     set_event(RDS_EVENT_RADIOTEXT);
                     if (chlist[curchind].radio_text.GetStatus() == RT_COMPLETE){
                       set_event(RDS_EVENT_LAST_RADIOTEXT);
                       chlist[curchind].radio_text.Clear();
                     }
                     break;
      case GROUP_2B: set_rds_flag(curchind, RDS_FLAG_TEXT_AB,(block1_lower5 & 0x10));
                     set_pi_code((group.GetByte(2,1) << 8) | group.GetByte(2,0));
                     chlist[curchind].radio_text.AddGroup(group);
                     set_event(RDS_EVENT_RADIOTEXT);
                     if (chlist[curchind].radio_text.GetStatus() == RT_COMPLETE){
                       set_event(RDS_EVENT_LAST_RADIOTEXT);
                       chlist[curchind].radio_text.Clear();
                     }
                     break;
      case GROUP_3A: break; //TODO: implement this
      case GROUP_4A: jul_date =   ((block1_lower5 & 0x03) << 15)
                                | (group.GetByte(2,1) << 7) | (group.GetByte(2,0) >> 1);
		     utc_hour = (group.GetByte(2,0) & 0x01) << 4;

		     utc_hour |= ((group.GetByte(3,1) & 0xF0) >> 4);
		     utc_minute = ((group.GetByte(3,1) & 0x0F) << 2)
		                 |((group.GetByte(3,0) & 0xC0) >> 6);
	             utc_offset = (group.GetByte(3,0) & 0x1F);
		     if (group.GetByte(3,0) & 0x20) utc_offset = -utc_offset;
		     set_datetime_strings();
                     break;
      case GROUP_8A: chlist[curchind].tmc_list.AddGroup(group);
                     if (chlist[curchind].tmc_list.IsChanged()) set_event(RDS_EVENT_TMC);
		     break;
      case GROUP_14A: 
      case GROUP_14B: {
                        unsigned int vtype = (group.GetByte(1, 0) & 0x0F);
                        int PIO = (group.GetByte(3,1) << 8 | group.GetByte(3,0));
                        int ochind = lookup_pi_code(PIO);
                        if (ochind < 0)
                          ochind = add_pi_code(PIO);

                        {
                          logstr << "Variant type: " << hex << vtype << " ";
                          logstr << "PIT : 0x" << hex << (group.GetByte(0,1) << 8 | group.GetByte(0,0)) << ", ";
                          logstr << "PIO : 0x" << hex << (group.GetByte(3,1) << 8 | group.GetByte(3,0)) << ", ";
                          logstr << "Oind: " << ochind;
                          LogMsg(LL_DEBUG);
                        }
                        set_rds_flag(ochind, RDS_FLAG_IS_TP, (group.GetByte(2, 0) & 0x10) );

                        if (group.GetGroupType()== GROUP_14A) {
                          switch (vtype) {
                            case 0x00:
                            case 0x01:
                            case 0x02:
                            case 0x03: // Other network program name
                              set_prog_name(ochind, vtype << 1, group.GetByte(2,1), group.GetByte(2,0));
                              break;
                            case 0x04: // AF type A information
                              tmpEONAFlist.AddGroup(group);
                              LogMsg(LL_DEBUG);

                              switch (tmpEONAFlist.GetStatus()){
                                case AS_ERROR:    tmpEONAFlist.Clear();
                                                  break;
                                case AS_COMPLETE: chlist[ochind].AFlist = tmpEONAFlist;
                                                  tmpEONAFlist.Clear();
                                                  break;
                                default: ;
                              }
                              logstr << "EONAFA: " << tmpEONAFlist.AsString();
                              LogMsg(LL_DEBUG);
                              break;
                            case 0x05:
                            case 0x06:
                            case 0x07:
                            case 0x08:
                            case 0x09: // AF linkage information
                              chlist[ochind].AFlist.AddGroup(group);
                              logstr << "EONAFB: " << chlist[ochind].AFlist.AsString();
                              LogMsg(LL_DEBUG);

                              break;
                            case 0x0a:break; // Unallocated
                            case 0x0b:break; // Unallocated
                            case 0x0c:break; // Linkage information 
                            case 0x0d: 
                                      set_pty_code(ochind, group.GetByte(3, 1) >> 3);
                                      set_rds_flag(ochind, RDS_FLAG_IS_TA, group.GetByte(3, 0) & 0x01);
                                      break; 
                            case 0x0e:break; // PIN
                            case 0x0f:break; // Reserved for broadcast use
                          }
                        } else { // GROUP_14B
                          set_rds_flag(ochind, RDS_FLAG_IS_TA, group.GetByte(2, 0) & 0x08);
                        }
                      }
                     break;
      default: break;
    } //switch
    group.Clear();
  } //while
}

rds_flags_t RDSdecoder::GetRDSFlags()
{
  return chlist[curchind].GetRDSFlags();
}

const string& RDSdecoder::GetRadioText()
{
  return chlist[curchind].GetRadioText();
}

const string& RDSdecoder::GetLastRadioText()
{
  return chlist[curchind].GetLastRadioText();
}

const string& RDSdecoder::GetProgramName()
{
  return chlist[curchind].GetProgramName();
}

const string& RDSdecoder::GetUTCDateTimeString()
{
  return utc_datetime_str;
}

const string& RDSdecoder::GetLocalDateTimeString()
{
  return local_datetime_str;
}

int RDSdecoder::GetPIcode()
{
  return chlist[curchind].GetPIcode();
}

int RDSdecoder::GetPTYcode()
{
  return chlist[curchind].GetPTYcode();
}

const string& RDSdecoder::GetGroupStatistics()
{
  ostringstream oss;
  
  for (int i=0; i<32; i++){
    oss << (i/2);
    if ((i & 1)==0) oss << "A "; else oss << "B ";
    oss << good_group_counters[i] << " ";
    oss << bad_group_counters[i] << endl;
  }

  oss << "??? " << good_group_counters[32] << " ";
  oss << bad_group_counters[32] << endl;
  group_stat_data = oss.str();
  return group_stat_data;
}

const string& RDSdecoder::GetAltFreqList()
{
  return chlist[curchind].GetAltFreqList();
}

const string& RDSdecoder::GetTMCList()
{
  return chlist[curchind].GetTMCList();
}

void RDSdecoder::set_event(rds_events_t evnt)
{
  events |= evnt;
}

void RDSdecoder::set_rds_flag(int channelindex, rds_flags_t flag, bool new_state)
{
  chlist[channelindex].set_rds_flag(flag, new_state);
  set_event(RDS_EVENT_FLAGS);
}

int RDSdecoder::lookup_pi_code(int pi_code)
{
  vector<RDSchanneldata>::iterator channel;
  int chind = 0;
  for (channel = chlist.begin(); channel != chlist.end(); channel++) {
    if (channel->PIcode == pi_code) break;
    chind++;
  }
  if (channel == chlist.end()) {
    return -1;
  } else {
    return chind;
  }
}

int RDSdecoder::add_pi_code(int pi_code) 
{
  int chind = chlist.size(); // one element after current last
  RDSchanneldata newchannel(pi_code);
  chlist.push_back(newchannel); // add channel
  return chind;
}

void RDSdecoder::set_pi_code(int new_code)
{
  if (last_pi_code != new_code) {
    // clear running AF lists
    tmpAFlist.Clear();
    tmpEONAFlist.Clear();
    curchind = lookup_pi_code(new_code);
    if (curchind < 0) {
      // new channel
      curchind = add_pi_code(new_code);
    } 
    
    logstr << "new pi code detected: " << new_code << " ";
    set_event(
      RDS_EVENT_RX_FREQ|
      RDS_EVENT_PI_CODE|
      RDS_EVENT_PROGRAMNAME|
      RDS_EVENT_RX_SIGNAL|
      RDS_EVENT_RADIOTEXT|
      RDS_EVENT_LAST_RADIOTEXT|
      RDS_EVENT_PTY_CODE
      );
    logstr << "channel index: " << curchind;
    LogMsg(LL_DEBUG);
  }
  last_pi_code = new_code;
  set_event(RDS_EVENT_PI_CODE);
}

void RDSdecoder::set_pty_code(int channelindex, int new_code)
{
  chlist[channelindex].set_pty_code(new_code);
  set_event(RDS_EVENT_PTY_CODE);
}

void RDSdecoder::set_prog_name(int channelindex, int first_index, char c1, char c2)
{
  chlist[channelindex].set_prog_name(first_index, c1, c2);
  if (first_index == 6) {
    if (channelindex == curchind)
      set_event(RDS_EVENT_PROGRAMNAME); // maybe we need a RDS_EON_EVENT_PROGRAMNAM...
    
    
    logstr << chlist[channelindex].GetPIcode() << ": " << chlist[channelindex].program_name << " (";
    for (int i=0; i<8; i++){
      logstr.setf(ios::hex, ios::basefield);
      logstr << (int)chlist[channelindex].program_name[i] << " ";
    }
    logstr << ")";
    LogMsg(LL_DEBUG);
  }
}

void RDSdecoder::set_datetime_strings()
{
  const int month_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int Y,M,D,K;
  Y = (int)(((double)jul_date - 15078.2)/365.25);
  M = (int)(((jul_date - 14956.1)-(int)(Y*365.25))/30.6001);
  D = jul_date - 14956 - (int)(Y*365.25) - (int)(M * 30.6001);
  if ((M == 14)||(M == 15)) K=1; else K=0;
  Y = Y + K + 1900;
  M = M - 1 - (K*12);
  ostringstream utcss;

  utcss << Y << "/" << D << "/" << M << " ";
  utcss.width(2); utcss.fill('0');
  utcss << utc_hour; 
  utcss << ":";
  utcss.width(2); utcss.fill('0');
  utcss << utc_minute;
  utcss << ":00";

  utc_datetime_str = utcss.str();

  int loc_hour = utc_hour; 
  int loc_min  = utc_minute + (utc_offset*30);

  while (loc_min < 0){
    loc_min += 60;
    --loc_hour;
  }
  while (loc_min >= 60){
    loc_min -= 60;
    ++loc_hour;
  }
  if (loc_hour < 0){
    loc_hour += 24;
    --D;
    if (D<1){
      --M;
      if (M<1){
        M=12;
	--Y;
      }
      D = month_days[M-1];
      if (((Y % 4)==0)&&(M==2)) D=29;
    }
  }
  if (loc_hour >= 24){
    loc_hour -= 24;
    ++D;
    int Dmax = (((Y % 4)==0)&&(M==2)) ? 29 : month_days[M-1];
    if (D > Dmax){
      ++M;
      if (M > 12){
        M = 1;
	++Y;
      }
    }
  }

  ostringstream locss;
  
  locss << Y << "/" << D << "/" << M << " ";
  locss.width(2); locss.fill('0');
  locss << loc_hour;
  locss << ":";
  locss.width(2); locss.fill('0');
  locss << loc_min;
  locss << ":00";

  local_datetime_str = locss.str();
  
  //cout << "Local date/time: " << local_datetime_str << endl;

  set_event(RDS_EVENT_UTCDATETIME);
  set_event(RDS_EVENT_LOCDATETIME);
}

};
