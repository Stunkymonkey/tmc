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
#include "radiotext.h"

namespace std {

RadioText::RadioText()
{
  Clear();
  ABflag = false;
  use_ABflag = true;
  required_count = 1;
}


RadioText::~RadioText()
{
}

void RadioText::Clear()
{
  status = RT_EMPTY;
  type = RTT_NONE;
}

void RadioText::AddGroup(RDSgroup& group)
{
  if (group.GetGroupStatus() != GS_COMPLETE) return;

  int addr_code = group.GetByte(1,0) & 0x0F;
  bool new_ABflag = ((group.GetByte(1,0) & 0x10) != 0);
  if (ABflag != new_ABflag){
    ABflag = new_ABflag;
    if (use_ABflag) zero_RTbuffer();
  }
  switch (group.GetGroupType()){
    case GROUP_2A: set_type(RTT_2A);
                   set_RTbuffer_char(4*addr_code + 0, group.GetByte(2,1));
                   set_RTbuffer_char(4*addr_code + 1, group.GetByte(2,0));
                   set_RTbuffer_char(4*addr_code + 2, group.GetByte(3,1));
                   set_RTbuffer_char(4*addr_code + 3, group.GetByte(3,0));
                   check_RTbuffer_status();
                   break;
    case GROUP_2B: set_type(RTT_2B);
                   set_RTbuffer_char(2*addr_code + 0, group.GetByte(3,1));
                   set_RTbuffer_char(2*addr_code + 1, group.GetByte(3,0));
                   check_RTbuffer_status();
                   break;
    default:       Clear(); //should not happen...
  }
}

const string& RadioText::GetBuffer()
{
  buf_string = "";
  for (unsigned int i = 0; i<RTbuffer.size(); i++){
    if (RTbuffer[i].Counter == 0) buf_string.push_back(' ');
    else{
      if (RTbuffer[i].Char == '\r') break;
      buf_string.push_back(RTbuffer[i].Char);
    }
  }
  return buf_string;
}

const string& RadioText::GetLastRadioText()
{
  return last_radio_text;
}

void RadioText::SetRequiredCount(int new_count)
{
  required_count = new_count;
  if (required_count < 1) required_count = 1;
}

void RadioText::set_type(RTtype new_type)
{
  if (type != new_type){
    type = new_type;
    switch (type){
      case RTT_NONE: Clear();
                     break;
      case RTT_2A:   RTbuffer.resize(64);
                     zero_RTbuffer();
                     break;
      case RTT_2B:   RTbuffer.resize(32);
                     zero_RTbuffer();
                     break;
    }
  }
}

void RadioText::zero_RTbuffer()
{
  for (int i=0; i<RTbuffer.size(); i++){
    RTbuffer[i].Char = '\0';
    RTbuffer[i].Counter = 0;
  }
  status = RT_EMPTY;
}

void RadioText::set_RTbuffer_char(int pos, int new_char)
{
  if (RTbuffer[pos].Counter == 0) RTbuffer[pos].Counter = 1;
  else{
    if (RTbuffer[pos].Char == new_char) RTbuffer[pos].Counter++;
    else RTbuffer[pos].Counter = 1;
  }
  RTbuffer[pos].Char = new_char;
  if (status != RT_COMPLETE) status = RT_INCOMPLETE;
}

void RadioText::check_RTbuffer_status()
{
  bool found_one = false;
  bool is_complete = true;
  for (int i=0; i<RTbuffer.size(); i++){
    if (RTbuffer[i].Counter >= required_count) found_one = true;
    if (RTbuffer[i].Char == '\r') break;
    if (RTbuffer[i].Counter < required_count) is_complete = false;
  }
  if (found_one && is_complete){
    status = RT_COMPLETE;
    last_radio_text = "";
    for (int i=0; i<RTbuffer.size(); i++){
      if (RTbuffer[i].Char == '\r') break;
      if (RTbuffer[i].Counter < required_count) break;
      last_radio_text.push_back(RTbuffer[i].Char);
    }
  }
}

}
