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
#ifndef STDALTFREQLIST_H
#define STDALTFREQLIST_H

#include <vector>
#include <string>
#include "rdsgroup.h"

namespace std {

/**
Class to decode alternative frequency lists from 0x0A groups.

@author Hans J. Koch
*/

enum AFListStatus {AS_EMPTY, AS_INCOMPLETE, AS_ERROR, AS_COMPLETE};

class AltFreq{
public:
  AltFreq() :Freq(-1), IsVariant(false), EonTNMapped(0) {}
  AltFreq(const AltFreq& other) : Freq(other.Freq), IsVariant(other.IsVariant), EonTNMapped(other.EonTNMapped) {}
  int Freq;
  int EonTNMapped;
  bool IsVariant;
  AltFreq& operator=(const AltFreq& rhs)
  {
    if (this == &rhs) return *this;
    Freq = rhs.Freq;
    EonTNMapped = rhs.EonTNMapped;
    IsVariant = rhs.IsVariant;
    return *this;
  }
};

class AltFreqList{
public:
  AltFreqList();
  ~AltFreqList();
  void Clear();
  void AddGroup(RDSgroup& group);
  AFListStatus GetStatus() { return status; }
  const string& AsString();
  AltFreqList& operator=(const AltFreqList& rhs)
  {
    if (this == &rhs) return *this;
    list_string = rhs.list_string;
    status = rhs.status;
    first_freq = rhs.first_freq;
    freq_counter = rhs.freq_counter;
    lfmf_follows = rhs.lfmf_follows;
    freq_list = rhs.freq_list;
    return *this;
  }
private:
  string list_string;
  AFListStatus status;
  int first_freq;
  int freq_counter;
  bool lfmf_follows;
  vector<AltFreq> freq_list;
  bool handle_freq_pair(int b0, int b1);
  int freq_in_khz(int b);
  bool add_freq(int freq, bool is_variant, int EONTNfreq);
};


}

#endif
