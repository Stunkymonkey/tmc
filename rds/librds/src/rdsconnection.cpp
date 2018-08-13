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
#include "rdsconnection.h"
#include <netdb.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <fcntl.h>
#include <sstream>

namespace std {

/*!
  The default constructor just does some variable initialization.
*/
RDSconnection::RDSconnection()
{
  sock_fd = -1;
  read_buf.resize(2048);
  last_scan_state = 0;
  active_debug_level = 0; //debugging off
  first_debug_line = 0;
  next_debug_line = 0;
  max_debug_lines = 100;
  timeout_time_msec = 3000; //max. wait time in milliseconds
  have_tmc_data = false;
  have_aflist_data = false;
  have_group_stat_data = false;
}

/*!
  The destructor also closes the socket if it is still open.
*/
RDSconnection::~RDSconnection()
{
  Close();
}

/*!
  Set parameters for library debugging. Calling this function will set the new
  parameters and clear the buffer.
  \param milliseconds The timeout time in milliseconds. It must be in the range
                      2..100000 (max. 100 seconds).
*/
int RDSconnection::SetTimeoutTime(unsigned int milliseconds)
{
  if ((milliseconds<2)||(milliseconds>100000)) return RDS_ILLEGAL_TIMEOUT;
  timeout_time_msec = milliseconds;
  return RDS_OK;
}

/*!
  Set parameters for library debugging. Calling this function will set the new
  parameters and clear the buffer.
  \param debug_level The higher this value is, the more information you will get.
                     debug_level==0 will turn debugging off.
  \param max_lines   Maximum number of lines in the internal buffer.
  \return RDS_OK on success.
*/
int RDSconnection::SetDebugParams(int debug_level, unsigned int max_lines)
{
  active_debug_level = debug_level;
  max_debug_lines = max_lines;
  debug_msg_buf.clear();
  first_debug_line = 0;
  next_debug_line = 0;
  return RDS_OK;
}

/*! 
  Copy the debug messages stored internally to the given buffer. The lines are
  separated by NEWLINE characters ('\n'), the whole text is zero terminated. No
  more than buf_size characters will be copied. If buf and/or buf_size is zero,
  nothing will be copied, but buf_size will receive the number of chars needed
  to store the current amount of text. If you don't call other librds functions
  in between, you can allocate a buffer of exactly this size and call  
  GetDebugTextBuffer() again.
  \param buf      Pointer to a buffer to receive the text. The user is responsible
                  for the allocation of this buffer.
  \param buf_size A variable that contains the size in chars of the buffer pointed
                  to by buf. If buf is a NULL pointer or buf_size has a zero value,
		  buf_size will receive the required size for buf.
  \return RDS_OK on success.
*/
int RDSconnection::GetDebugTextBuffer(char* buf, size_t& buf_size)
{
  if ((buf==0)||(buf_size==0)){
    buf_size=0;
    unsigned int i=first_debug_line;
    while (i != next_debug_line){
      buf_size += debug_msg_buf.at(i).size();
      buf_size += 1; // line feed char
      ++i;
      if (i >= max_debug_lines) i=0;
    }
    return RDS_OK;
  }
  unsigned int i=first_debug_line;
  size_t size=0;
  char *ptr = buf;
  while ((i != next_debug_line)&&(size < buf_size)){
    size_t len = debug_msg_buf.at(i).size();
    if ((size+len)<buf_size){
      memcpy(ptr,debug_msg_buf[i].c_str(),len);
      ptr += len;
      *ptr = '\n';
      ptr++;
    }
    size += (len+1);
    ++i;
    if (i >= max_debug_lines) i=0;
  }
  first_debug_line = i; // everything up to i is returned...
  *ptr = 0;
  return RDS_OK;
}

/*!
  Open() tries to establish a connection with rdsd. Open() itself does not
  transfer any data. After a succesful Open(), you can use any of the query
  functions. You can also use SetEventMask() to receive notifications if
  an RDS source has decoded new data.
  \param serv_path For TCP/IP, the name or IP of the server. For unix domain socket,
                   the filename the server uses.
  \param conn_type One of CONN_TYPE_TCPIP or CONN_TYPE_UNIX. The latter is only
                   possible on systems where unix domain sockets are available.
  \param port if conn_type=CONN_TYPE_TCPIP, you must pass the servers port here.
              if conn_type=CONN_TYPE_UNIX, this parameter is ignored.
  \param my_path if conn_type=CONN_TYPE_UNIX, the name of a temporary file used to
                 bind the client socket to. This must be writeable by your user.
  \return On success, Open() returns RDS_OK (0). Otherwise, a positive error code is
          returned.
*/
int RDSconnection::Open(const char* serv_path, int conn_type, int port, const char* my_path)
{
  switch (conn_type){
    case CONN_TYPE_TCPIP: return open_tcpip(serv_path,port); 
                          break;
    case CONN_TYPE_UNIX:  return open_unix(serv_path,my_path);
                          break;
    default: debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): Illegal conn_type.");
             return RDS_ILLEGAL_CONN_TYPE;
  }
}

