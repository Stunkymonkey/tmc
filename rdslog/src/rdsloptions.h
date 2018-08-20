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
#ifndef STDRDSQOPTIONS_H
#define STDRDSQOPTIONS_H

#include <librds.h>
#include <string>

namespace std {

/**
@author Hans J. Koch
*/
class RdsqOptions{
public:
	RdsqOptions();
	~RdsqOptions();
	bool ProcessCmdLine(int argc, char *argv[]);
	void ShowOptions();
	int GetConnectionType() { return conn_type; }
	const string& GetServerName() { return server_name; }
	int GetPort() { return tcpip_port; }
	int GetSourceNum() { return source_num; }
	rds_events_t GetEventMask() { return event_mask; }
	bool IsAppendMode() {return !have_opt_c; }
	const string& GetFileName() { return file_name; }
private:
	int conn_type;
	string server_name;
	int tcpip_port;
	int source_num;
	rds_events_t event_mask;
	bool have_opt_s;
	bool have_opt_p;
	bool have_opt_u;
	bool have_opt_c;
	string file_name;
	void show_usage();
	void show_version();
	bool try_str_to_int(char *s, int &result);
};

}

#endif
