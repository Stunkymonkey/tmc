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
#ifndef STDRADIOTEXT_H
#define STDRADIOTEXT_H

#include <vector>
#include <string>
#include "rdsgroup.h"

namespace std {

/**
Class to handle RDS radio text decoding.

@author Hans J. Koch
*/

enum RTstatus {RT_EMPTY, RT_INCOMPLETE, RT_ERROR, RT_COMPLETE};
enum RTtype {RTT_NONE, RTT_2A, RTT_2B};

class RTchar{
public:
  RTchar() : Char('\0'), Counter(0) {}
  RTchar(const RTchar& other) : Char(other.Char), Counter(other.Counter) {}
  char Char;
  char Counter;
  RTchar& operator=(const RTchar& rhs)
  {
    if (this == &rhs) return *this;
    Char = rhs.Char;
    Counter = rhs.Counter;
    return *this;
  }
};

class RadioText{
public:
  RadioText();
  ~RadioText();
  void Clear();
  void AddGroup(RDSgroup& group);
  RTstatus GetStatus() { return status; }
  const string& GetBuffer();
  const string& GetLastRadioText();
  int GetRequiredCount() { return required_count; }
  void SetRequiredCount(int new_count);
  bool GetABflag() { return ABflag; }
  bool GetUseABflag() { return use_ABflag; }
  void SetUseABflag(bool use_AB) { use_ABflag = use_AB; } 
private:
  vector<RTchar> RTbuffer;
  string buf_string;
  string last_radio_text;
  RTstatus status;
  RTtype type;
  int required_count;
  bool ABflag;
  bool use_ABflag;
  void set_type(RTtype new_type);
  void zero_RTbuffer();
  void set_RTbuffer_char(int pos, int new_char);
  void check_RTbuffer_status();
};

}

#endif
