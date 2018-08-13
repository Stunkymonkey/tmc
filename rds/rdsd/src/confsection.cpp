/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   koch@hjk-az.de                                                *
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
#include "confsection.h"

namespace std {

ConfSection::ConfSection()
{
}


ConfSection::~ConfSection()
{
  clear_values();
}

string ConfSection::GetName()
{
  return sectname;
}

void ConfSection::SetName(string newname)
{
  sectname = newname;
}

int ConfSection::AddValue(string name, string value)
{
  ConfValue* cv = new ConfValue;
  cv->SetName(name);
  cv->SetString(value);
  values.push_back(cv);
  return values.size()-1;
}

int ConfSection::GetValueCount()
{
  return values.size();
}

ConfValue* ConfSection::GetValue(int index)
{
  if ((index>=0)&&((unsigned int)index<values.size())) return values[index];
      else return 0; 
}

void ConfSection::clear_values()
{
  int i = values.size();
  while (i>=0) {
    ConfValue* cv = GetValue(i);
    if (cv) delete cv;
    i--;
  }
  values.clear();
}

};
