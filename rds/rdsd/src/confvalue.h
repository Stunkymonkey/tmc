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
#ifndef STDCONFVALUE_H
#define STDCONFVALUE_H

#include <string>

namespace std {

/**
Class to store one value from a configuration file. The value is initially a string. 
The class provides methods to convert to/from other data types.

@author Hans J. Koch
*/
class ConfValue{
public:
    ConfValue();
    ~ConfValue();
    bool IsSet();
    const string& GetName();
    void SetName(string newname);
    
    const string& GetString(bool &valid);
    int GetInt(bool &valid);
    double GetDouble(bool &valid);
    bool GetBool(bool &valid);
    
    string GetStringDef(string Default);
    int GetIntDef(int Default);
    double GetDoubleDef(double Default);
    bool GetBoolDef(bool Default);
    
    void SetString(string newvalue);
    void SetInt(int newvalue);
    void SetDouble(double newvalue);
    void SetBool(bool newvalue);
private:
    string varname;
    string valstr;
    bool is_set;
    string int2string(int num);
    string dbl2string(double num);
    string bool2string(bool val);
    int string2int(string sval, bool &valid);
    double string2double(string sval, bool &valid);
    bool string2bool(string sval, bool &valid);
};

};

#endif