/*!
  Close() closes a connection. Only one connection can be handled by a RDSconnection
  object. You need to call Close() before you can open a new connection.
  \return RDS_OK on success, RDS_SOCKET_NOT_OPEN or RDS_CLOSE_ERROR on errors.
*/
int RDSconnection::Close()
{
  if (sock_fd < 0) return RDS_SOCKET_NOT_OPEN;
  if (close(sock_fd)) return RDS_CLOSE_ERROR;
  return RDS_OK;
}

/*!
  EnumSources() is usually the first function you will call after a successful Open().
  It fills a buffer with a number of ASCII lines. Each line represents an RDS input
  source that is used by rdsd. Each line starts with a number, followed by a colon (':').
  This is the source number which you need to query data from that source. The rest of
  the line is a description of the source.
  \param buf Pointer to a buffer that receives the source description strings. The buffer
             should be large enough for several strings (4 kilobytes might be a safe value).
  \param bufsize Size of the buffer pointed to by buf, in bytes.
  \return Returns RDS_OK (0) on success. If the function fails, a non-zero error code is returned.
*/
int RDSconnection::EnumSources(char* buf, size_t bufsize)
{
  string data;
  int ret = wait_for_data(-1,"esrc",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > bufsize-1) len = bufsize-1;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::SetEventMask(unsigned int src, rds_events_t evnt_mask)
{
  ostringstream oss;
  oss << evnt_mask;
  int ret = send_command(src,"sevnt",oss.str());
  if (ret) return ret;
  string data;
  ret = wait_for_data(src,"sevnt",data);
  if (ret) return ret;
  if (data != "OK"){
    debug_msg(RDS_DEBUG_WARN,"SetEventMask(): Unexpected response: >"+data+"<");
    return RDS_UNEXPECTED_RESPONSE;
  }
  return RDS_OK;
}

int RDSconnection::GetEventMask(unsigned int src, rds_events_t &evnt_mask)
{
  string data;
  int ret = wait_for_data(src,"gevnt",data);
  if (ret) return ret;
  if (! StringToEvnt(data,evnt_mask)) return RDS_UNEXPECTED_RESPONSE;
  return RDS_OK;
}

/*!
  GetEvent() is usually called from the main loop of the user application. It will process
  incoming data and will return all event flags that were set since the last call.
  \param src The source for which GetEvent() is called.
  \param events A variable that will receive the events that occured since the last call.
  \return Returns RDS_OK (0) on success. If the function fails, a non-zero error code is returned.
*/
int RDSconnection::GetEvent(unsigned int src, rds_events_t &events)
{
  events = 0;
  int ret = process();
  if (ret != RDS_OK) return ret;
  if (src<rcvd_events.size()){
    events=rcvd_events[src];
    rcvd_events[src]=0;
    return RDS_OK;
  }
  return RDS_ILL_SRC_NUM;
}

int RDSconnection::GetFlags(unsigned int src, rds_flags_t &flags)
{
  string data;
  int ret = wait_for_data(src,"rflags",data);
  if (ret) return ret;
  if (! StringToFlags(data,flags)) return RDS_UNEXPECTED_RESPONSE;
  return RDS_OK;
}

int RDSconnection::GetPTYcode(unsigned int src, int &pty_code)
{
  string data;
  int ret = wait_for_data(src,"ptype",data);
  if (ret) return ret;
  if (! StringToInt(data,pty_code)) return RDS_UNEXPECTED_RESPONSE;
  return RDS_OK;
}

int RDSconnection::GetPIcode(unsigned int src, int &pi_code)
{
  string data;
  int ret = wait_for_data(src,"picode",data);
  if (ret) return ret;
  if (! StringToInt(data,pi_code)) return RDS_UNEXPECTED_RESPONSE;
  return RDS_OK;
}

int RDSconnection::GetProgramName(unsigned int src, char* buf)
{
  string data;
  int ret = wait_for_data(src,"pname",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > 8) len = 8;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::GetRadiotext(unsigned int src, char* buf)
{
  string data;
  int ret = wait_for_data(src,"rtxt",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > 64) len = 64;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::GetLastRadiotext(unsigned int src, char* buf)
{
  string data;
  int ret = wait_for_data(src,"lrtxt",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > 64) len = 64;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::GetUTCDateTimeString(unsigned int src, char* buf)
{
  string data;
  int ret = wait_for_data(src,"utcdt",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > 255) len = 255;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::GetLocalDateTimeString(unsigned int src, char* buf)
{
  string data;
  int ret = wait_for_data(src,"locdt",data);
  if (ret) return ret;
  size_t len = data.size();
  if (len > 255) len = 255;
  memcpy(buf,data.c_str(),len);
  buf[len] = 0;
  return RDS_OK;
}

int RDSconnection::GetTMCBuffer(unsigned int src, char* buf, size_t& buf_size)
{
  string data;
  int ret;
  if ((buf==0)||(buf_size==0)){
    have_tmc_data = false;
    ret = wait_for_data(src,"tmc",data);
    if (ret) return ret;
    tmc_data = data;
    have_tmc_data = true;
    buf_size=data.size()+1;
    return RDS_OK;
  }
  else {
    if (have_tmc_data){
      data = tmc_data;
      have_tmc_data = false;
    }
    else {
      ret = wait_for_data(src,"tmc",data);
      if (ret) return ret;
    }
    size_t len = data.size();
    if (len > (buf_size-1)) len = (buf_size-1);
    memcpy(buf,data.c_str(),len);
    buf[len] = 0;
  }
  return RDS_OK;
}

int RDSconnection::GetAltFreqBuffer(unsigned int src, char* buf, size_t& buf_size)
{
  string data;
  int ret;
  if ((buf==0)||(buf_size==0)){
    have_aflist_data = false;
    ret = wait_for_data(src,"aflist",data);
    if (ret) return ret;
    aflist_data = data;
    have_aflist_data = true;
    buf_size=data.size()+1;
    return RDS_OK;
  }
  else {
    if (have_aflist_data){
      data = aflist_data;
      have_aflist_data = false;
    }
    else {
      ret = wait_for_data(src,"aflist",data);
      if (ret) return ret;
    }
    size_t len = data.size();
    if (len > (buf_size-1)) len = (buf_size-1);
    memcpy(buf,data.c_str(),len);
    buf[len] = 0;
  }
  return RDS_OK;
}

int RDSconnection::GetGroupStatisticsBuffer(unsigned int src, char* buf, size_t& buf_size)
{
  string data;
  int ret;
  if ((buf==0)||(buf_size==0)){
    have_group_stat_data = false;
    ret = wait_for_data(src,"gstat",data);
    if (ret) return ret;
    group_stat_data = data;
    have_group_stat_data = true;
    buf_size=data.size()+1;
    return RDS_OK;
  }
  else {
    if (have_group_stat_data){
      data = group_stat_data;
      have_group_stat_data = false;
    }
    else {
      ret = wait_for_data(src,"gstat",data);
      if (ret) return ret;
    }
    size_t len = data.size();
    if (len > (buf_size-1)) len = (buf_size-1);
    memcpy(buf,data.c_str(),len);
    buf[len] = 0;
  }
  return RDS_OK;
}

int RDSconnection::GetTunerFrequency(unsigned int src, double& freq)
{
  string data;
  int ret = wait_for_data(src,"rxfre",data);
  if (ret) return ret;
  int freq_khz;
  if (! StringToInt(data,freq_khz)) return RDS_UNEXPECTED_RESPONSE;
  freq = (double)freq_khz * 1000.0;
  return RDS_OK;
}

int RDSconnection::SetTunerFrequency(unsigned int src, double freq)
{
  int freq_khz = (int)(freq/1000.0 + 0.5);
  ostringstream oss;
  oss << freq_khz;
  int ret = send_command(src,"srxfre",oss.str());
  if (ret) return ret;
  string data;
  ret = wait_for_data(src,"srxfre",data);
  if (ret) return ret;
  if (data != "OK"){
    debug_msg(RDS_DEBUG_WARN,"SetTunerFrequency(): Unexpected response: >"+data+"<");
    return RDS_UNEXPECTED_RESPONSE;
  }
  return RDS_OK;
}

int RDSconnection::GetTunerSignalStrength(unsigned int src, int& strength)
{
  string data;
  int ret = wait_for_data(src,"rxsig",data);
  if (ret) return ret;
  if (! StringToInt(data,strength)) return RDS_UNEXPECTED_RESPONSE;
  return RDS_OK;
}

// private member functions -------------------------------------

unsigned long RDSconnection::get_millisec_time()
{
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  return tv.tv_sec*1000 + tv.tv_usec/1000;
}

void RDSconnection::debug_msg(int debug_level, const string& msg)
{
  if (debug_level<=active_debug_level){
    unsigned int cnt = debug_msg_buf.size();
    if (next_debug_line >= cnt) debug_msg_buf.resize(next_debug_line+1);
    debug_msg_buf[next_debug_line] = msg;
    next_debug_line++;
    if (next_debug_line >= max_debug_lines) next_debug_line=0;
    if (next_debug_line == first_debug_line) {
      first_debug_line++;
      if (first_debug_line >= max_debug_lines) first_debug_line=0;
    }
  }
}

int RDSconnection::open_tcpip(const char* path, int port)
{
  if (sock_fd>=0){
    debug_msg(RDS_DEBUG_WARN,"rds_open_connection(): Already open.");
    return RDS_SOCKET_ALREADY_OPEN;
  }
  struct hostent *server;
  struct in_addr inaddr;
  if (inet_aton(path,&inaddr))
    server = gethostbyaddr((char*)&inaddr,sizeof(inaddr),AF_INET);
  else
    server = gethostbyname(path);

  if (!server){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): TCP/IP server not found.");
    return RDS_SERVER_NOT_FOUND;
  }
  struct sockaddr_in sock_addr;

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  memcpy(&sock_addr.sin_addr, server->h_addr_list[0], sizeof(sock_addr.sin_addr));

  sock_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): No socket.");
    return RDS_SOCKET_ERROR;
  }
  if (connect(sock_fd,(struct sockaddr*)&sock_addr,sizeof(sock_addr))){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): connect() failed.");
    Close();
    return RDS_CONNECT_ERROR;
  }
  debug_msg(RDS_DEBUG_INFO,"rds_open_connection(): TCP/IP connect() succeeded.");
  return RDS_OK;
}

int RDSconnection::open_unix(const char* serv_path, const char* my_path)
{
  if (sock_fd>=0){
    debug_msg(RDS_DEBUG_WARN,"rds_open_connection(): Already open.");
    return RDS_SOCKET_ALREADY_OPEN;
  }
  
  struct sockaddr_un sock_addr;
  // Create unix domain socket
  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_fd < 0){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): No socket.");
    return RDS_SOCKET_ERROR;
  }
  // Write sock_addr with own address
  memset(&sock_addr,0,sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strncpy(sock_addr.sun_path,my_path,sizeof(sock_addr.sun_path)-1);
  int size = strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family);
  unlink(sock_addr.sun_path);
  if (bind(sock_fd,(struct sockaddr*)&sock_addr,size)<0){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): bind() failed.");
    close(sock_fd);
    return RDS_BIND_ERROR;
  }
  if (chmod(sock_addr.sun_path,S_IRWXU)<0){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): chmod() failed.");
    Close();
    return RDS_CHMOD_ERROR;
  }
  // Write sock_addr with server address
  memset(&sock_addr,0,sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strncpy(sock_addr.sun_path,serv_path,sizeof(sock_addr.sun_path)-1);
  size = strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family);

  if (connect(sock_fd,(struct sockaddr*)&sock_addr,size)<0){
    debug_msg(RDS_DEBUG_ERROR,"rds_open_connection(): connect() failed.");
    Close();
    return RDS_CONNECT_ERROR;
  }
  debug_msg(RDS_DEBUG_INFO,"rds_open_connection(): unix socket connect() succeeded.");
  return RDS_OK;
}

