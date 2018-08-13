/***************************************************************************
 *   Copyright (C) 2005 by Daniel J.W. Lindenaar                           *
 *   daniel-rdsd@lindenaar.eu                                              *
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

#include "rdschanneldata.h"

namespace std {
  
RDSchanneldata::RDSchanneldata(int newPIcode):
program_name("        "),
PIcode(newPIcode),
PTYcode(-1)
{

}

RDSchanneldata::~RDSchanneldata()
{

}

rds_flags_t RDSchanneldata::GetRDSFlags()
{
  return rds_flags;
}

const string& RDSchanneldata::GetRadioText()
{
  return radio_text.GetBuffer();
}

const string& RDSchanneldata::GetLastRadioText()
{
  return radio_text.GetLastRadioText();
}

const string& RDSchanneldata::GetProgramName()
{
  return program_name;
}

int RDSchanneldata::GetPIcode()
{
  return PIcode;
}

int RDSchanneldata::GetPTYcode()
{
  return PTYcode;
}

const string& RDSchanneldata::GetAltFreqList()
{
  return AFlist.AsString();
}

const string& RDSchanneldata::GetTMCList()
{
  return tmc_list.AsString();
}

void RDSchanneldata::set_rds_flag(rds_flags_t flag, bool new_state)
{
  if (new_state) rds_flags |= flag;
  else rds_flags &= (!flag);
}

void RDSchanneldata::set_pty_code(int new_code)
{
  PTYcode = new_code;
}

void RDSchanneldata::set_prog_name(int first_index, char c1, char c2)
{
  program_name[first_index]   = c1;
  program_name[first_index+1] = c2;
}

};
