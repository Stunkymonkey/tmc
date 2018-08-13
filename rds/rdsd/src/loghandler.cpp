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
#include "loghandler.h"
#include <iostream>
#include <fstream>
#include <ctime>

namespace std {

LogHandler::LogHandler()
{
  log_level = LL_WARN;
  log_filename_valid = false;
}

LogHandler::~LogHandler()
{
}

LogLevel LogHandler::GetLogLevel()
{
  return log_level;
}

void LogHandler::SetLogLevel(LogLevel NewLL)
{
  log_level = NewLL;
}

int LogHandler::SetLogFilename(string filename)
{
  log_filename = "";
  ofstream test(filename.c_str(),ios::app);
  if (!test){
    log_filename_valid = false;
    return -1;
  }
  log_filename = filename;
  log_filename_valid = true;
  return 0;
}

string LogHandler::GetLogFilename()
{
  return log_filename;
}

bool LogHandler::GetLogFileValid()
{
  return log_filename_valid;
}

bool LogHandler::GetConsoleLog()
{
  return console_log;
}

void LogHandler::SetConsoleLog(bool DoConsoleLog)
{
  console_log = DoConsoleLog;
}

bool LogHandler::GetFileLog()
{
  return file_log;
}

void LogHandler::SetFileLog(bool DoFileLog)
{
  file_log = DoFileLog;
}

void LogHandler::LogMsg(LogLevel prio, string msg)
{
  if (prio <= log_level){
    time_t td;
    time(&td);
    string timestr = ctime(&td);
    timestr[timestr.length()-1]=' ';
    if (console_log) cerr << timestr << msg << endl;
    if ((file_log)&&(! log_filename.empty())){
      ofstream logfile(log_filename.c_str(),ios::app);
      if (logfile) logfile << timestr << msg << endl;
    }
  }
}



};
