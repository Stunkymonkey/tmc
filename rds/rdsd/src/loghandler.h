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
#ifndef STDLOGHANDLER_H
#define STDLOGHANDLER_H

#include <string>

namespace std {

/**
Handles log messages.

@author Hans J. Koch
*/

enum LogLevel{LL_EMERG,LL_CRIT,LL_ERR,LL_WARN,LL_INFO,LL_DEBUG};


class LogHandler{
public:
  LogHandler();
  ~LogHandler();
  LogLevel GetLogLevel();
  void SetLogLevel(LogLevel NewLL);
  int SetLogFilename(string filename);
  bool GetLogFileValid();
  string GetLogFilename();
  bool GetConsoleLog();
  void SetConsoleLog(bool DoConsoleLog);
  bool GetFileLog();
  void SetFileLog(bool DoFileLog);
  void LogMsg(LogLevel prio, string msg);
private:
  LogLevel log_level;
  string log_filename;
  bool log_filename_valid;
  bool console_log;
  bool file_log;
};

};

#endif
