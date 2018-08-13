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
#include "conffile.h"
#include <fstream>
#include <sstream>

namespace std {

ConfFile::ConfFile()
{
  eol_char = 0;
}

ConfFile::~ConfFile()
{
  clear_sections();
}

int ConfFile::GetSectionCount()
{
  return sections.size();
}

ConfSection* ConfFile::GetSection(int index)
{
  if ((index>=0)&&((unsigned int)index<sections.size())) return sections[index];
      else return 0; 
}

int ConfFile::LoadConf(string filename)
{
  clear_sections();
  return parse(filename);
}


void ConfFile::clear_sections()
{
  int i = sections.size();
  while (i>=0) {
    ConfSection* cs = GetSection(i);
    if (cs) delete cs;
    --i;
  }
  sections.clear();
}

char ConfFile::next_char(ifstream& fs)
{
  char ch;
  bool ready=false;
  while (!ready){
    ready=true;
    if (fs.get(ch)){
      if ((ch==0x0a)||(ch==0x0d)){
        if (eol_char==0) eol_char=ch;
        if (ch==eol_char) ++line_num; else ready=false;
      }
    }
    else ch=0;
  }
  return ch;
}

bool ConfFile::is_valid_char(char ch)
{
  if ((ch>='A')&&(ch<='Z')) return true;
  if ((ch>='a')&&(ch<='z')) return true;
  if ((ch>='0')&&(ch<='9')) return true;
  if (ch=='_') return true;
  if ((ch=='+')||(ch=='-')) return true;
  if ((ch==',')||(ch=='.')) return true;
  return false;
}

int ConfFile::parse(string filename)
{
  enum CFScanState {CF_ERROR,CF_RESET,CF_SECTNAME,CF_COMMENT,CF_VARNAME,CF_VALUE,
                    CF_STRING,CF_WAITSECT,CF_WAITEQ,CF_WAITVAL,CF_READY};
  err_str = "";
  ifstream infile(filename.c_str());
  if (!infile) return -1;
  CFScanState state = CF_RESET;
  line_num=1;
  string sectname; 
  string varname;
  string valuestr;
  ConfSection* cursect=0;
  while (char ch = next_char(infile)){
    switch (ch) {
      case 0x0a:line_num++; // Fall through...
      case 0x0d:switch (state) {
                  case CF_READY:
                  case CF_RESET:
                  case CF_COMMENT: state=CF_RESET; 
		                   break;
                  case CF_VALUE:   state=CF_READY;
		                   break;
		  default: err_str = "Unexpected end of line.";
		           state=CF_ERROR;
                } 
                break;
      case 0x09:
      case ' ': switch (state) {
                  case CF_VARNAME: state=CF_WAITEQ; 
		                   break;
                  case CF_VALUE:   state=CF_READY;
		                   break; 
		  case CF_STRING:  valuestr+=ch;
		                   break;
		  default: ;  
                }
                break;
      case '#': switch (state) {
                  case CF_READY:
                  case CF_RESET:   state=CF_COMMENT; 
		                   break;
		  case CF_STRING: valuestr+=ch;
		                  break;
		  default: err_str = "Char '#' not valid here.";
		           state=CF_ERROR;  
                }
                break;     
      case '=': switch (state) {
                  case CF_VARNAME:
                  case CF_WAITEQ: state=CF_WAITVAL;
		                  valuestr="";
				  break;
		  case CF_STRING: valuestr+=ch;
		                  break;
		  case CF_COMMENT: break;
		  default: err_str = "Char '=' not valid here.";
		  	   state=CF_ERROR;	  
                }
                break;
      case '"': switch (state) {
                  case CF_WAITVAL: state=CF_STRING;
		                   valuestr="";
				   break;
		  case CF_STRING:  state=CF_READY;
		                   break;
		  case CF_COMMENT: break;
		  default: err_str = "Char '\"' not valid here.";
		           state=CF_ERROR;	  
                }
                break;
      case '[': switch (state) {
                  case CF_RESET:  state=CF_WAITSECT;
		                  sectname="";
				  cursect=0;
				  break;
		  case CF_STRING: valuestr+=ch;
		                  break;
		  case CF_COMMENT: break;
		  default: err_str = "Char '[' not valid here.";
		           state=CF_ERROR;
                }
                break;
      case ']': switch (state) {
                  case CF_SECTNAME: state=CF_RESET;
		                    if (sectname.empty()) state=CF_ERROR;
				    else {
				      cursect = new ConfSection;
				      cursect->SetName(sectname);
				      sections.push_back(cursect);
				      sectname="";
				    }
				    break;
		  case CF_STRING:   valuestr+=ch;
		                    break;
		  case CF_COMMENT:  break;
		  default: err_str = "Char ']' not valid here.";
		           state=CF_ERROR;	  
                }
                break;
      default:  if (state==CF_STRING) valuestr+=ch;
                else {
		  if (is_valid_char(ch)){
		    switch(state) {
		      case CF_WAITSECT: sectname=ch;
		                        state=CF_SECTNAME;
					break;
		      case CF_WAITVAL:  valuestr=ch;
		                        state=CF_VALUE;
					break;
		      case CF_SECTNAME: sectname+=ch;
		                        break;
		      case CF_RESET:    varname=ch;
		                        state=CF_VARNAME;
					break;
		      case CF_VARNAME:  varname+=ch;
		                        break;
		      case CF_VALUE: 
		      case CF_STRING:   valuestr+=ch;
		                        break;
		      case CF_WAITEQ:   err_str = "'=' expected.";
		                        state=CF_ERROR;
		                        break;
		      default: ;
		    }   
		  } 
		  else if (state != CF_COMMENT){
		    err_str = "Illegal char found.";
		    state=CF_ERROR;
		  }
		}
    } //switch (ch)...
    if (state==CF_READY){
      if (cursect) {
        cursect->AddValue(varname,valuestr);
        varname="";
        valuestr="";
        state=CF_RESET;
      }
      else{
        err_str = "Section header missing.";
        state=CF_ERROR;
      }
    }
    if (state==CF_ERROR){
      ostringstream oss;
      oss << "(Line " << line_num << "): " << err_str;
      err_str = oss.str();
      break;
    }
  } //while
  if (state==CF_ERROR) return -2;
  return 0;
}

};
