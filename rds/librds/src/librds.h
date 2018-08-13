/*****************************************************************************
 *   This file is part of librds, a library to fetch data from the           *
 *   Radio Data System Daemon (rdsd). See http://rdsd.berlios.de             * 
 *   Copyright (C) 2005 by Hans J. Koch                                      *
 *   hjkoch@users.berlios.de                                                 *
 *                                                                           *
 *   This library is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public              *
 *   License as published by the Free Software Foundation; either            *
 *   version 2.1 of the License, or (at your option) any later version.      *
 *                                                                           * 
 *   This library is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *   Lesser General Public License for more details.                         *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *    
 *   License along with this library; if not, write to the Free Software     *
 *   Foundation,Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA *
 *****************************************************************************/

#ifndef LIBRDS_H
#define LIBRDS_H

#include <sys/types.h>

//! Type of the handle returned by rds_open_connection()
/*!
  Presently, a pointer to an object of class RDSconnection is used as a handle.
  Applications using librds should not rely on this. The typedef for
  RDSConnectionHandle might change in the future.
*/
typedef void* RDSConnectionHandle;

const unsigned int MAX_SRC_NUM = 255;   //!< Maximum RDS source number

const int CONN_TYPE_TCPIP = 1; //!< Connection uses TCP/IP
const int CONN_TYPE_UNIX  = 2; //!< Connection uses unix domain socket

//! Constants for commands.
/*!
  Used internally.
*/

enum RdsCmdNums {
  RDS_CMD_NONE = 0,   //!< No command. This is usually an error.
  RDS_CMD_ENUM_SRC,
  RDS_CMD_SET_RX_FREQ,
  RDS_CMD_GET_RX_FREQ,
  RDS_CMD_GET_RX_SIGNAL,
  RDS_CMD_SET_EVENT,
  RDS_CMD_GET_EVENT,
  RDS_CMD_FLAGS,
  RDS_CMD_PI_CODE,
  RDS_CMD_PTY_CODE,
  RDS_CMD_PROGRAMNAME,
  RDS_CMD_LOCDATETIME,
  RDS_CMD_UTCDATETIME,
  RDS_CMD_RADIOTEXT,
  RDS_CMD_LAST_RADIOTEXT,
  RDS_CMD_TMC,
  RDS_CMD_ALT_FREQ,
  RDS_CMD_GROUP_STAT,
  RDS_CMD_COUNT   //!< Dummy value to determine the command count.
};


//! String representations of commands.
/*!
  These strings are used by rdsd's protocol. Commandline tool rdsquery
  also uses them.
*/

static const char* RdsCommands[RDS_CMD_COUNT] = {
	"none",
	"esrc",
	"srxfre",
	"rxfre",
	"rxsig",
	"sevnt",
	"gevnt",
	"rflags",
	"picode",
	"ptype",
	"pname",
	"locdt",
	"utcdt",
	"rtxt",
	"lrtxt",
	"tmc",
	"aflist",
	"gstat",
};

//! Error codes returned by librds API functions
/*!
  librds API functions return one of these values (cast to int). It is guaranteed that
  RDS_OK (returned on success) will always be zero. The other values are positive integers. 
*/
enum LibRdsErr {
  RDS_OK = 0,                //!< Success, no errors
  RDS_UNKNOWN_ERROR,         //!< Error, reason is not clear
  RDS_ILLEGAL_CONN_TYPE,     //!< Connection type is neither CONN_TYPE_TCPIP nor CONN_TYPE_UNIX 
  RDS_SERVER_NOT_FOUND,      //!< The rdsd server could not be found
  RDS_SOCKET_ERROR,          //!< The socket() system call failed
  RDS_CONNECT_ERROR,         //!< The connect() system call failed
  RDS_OPEN_ERROR,            //!< The open() system call failed  
  RDS_SOCKET_NOT_OPEN,       //!< There is no valid socket file descriptor for a connection
  RDS_CLOSE_ERROR,           //!< The close() system call failed
  RDS_SOCKET_ALREADY_OPEN,   //!< Attempt to open a connection while there is still an open socket 
  RDS_BIND_ERROR,            //!< The bind() system call failed
  RDS_CHMOD_ERROR,           //!< The chmod() system call failed
  RDS_WRITE_ERROR,           //!< The write() system call failed
  RDS_READ_ERROR,            //!< The read() system call failed
  RDS_ILL_SRC_NUM,           //!< An illegal source number was given
  RDS_CMD_LIST_ERROR,        //!< An internal error in the command list. Must never happen!
  RDS_REQUEST_TIMEOUT,       //!< There was no response from rdsd within the time limit
  RDS_UNEXPECTED_RESPONSE,   //!< The response from rdsd is not what was expected
  RDS_ILLEGAL_TIMEOUT,       //!< Attempt to set a timeout value that is too big or too small
  RDS_NO_RADIO_SOURCE,       //!< Attempt to get/set the RX frequency of a source that is no radio.
  RDS_ILLEGAL_TUNER_FREQ,    //!< An illegal value for the tuner frequency was given.
  RDS_NOT_IMPLEMENTED	     //!< The requested feature is not implemented (yet).
}; 

