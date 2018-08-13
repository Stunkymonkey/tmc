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
#include "altfreqlist.h"

#include <sstream>
#include <iostream> //test only

namespace std {

AltFreqList::AltFreqList()
{
  Clear();
}


AltFreqList::~AltFreqList()
{
}

void AltFreqList::Clear()
{
  freq_list.resize(0);
  status = AS_EMPTY;
  first_freq = -1;
  freq_counter = -1;
  lfmf_follows = false;
}

void AltFreqList::AddGroup(RDSgroup& group)
{
  unsigned int vtype = (group.GetByte(1, 0) & 0x0F);
  if (  (group.GetGroupStatus() != GS_COMPLETE)
      ||((group.GetGroupType() != GROUP_0A)
      && (group.GetGroupType() != GROUP_14A))) return;
  
  int b0 = group.GetByte(2,1); // Byte order OK ???
  int b1 = group.GetByte(2,0);
  
  if ((group.GetGroupType() == GROUP_14A) && vtype != 0x04) { //special case for EON mapped frequencies
    int EONf = freq_in_khz(b0);
    if (vtype == 0x09) lfmf_follows = true;
    int f = freq_in_khz(b1);
    add_freq(f, false, EONf);
    status = AS_COMPLETE; // completeness is never known with this method
  } else {
      switch (status){
          case AS_EMPTY     : if ((b0>=224)&&(b0<=249)){
                                  freq_counter = b0-224;
                                  if (freq_counter==0){
                                      status = AS_COMPLETE;
                                      return;
                                  }
                                  if (b1 == 250){
                                      lfmf_follows=true;
                                      status = AS_INCOMPLETE;
                                      return;
                                  }
                                  if ((b1>=1)&&(b1<=204)){
                                      int f = (100*(b1-1))+87600;
                                      first_freq = f;
                                      add_freq(f,false,0);
                                  }
                                  else status = AS_ERROR;
                              }
                              else status = AS_ERROR;
                              break;
          case AS_INCOMPLETE: handle_freq_pair(b0,b1);
                              break;
          case AS_ERROR     : Clear();
                              break;
          case AS_COMPLETE  : status = AS_ERROR;
                              break;
      }
  }
}

const string& AltFreqList::AsString()
{
  ostringstream oss;
  oss << "AF list, count = " << freq_list.size() << endl; // Test only!
  for (int i=0; i<freq_list.size(); i++){
    if (freq_list[i].IsVariant) oss << "V "; else oss << "S ";
    oss << freq_list[i].EonTNMapped << "->";
    oss << freq_list[i].Freq << endl;
  }
  list_string = oss.str();
  return list_string;
}

bool AltFreqList::handle_freq_pair(int b0, int b1)
{
  int f1 = freq_in_khz(b0);
  if (f1<0) return false;
  int f2 = freq_in_khz(b1);
  if (f2<0) return false;
  if ((f1==first_freq)||(f2==first_freq)){ //Method B
    if (f1==first_freq) add_freq(f2,(f2<f1),0);
    else add_freq(f1,(f2<f1),0);
    if (status == AS_ERROR) return false;
  }
  else{ //Method A
    if (f1>1){
      add_freq(f1,false,0);
      if (status == AS_ERROR) return false;
    }
    if (f2>1){
      add_freq(f2,false,0);
      if (status == AS_ERROR) return false;
    }
  }
  return true;
}

int AltFreqList::freq_in_khz(int b)
{
  if (b == 205) return 0; //filler code
  if (b == 250){     //LF/MF frequency follows
    lfmf_follows = true;
    return 1;
  }
  int result = -1;
  if (lfmf_follows){
    if ((b>=1)&&(b<=15)) result = ((b-1)*9)+153;
    else if ((b>=16)&&(b<=135)) result = ((b-16)*9)+531;
    lfmf_follows = false;
  }
  else{
    if ((b>=1)&&(b<=204)) result = ((b-1)*100)+87600;
  }
  return result;
}

bool AltFreqList::add_freq(int freq, bool is_variant, int EONTNfreq)
{
  if ( (freq_counter > 0) || (EONTNfreq > 0) ) {
    AltFreq AF;
    vector<AltFreq>::iterator iter;
    for (iter = freq_list.begin(); iter < freq_list.end(); iter++)
      if ( ( iter->Freq == freq ) && ( iter->EonTNMapped == EONTNfreq ) ) {
        break;
      }
    if (iter == freq_list.end()) {
      AF.Freq = freq;
      AF.IsVariant = is_variant;
      AF.EonTNMapped = EONTNfreq;
      freq_list.push_back(AF);
      freq_counter--;
      if (freq_counter==0) status = AS_COMPLETE;
      else status = AS_INCOMPLETE;
    }
    return true;
  }
  status = AS_ERROR;
  return false;
}


}
