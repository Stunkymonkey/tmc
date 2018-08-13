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
#ifndef STDRDSQUERYHANDLER_H
#define STDRDSQUERYHANDLER_H

#include <string>
#include <vector>
#include <librds.h>

namespace std {

/**
@author Hans J. Koch
*/

const string RdsErrorStrings[] = {
  "Success, no errors.",
  "Error, reason is not clear.",
  "Connection type is neither CONN_TYPE_TCPIP nor CONN_TYPE_UNIX.",
  "The rdsd server could not be found.",
  "The socket() system call failed.",
  "The connect() system call failed.",
  "The open() system call failed.",
  "There is no valid socket file descriptor for a connection.",
  "The close() system call failed.",
  "Attempt to open a connection while there is still an open socket.",
  "The bind() system call failed.",
  "The chmod() system call failed.",
  "The write() system call failed.",
  "The read() system call failed.",
  "An illegal source number was given.",
  "An internal error in the command list. Must never happen!.",
  "There was no response from rdsd within the time limit.",
  "The response from rdsd is not what was expected.",
  "Attempt to set a timeout value that is too big or too small.",
  "Attempt to get/set the RX frequency of a source that is no radio.",
  "An illegal value for the tuner frequency was given.",
  "The requested feature is not implemented (yet)."
};


class RdsQueryHandler{
public:
	RdsQueryHandler();
	~RdsQueryHandler();
	void SetHandle(RDSConnectionHandle hnd) { handle = hnd; }
	RDSConnectionHandle GetHandle() { return handle; }
	void SetSourceNum(int src) { src_num = src; }
	int GetSourceNum() { return src_num; }
	void SetShowDebugOnError(bool Show) {show_debug_on_error = Show; } 
	bool GetShowDebugOnError() {return show_debug_on_error; }
	void InitRecordCounters(rds_events_t events, int count);
	bool DecRecordCounters(rds_events_t events);
	void ShowError(int rds_err_num);
	void ShowEnumSrc();
	void ShowFlags();
	void ShowPIcode();
	void ShowPTYcode();
	void ShowProgramName();
	void ShowRadioText();
	void ShowLastRadioText();
	void ShowLocalDateTime();
	void ShowUTCdateTime();
	void ShowGroupStatistics();
	void ShowAltFreqList();
	void ShowTMCList();
	void ShowTunerFrequency();
	void ShowSetTunerFrequency(double FreqToSet);
	void ShowSignalStrength();
private:
	RDSConnectionHandle handle;
	vector<int> event_counts;
	bool infinite_record_count;
	int src_num;
	bool show_debug_on_error;
	void show_debug();
};

}

#endif
