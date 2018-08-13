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
#ifndef STDCONFFILE_H
#define STDCONFFILE_H

#include <vector>
#include "confvalue.h"
#include "confsection.h"
#include <iostream>
#include <string>

namespace std {

/**
Class to read and write configuration files. The files are similar to *.ini files but can handle two or more sections with the same name. 

@author Hans J. Koch
*/

class ConfFile{
public:
    ConfFile();
    ~ConfFile();
    int LoadConf(string filename);
    int GetSectionCount();
    ConfSection* GetSection(int index);
    const string& GetErrStr() { return err_str; }
private:
    vector<ConfSection*> sections;
    void clear_sections();
    char next_char(ifstream& fs);
    bool is_valid_char(char ch);
    int parse(string filename);
    char eol_char;
    int line_num;
    string err_str;
};

};

#endif
