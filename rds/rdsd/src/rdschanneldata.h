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
#ifndef STDRDSCHANNELDATA_H
#define STDRDSCHANNELDATA_H

#include <vector>
#include <string>
#include <librds.h>
#include "rdsgroup.h"
#include "altfreqlist.h"
#include "radiotext.h"
#include "tmclist.h"
#include "rdsdecoder.h"

namespace std {

class RDSchanneldata{
public:
  RDSchanneldata(int newPIcode);
  ~RDSchanneldata();
  friend class RDSdecoder;
  rds_flags_t GetRDSFlags();
  const string& GetRadioText();
  const string& GetLastRadioText();
  const string& GetProgramName();
  const string& GetAltFreqList();
  const string& GetTMCList();
  int GetPIcode();
  int GetPTYcode();

private:
  AltFreqList EONAFlist;
  int EonPIcode;
  AltFreqList AFlist;
  RadioText radio_text;
  TMClist tmc_list;
  string program_name;
  int PIcode;
  int PTYcode;
  rds_flags_t rds_flags;

  void set_rds_flag(rds_flags_t flag, bool new_state);
  void set_pty_code(int new_code);
  void set_prog_name(int first_index, char c1, char c2);

};

};

#endif
