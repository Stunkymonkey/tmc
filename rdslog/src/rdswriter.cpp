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

#include "rdswriter.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

RdsWriter::RdsWriter(string filename, bool append)
{
  if (append) {
    myfile.open (filename, ios_base::app);
  } else {
    myfile.open (filename);
  }
  
}

RdsWriter::~RdsWriter()
{
  myfile.close();
}

bool RdsWriter::write(string s) {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%FT%T",timeinfo);
  std::string str_time(buffer);

  if (!myfile.is_open()) return false;
  myfile << "tmc: " << str_time << endl;
  myfile << s << endl;
  return true;
}