//! Type for a variable that stores RDS flags.
/*!
  RDS data contains a number of one-bit flags. To save space, time, and network traffic,
  these flags are stored together in one variable of type rds_flags_t.
*/
typedef unsigned long rds_flags_t;

const rds_flags_t RDS_FLAG_IS_TP              = 0x0001; //!< Program is a traffic program
const rds_flags_t RDS_FLAG_IS_TA              = 0x0002; //!< Program currently broadcasts a traffic announcement
const rds_flags_t RDS_FLAG_IS_MUSIC           = 0x0004; //!< Program currently broadcasts music
const rds_flags_t RDS_FLAG_IS_STEREO          = 0x0008; //!< Program is transmitted in stereo
const rds_flags_t RDS_FLAG_IS_ARTIFICIAL_HEAD = 0x0010; //!< Program is an artificial head recording
const rds_flags_t RDS_FLAG_IS_COMPRESSED      = 0x0020; //!< Program content is compressed
const rds_flags_t RDS_FLAG_IS_DYNAMIC_PTY     = 0x0040; //!< Program type can change 
const rds_flags_t RDS_FLAG_TEXT_AB            = 0x0080; //!< If this flag changes state, a new radio text string begins

//! Type for a variable that stores RDS events.
/*!
  RDS events are stored in a variable where each event is represented
  by one bit. If an event occured, the respective bit is set to 1.
*/
typedef unsigned long rds_events_t;

const rds_events_t RDS_EVENT_FLAGS          = 0x0001; //!< One of the RDS flags has changed state
const rds_events_t RDS_EVENT_PI_CODE        = 0x0002; //!< The program identification code has changed
const rds_events_t RDS_EVENT_PTY_CODE       = 0x0004; //!< The program type code has changed
const rds_events_t RDS_EVENT_PROGRAMNAME    = 0x0008; //!< The program name has changed
const rds_events_t RDS_EVENT_UTCDATETIME    = 0x0010; //!< A new UTC date/time is available
const rds_events_t RDS_EVENT_LOCDATETIME    = 0x0020; //!< A new local date/time is available
const rds_events_t RDS_EVENT_RADIOTEXT      = 0x0040; //!< New characters were added to the radiotext buffer
const rds_events_t RDS_EVENT_LAST_RADIOTEXT = 0x0080; //!< A radio text string was completed
const rds_events_t RDS_EVENT_TMC            = 0x0100; //!< The TMC message list was modified
const rds_events_t RDS_EVENT_GROUP_STAT     = 0x0200; //!< The group statistics were updated
const rds_events_t RDS_EVENT_AF_LIST        = 0x0400; //!< An alternative frequency list is ready
const rds_events_t RDS_EVENT_RX_FREQ        = 0x0800; //!< The receiver frequency has changed.
const rds_events_t RDS_EVENT_RX_SIGNAL      = 0x1000; //!< New receiver signal strength info.


//! Constants for debug levels
/*!
  To be displayed, a debug message must have a level that is lower than or equal to the
  debug level set with rds_set_debug_params(). The minimum level that is used internally
  is RDS_DEBUG_ERROR, so setting the debug level to RDS_DEBUG_OFF will turn off all debug
  messages.
*/