int RDSconnection::send_command(int src, const string& cmd, const string& opt_param)
{
  if (sock_fd<0) return RDS_SOCKET_NOT_OPEN;
  ostringstream oss;
  if (src>=0) oss << src << ":";
  oss << cmd;
  
  RdsdCommand* RdsdCmd = CmdList.FindOrAdd(oss.str());
  if (! RdsdCmd) return RDS_CMD_LIST_ERROR; // Should never happen...
  
  if (RdsdCmd->GetStatus() != RCS_WAITING){
    if (opt_param.size()>0) oss << " " << opt_param;
    debug_msg(RDS_DEBUG_MORE,"send_command(): Will send: "+oss.str());
    oss << endl;
    int n = oss.str().size();
    if (write(sock_fd,oss.str().c_str(),n)!=n){
      debug_msg(RDS_DEBUG_MORE,"send_command(): write() failed.");
      return RDS_WRITE_ERROR;
    }
    RdsdCmd->SetStatus(RCS_WAITING);
  }
  
  return RDS_OK;
}

int RDSconnection::wait_for_data(int src, const string& cmd, string& data)
{
  ostringstream oss;
  if (src>=0) oss << src << ":";
  oss << cmd;
  debug_msg(RDS_DEBUG_MOST,"wait_for_data(): Waiting for: "+oss.str());
  RdsdCommand* RdsdCmd = CmdList.FindOrAdd(oss.str());
  if (! RdsdCmd) return RDS_CMD_LIST_ERROR; // Should never happen...
  if (RdsdCmd->GetStatus() == RCS_REQUEST_NEEDED){
    int ret = send_command(src,cmd,"");
    if (ret) return ret;
  }
  unsigned long t0,t1;
  t0 = get_millisec_time();
  while (RdsdCmd->GetStatus() != RCS_VALID){
    int ret = process();
    if (ret) return ret;
    t1 = get_millisec_time();
    bool timeout = false;
    if ((t1>t0)&&((t1-t0)>timeout_time_msec)) timeout=true;
    if ((t1<t0)&&((t1+86400000-t0)>timeout_time_msec)) timeout=true;
    if (timeout){
      debug_msg(RDS_DEBUG_WARN,"wait_for_data(): Timeout waiting for: "+oss.str());
      return RDS_REQUEST_TIMEOUT;
    }
  }
  debug_msg(RDS_DEBUG_MOST,"wait_for_data(): Found data for: "+oss.str());
  data = RdsdCmd->GetData();
  return RDS_OK;
}

