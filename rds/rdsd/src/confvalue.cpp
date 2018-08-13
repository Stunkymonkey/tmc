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
#include "confvalue.h"
#include <sstream>
#include <cstdlib>

namespace std {

ConfValue::ConfValue()
{
  is_set=false;
}


ConfValue::~ConfValue()
{
}

const string& ConfValue::GetName()
{
  return varname;
}

void ConfValue::SetName(string newname)
{
  varname = newname;
}

bool ConfValue::IsSet()
{
  return is_set;
}

const string& ConfValue::GetString(bool &valid)
{
  valid = IsSet();
  return valstr;
}

int ConfValue::GetInt(bool &valid)
{
  valid = false;
  if (! IsSet()) return 0;
  return string2int(valstr,valid);
}

double ConfValue::GetDouble(bool &valid)
{
  valid = false;
  if (! IsSet()) return 0;
  return string2double(valstr,valid);
}

bool ConfValue::GetBool(bool &valid)
{
  valid = false;
  if (! IsSet()) return 0;
  return string2bool(valstr,valid);
}

string ConfValue::GetStringDef(string Default){
  bool valid;
  string result = GetString(valid);
  if (valid) return result; else return Default;
}

int ConfValue::GetIntDef(int Default){
  bool valid;
  int result = GetInt(valid);
  if (valid) return result; else return Default;
}

double ConfValue::GetDoubleDef(double Default){
  bool valid;
  double result = GetDouble(valid);
  if (valid) return result; else return Default;
}

bool ConfValue::GetBoolDef(bool Default){
  bool valid;
  bool result = GetBool(valid);
  if (valid) return result; else return Default;
}


void ConfValue::SetString(string newvalue)
{
  valstr = newvalue;
  is_set = true;
}

void ConfValue::SetInt(int newvalue)
{
  valstr = int2string(newvalue);
  is_set = true;
}

void ConfValue::SetDouble(double newvalue)
{
  valstr = dbl2string(newvalue);
  is_set = true;
}

void ConfValue::SetBool(bool newvalue)
{
  valstr = bool2string(newvalue);
  is_set = true;
}

string ConfValue::int2string(int num){
  ostringstream oss;
  oss << num;
  return oss.str();
}

string ConfValue::dbl2string(double num){
  ostringstream oss;
  oss << num;
  return oss.str();
}

string ConfValue::bool2string(bool val){
  if (val) return "yes"; else return "no";
}

int ConfValue::string2int(string sval, bool &valid){
  istringstream iss(sval);
  int result;
  if (iss >> result) {
    valid = true;
    return result;
  }
  return 0;
}
    
double ConfValue::string2double(string sval, bool &valid){
  istringstream iss(sval);
  double result;
  if (iss >> result) {
    valid = true;
    return result;
  }
  return 0.0;
}

bool ConfValue::string2bool(string sval, bool &valid){
  valid=false;
  if (  (sval=="yes") || (sval=="YES")
      ||(sval=="y")||(sval=="Y")
      ||(sval=="1")||(sval=="true")||(sval=="TRUE") ) {
    valid = true;
    return true;    
  }
  if (  (sval=="no") || (sval=="NO")
      ||(sval=="n")||(sval=="N")
      ||(sval=="0")||(sval=="false")||(sval=="FALSE") ) {
    valid = true;
    return false;    
  }
  return false;
}

};