enum RdsDebugLevels {
  RDS_DEBUG_OFF = 0, //!< This will turn off debug messages
  RDS_DEBUG_ERROR,   //!< Serious errors
  RDS_DEBUG_WARN,    //!< Warnings
  RDS_DEBUG_INFO,    //!< Basic information
  RDS_DEBUG_MORE,    //!< More information
  RDS_DEBUG_MOST,    //!< Even more information
  RDS_DEBUG_ALL      //!< A lot of details only usefull for developers
};

#ifdef __cplusplus
extern "C" {
#endif 

//! Create a connection object
RDSConnectionHandle rds_create_connection_object();
//! Delete the connection object and free the associated resources.
int rds_delete_connection_object(RDSConnectionHandle hnd);
//! Open a connection with rdsd.
int rds_open_connection(RDSConnectionHandle hnd, const char* rdsd_path, int conn_type,
                        int port, const char* unix_path);
//! Close a connection
int rds_close_connection(RDSConnectionHandle hnd);
//! Set the timeout for the communication with rdsd.
int rds_set_timeout_time(RDSConnectionHandle hnd, unsigned int timeout_msec);
//! Set debug parameters.
int rds_set_debug_params(RDSConnectionHandle hnd, int debug_level, unsigned int max_lines);
//! Get stored debug messages or query the required buffer size.
int rds_get_debug_text(RDSConnectionHandle hnd, char* buf, size_t* buf_size);
//! Enumerate the sources that rdsd knows about.
int rds_enum_sources(RDSConnectionHandle hnd, char* buf, size_t bufsize);
//! Set the event mask for a RDS data source.
int rds_set_event_mask(RDSConnectionHandle hnd, int src, rds_events_t evnt_mask);
//! Query the event mask that is used for a RDS data source.
int rds_get_event_mask(RDSConnectionHandle hnd, int src, rds_events_t *evnt_mask);
//! Find out which events were signaled by a data source.
int rds_get_event(RDSConnectionHandle hnd, int src, rds_events_t *events);
//! Get RDS flags data.
int rds_get_flags(RDSConnectionHandle hnd, int src, rds_flags_t *flags);
//! Get RDS program type code.
int rds_get_pty_code(RDSConnectionHandle hnd, int src, int *pty_code);
//! Get RDS program identification code.
int rds_get_pi_code(RDSConnectionHandle hnd, int src, int *pi_code);
//! Get RDS program name (Usually the abbreviation of the station name).
int rds_get_program_name(RDSConnectionHandle hnd, int src, char* buf);
//! Get the current radio text buffer. 
int rds_get_radiotext(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last complete radio text string.
int rds_get_last_radiotext(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last UTC date/time as a string.
int rds_get_utc_datetime_string(RDSConnectionHandle hnd, int src, char* buf);
//! Get the last local date/time as a string.
int rds_get_local_datetime_string(RDSConnectionHandle hnd, int src, char* buf);
//! Get the TMC message buffer or query the required buffer size.
int rds_get_tmc_buffer(RDSConnectionHandle hnd, int src, char* buf, size_t *bufsize);
//! Get the alternative frequencies buffer or query the required buffer size.
int rds_get_af_buffer(RDSConnectionHandle hnd, int src, char* buf, size_t *bufsize);
//! Get the current receiver frequency of a V4L2 radio source.
int rds_get_rx_frequency(RDSConnectionHandle hnd, int src, double *frequency);
//! Set the current receiver frequency of a V4L2 radio source.
int rds_set_rx_frequency(RDSConnectionHandle hnd, int src, double frequency);
//! Get the current receiver signal strength of a V4L2 radio source.
int rds_get_rx_signal_strength(RDSConnectionHandle hnd, int src, int *strength);
//! Get the RDS group statistics buffer or query the required buffer size.
int rds_get_group_stat_buffer(RDSConnectionHandle hnd, int src, char* buf, size_t *bufsize);
#ifdef __cplusplus
}
#endif

#endif