int RDSconnection::process()
{
  if (sock_fd<0) return RDS_SOCKET_NOT_OPEN;
  int rd_cnt = read(sock_fd,&read_buf[0],read_buf.size());
  if (rd_cnt<=0) return RDS_READ_ERROR;
  enum ScanState {ssEOL=0,ssData,ssTerm,ssEvent};
  ScanState state = (ScanState)last_scan_state;
  int n, i=0;
  while (i<rd_cnt){
    char ch = read_buf[i];
    switch (ch){
      case '\n':
      case '\r': switch (state) {
                   case ssTerm:  n = read_str.size()-1;
				 while (n>0){
				   if ((read_str[n]=='\n')||(read_str[n]=='\r')){
				     read_str.erase(n,1);
				   }
				   else n=0;
				   n--;
				 }
				 process_msg();
                                 state = ssEOL;
		                 break;
		   case ssEvent: process_event_msg();
				 state = ssEOL;
		                 break; 
		   case ssEOL:   break;
                   default: read_str.push_back(ch);
		 }
		 state = ssEOL;
                 break;
      case '!' : if (state == ssEOL) state=ssEvent;
                 else read_str.push_back(ch);
		 break;
      case '.' : if (state == ssEOL) state=ssTerm;
                 else read_str.push_back(ch);
                 break;
      default:   switch (state){
                   case ssEOL:  read_str.push_back(ch);
		                state = ssData;
			        break;
		   case ssEvent:
	           case ssData: read_str.push_back(ch);
		                break;
		   case ssTerm: read_str.push_back('.');
		                read_str.push_back(ch);
		                state = ssData;
			        break;
                 }
    }
    last_scan_state = state;
    ++i;
  }
  return RDS_OK;
}


