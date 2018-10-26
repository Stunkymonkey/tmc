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

#include "tmcreader.h"

using namespace std;

TmcReader::TmcReader(string filename)
{
  cout << filename << endl;
  // Open the file
  boost::iostreams::file_source myCprdFile (filename, std::ios_base::in | std::ios_base::binary);

  // Uncompress the file with the BZ2 library and its Boost wrapper
  boost::iostreams::filtering_istream bunzip2Filter;
  bunzip2Filter.push (boost::iostreams::bzip2_decompressor());
  bunzip2Filter.push (myCprdFile);

  cout << bunzip2Filter.good() << endl;
  string itReadLine;
  getline(bunzip2Filter, itReadLine);
  cout << itReadLine << std::endl;
}

TmcReader::~TmcReader()
{
  //myfile.close();
}

bool TmcReader::read(string &s) {
  cout << bunzip2Filter.good() << endl;
  if(getline(bunzip2Filter, s)) {
    cout << s << endl;
    return true;
  }
  cout << s << endl;
  return false;
}