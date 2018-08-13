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
#include "tmclist.h"
#include <sstream>


namespace std {

TMClist::TMClist()
{
  is_changed = false;
  time_to_live = 600; // 10 minutes
  tmc_provider.resize(8,' ');
}


TMClist::~TMClist()
{
}

void TMClist::AddGroup(RDSgroup& group)
{
  if (group.GetGroupStatus() != GS_COMPLETE) return;

  TMCtype type = (TMCtype)((group.GetByte(1,0) & 0x18) >> 3);
  int duration = group.GetByte(1,0) & 0x07;
  int CI = duration;
  int event = group.GetWord(2) & 0x7FF;
  int location = group.GetWord(3);
  int extent = (group.GetByte(2,1) & 0x38) >> 3;
  int direction = (group.GetByte(2,1) & 0x40) >> 6;
  int SGI = direction;
  int diversion = (group.GetByte(2,1) & 0x80) >> 7;
  int GSI = (group.GetByte(2,1) & 0x30) >> 4;
  int frequency = ((group.GetWord(2) & 0xFFF) << 16)+group.GetWord(3);
  ostringstream oss;
  int i;
  switch (type){
    case TMC_GROUP:  if ((CI >= 1) & (CI <= 6)) {
                       /* CI=0 is for encrypted TMCpro */
                       /* CI=7 is reserved for future use */
                       if (diversion) {
                         /* first group */
                         oss << "GF evt=" << event << " loc=" << location;
                         oss << " ext=" << extent << " CI=" << CI;
                         oss << " dir=" << direction;
                       } else {
                         /* subsequent groups */
                         oss << "GS CI=" << CI << " GSI=" << GSI;
                         oss << " F1=" << event << " F2=" << location; // Free Format
                       }
                     break;
    case TMC_SINGLE: oss << "S evt=" << event << " loc=" << location;
                     oss << " ext=" << extent << " dur=" << duration;
                     oss << " dir=" << direction << " div=" << diversion;
                     break;
    case TMC_SYSTEM: oss << "Y ";
                     switch (CI){
                       case 0:
                       case 1:
                       case 2:
                       case 3: oss << "CI=" << CI << " ";
                               for (i=1; i<=3; ++i){
                                 oss.setf(ios::hex,ios::basefield);
                                 oss.width(4);
                                 oss.fill('0');
                                 oss << group.GetWord(i) << " ";
                               }
                               break;
                       case 4: tmc_provider[0]=group.GetByte(2,1);
                               tmc_provider[1]=group.GetByte(2,0);
                               tmc_provider[2]=group.GetByte(3,1);
                               tmc_provider[3]=group.GetByte(3,0);
                               oss << "provider=" << tmc_provider;
                               break;
                       case 5: tmc_provider[4]=group.GetByte(2,1);
                               tmc_provider[5]=group.GetByte(2,0);
                               tmc_provider[6]=group.GetByte(3,1);
                               tmc_provider[7]=group.GetByte(3,0);
                               oss << "provider=" << tmc_provider;
                               break;
                       case 6:
                       case 7:;
                     }
                     break;
    case TMC_TUNING: oss << "T ";
                     switch (CI){
                       case 0:
                       case 1:
                       case 2:
                       case 3:
                       case 4:
                       case 5:
                       case 6:
                       case 7: oss << "CI=" << CI << " ";
                               for (i=1; i<=3; ++i){
                                 oss.setf(ios::hex,ios::basefield);
                                 oss.width(4);
                                 oss.fill('0');
                                 oss << group.GetWord(i) << " ";
                               }
                     }
                     break;
  }
  if (add_string(oss.str())) is_changed = true;
  if (check_timeouts()) is_changed = true;
}

const string& TMClist::AsString()
{
  ostringstream oss;
  list<TMCinfo>::iterator it;
  for (it=tmc_list.begin(); it != tmc_list.end(); ++it){
    oss << (*it).data << endl;
  }
  list_string = oss.str();
  return list_string;
}

void TMClist::SetTimeToLive(time_t TTL)
{
  if (time_to_live != TTL){
    time_to_live = TTL;
    if (check_timeouts()) is_changed = true;
  }
}

bool TMClist::add_string(const string& tmc_string)
{
  bool result = true;
  
  list<TMCinfo>::iterator it;
  for (it=tmc_list.begin(); it != tmc_list.end(); ++it){
    if ((*it).data == tmc_string){
      (*it).rx_time = time(0);
      result = false;
      break;
    }
  }
  
  if (result){
    TMCinfo info;
    info.rx_time = time(0);
    info.data = tmc_string;
    tmc_list.push_back(info);
  }
  return result;
}

bool TMClist::check_timeouts()
{
  bool result = false;
  
  time_t time_now = time(0);
  list<TMCinfo>::iterator it,it1,itend;
  it = --tmc_list.end();
  itend = --tmc_list.begin();
  while (it != itend){
    TMCinfo info = *it;
    if ((time_now - info.rx_time)>time_to_live){
      it1=it;
      --it1;
      tmc_list.erase(it);
      it = it1;
      result = true;
    }
    else --it;
  }
  return result;
}


}
