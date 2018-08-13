/***************************************************************************
 *   Copyright (C) 2005 by Hans J. Koch                                    *
 *   hjkoch@users.berlios.de                                              *
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
#ifndef RDSD_ERRORS_H
#define RDSD_ERRORS_H

 /**
Error messages for rdsd

@author Hans J. Koch
*/

enum RdsdError { RDSD_OK = 0,
                 RDSD_UNKNOWN_ERROR,
                 RDSD_CONFFILE_NOT_FOUND,
		 RDSD_CONFFILE_PARSE_ERROR,
		 RDSD_INVALID_LOGFILE,
		 RDSD_NO_SOURCE_NAME,
		 RDSD_NO_SOURCE_PATH,
		 RDSD_NO_SOURCE_TYPE,
		 RDSD_NO_SOURCES,
		 RDSD_SOURCE_OPEN_ERROR,
		 RDSD_SOURCE_READ_ERROR,
		 RDSD_UNIX_SOCK_CREATE,
		 RDSD_TCPIP_SOCK_CREATE,
		 RDSD_RADIO_IOCTL,
		 RDSD_I2C_IOCTL,
	       };


	       
               
	       	       
#endif