bool RDSconnection::process_msg()
{
  string cmd_str;
  string data_str;
  enum ScanState {ssSrcNum,ssCmd,ssData,ssReady,ssErr};
  ScanState state = ssSrcNum;
  unsigned int i=0;
  while ((i<read_str.size())&&(state != ssErr)){
    char ch = read_str[i];
    switch (state){
      case ssSrcNum:  if ((ch>='0')&&(ch<='9')) cmd_str.push_back(ch);
                      else if (ch==':'){
		        if (cmd_str.size()>0){
                          cmd_str.push_back(ch);
		          state=ssCmd;
		        }
		        else state=ssErr;
		      }
		      else if ((ch=='e')&&(cmd_str.size()==0)){
		        cmd_str.push_back(ch);
		        state=ssCmd; // handle esrc command
		      }
		      else state=ssErr;
		      break;
      case ssCmd:     if ((ch>='a')&&(ch<='z')) cmd_str.push_back(ch);
                      else if (ch=='\n'){
		        if (cmd_str.size()>0) state=ssData;
		        else state=ssErr;
		      }
		      break;
      case ssData:    data_str.push_back(ch);
		      break;
      default:        // just to prevent compiler warnings...
                      break;
    }
    ++i;
  } 
  read_str = "";
  debug_msg(RDS_DEBUG_MOST,"process_msg(): Adding message: "+cmd_str);
  RdsdCommand* RdsdCmd = CmdList.FindOrAdd(cmd_str);
  if (! RdsdCmd) return RDS_CMD_LIST_ERROR; // Should never happen...
  RdsdCmd->SetData(data_str);
  RdsdCmd->SetStatus(RCS_VALID);
  return true;
}

