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
#include "rdsdcommandlist.h"

namespace std {

RdsdCommandList::RdsdCommandList()
{
}


RdsdCommandList::~RdsdCommandList()
{
}

void RdsdCommandList::Clear()
{
  
  RdsdCommandMap::iterator it;
  for (it=CmdMap.begin(); it != CmdMap.end(); it++){
    delete (it->second);
  }
  CmdMap.clear();
}

RdsdCommand* RdsdCommandList::Find(const string& CmdStr)
{
  RdsdCommandMap::iterator it = CmdMap.find(CmdStr);
  if (it == CmdMap.end()) return 0;
  return it->second;
}

RdsdCommand* RdsdCommandList::FindOrAdd(const string& CmdStr)
{
  RdsdCommand* result = Find(CmdStr);
  if (! result){
    result = new RdsdCommand;
    CmdMap[CmdStr] = result;
  }
  return result;
}


};