bool RDSconnection::StringToEvnt(const string &s, rds_events_t &evnt)
{
  istringstream myStream(s);
  if (myStream >> evnt) return true;
  return false;
}

bool RDSconnection::StringToFlags(const string &s, rds_flags_t &flags)
{
  istringstream myStream(s);
  if (myStream >> flags) return true;
  return false;
}

bool RDSconnection::StringToInt(const string &s, int &result)
{
  istringstream myStream(s);
  if (myStream >> result) return true;
  return false;
}

bool RDSconnection::StringToSrcNum(const string &s, unsigned int &src_num)
{
  istringstream myStream(s);
  if (myStream >> src_num){
    if ((src_num>=0)&&(src_num<=MAX_SRC_NUM)) return true;
  }
  return false;
}

bool RDSconnection::process_event_msg()
{
  string num_str;
  string evnt_str;
  bool is_num_str = true;
  bool is_error = false;
  for (unsigned int i=0; i<read_str.size(); i++){
    char ch = read_str[i];
    if (ch == ':') is_num_str = false;
    else {
      if ((ch>='0')&&(ch<='9')){
        if (is_num_str) num_str += ch; else evnt_str += ch;
      }
      else is_error=true;
    }
  }
  if (is_error) return false;
  unsigned int src;
  if (! StringToSrcNum(num_str,src)) return false;
  rds_events_t event_code;
  if (! StringToEvnt(evnt_str,event_code)) return false;
  if (rcvd_events.size() <= src){
    unsigned int i=rcvd_events.size();
    rcvd_events.resize(src+1);
    while (i<(src+1)) rcvd_events[i++] = 0;
  }
  debug_msg(RDS_DEBUG_MOST,"process_event_msg(): Received events: "+evnt_str);
  rcvd_events[src] |= event_code;
  if (event_code & RDS_EVENT_FLAGS)          send_command(src,"rflags","");
  if (event_code & RDS_EVENT_PI_CODE)        send_command(src,"picode","");
  if (event_code & RDS_EVENT_PTY_CODE)       send_command(src,"ptype","");
  if (event_code & RDS_EVENT_PROGRAMNAME)    send_command(src,"pname","");
  if (event_code & RDS_EVENT_UTCDATETIME)    send_command(src,"utcdt","");
  if (event_code & RDS_EVENT_LOCDATETIME)    send_command(src,"locdt","");
  if (event_code & RDS_EVENT_RADIOTEXT)      send_command(src,"rtxt","");
  if (event_code & RDS_EVENT_LAST_RADIOTEXT) send_command(src,"lrtxt","");
  if (event_code & RDS_EVENT_TMC)            send_command(src,"tmc","");
  if (event_code & RDS_EVENT_GROUP_STAT)     send_command(src,"gstat","");
  if (event_code & RDS_EVENT_AF_LIST)        send_command(src,"aflist","");
  if (event_code & RDS_EVENT_RX_FREQ)        send_command(src,"rxfre","");
  if (event_code & RDS_EVENT_RX_SIGNAL)      send_command(src,"rxsig","");
  return true;
}

};
