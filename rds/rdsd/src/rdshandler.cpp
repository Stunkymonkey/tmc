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
#include "rdshandler.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <algorithm>

namespace std {

RDShandler::RDShandler()
{
  unix_sock_fd = -1;
  tcpip_sock_fd = -1;
  maxfd = -1;
  pid_file_name = "/var/tmp/rdsd.pid";
  unix_sock_name = "/var/tmp/rdsd.sock";
  tcpip_port = 4321;
}


RDShandler::~RDShandler()
{
  clear_srclist();
}

const string& RDShandler::GetPidFilename()
{
  return pid_file_name;
}


int RDShandler::InitConf(string conf_file_name)
{
  int ret=conf.LoadConf(conf_file_name);
  
  if (ret){
    if (ret == -1){
      log.LogMsg(LL_ERR,"Config file not found: "+conf_file_name);
      return RDSD_CONFFILE_NOT_FOUND;
    }
    if (ret == -2){
      log.LogMsg(LL_ERR,"Error parsing "+conf_file_name+" :");
      log.LogMsg(LL_ERR,conf.GetErrStr());
      return RDSD_CONFFILE_PARSE_ERROR;
    }
    return RDSD_UNKNOWN_ERROR;
  }
  ret = init_global();
  
  if (ret) return ret;
  return 0;
}

int RDShandler::Init()
{
  int ret = init_sources();
  if (ret){
    log.LogMsg(LL_ERR,"Error setting up RDS sources.");
    return ret;
  }
  else log.LogMsg(LL_DEBUG,"RDS sources setup OK.");
  
  return 0;
}

int RDShandler::WorkLoop()
{
  terminated = false;
  
  FD_ZERO(&all_fds);
  
  int ret = init_server();
  if (ret) return ret;
  ret = open_sources();
  if (ret){
    log.LogMsg(LL_ERR,"open_sources() failed.");
    return ret;
  } 
  
  calc_maxfd();
  
  while (! terminated) {
    
    fd_set tmp_fds = all_fds;
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    int n = select(maxfd+1,&tmp_fds,0,0,&timeout);
    if (n>0){
      // Process RDS source data
      for (uint i=0; i<srclist.size(); ++i){
        RDSsource* src = srclist[i];
        if (src){
	  src->Data.SetAllEvents(0);
	  if (src->GetFd()>=0){
            if (FD_ISSET(src->GetFd(),&tmp_fds)) src->Process();
	  }
        }
      }
      // Look for new client connection attempts
      if (unix_sock_fd>=0){
        if (FD_ISSET(unix_sock_fd,&tmp_fds)) accept_unix_client();
      }
      
      if (tcpip_sock_fd>=0){
        if (FD_ISSET(tcpip_sock_fd,&tmp_fds)) accept_inet_client();
      }
      // Process pending client requests
      for (uint i=0; i<clientlist.size(); ++i){
        RDSclient* cli = clientlist[i];
        if (cli){
	  // Check if we have events the client is waiting for
          if (cli->CheckEvents()<0) delete_client(cli);
	  // Did the client send a new request ?
          else if (FD_ISSET(cli->GetFd(),&tmp_fds)){
	    if (cli->Process()<0) delete_client(cli); 
	  }
        }
      }
    }
  }
  return RDSD_OK;
}

void RDShandler::Terminate()
{
  terminated = true;
}

void RDShandler::clear_srclist()
{
  for (int i=srclist.size()-1; i>=0; --i){
    RDSsource* src = srclist[i];
    if (src) delete src;
  }
  srclist.clear();
}

void RDShandler::clear_clientlist()
{
  while (clientlist.size()>0){
    RDSclient* cli = clientlist[0];
    delete_client(cli);
  }
}

int RDShandler::init_global()
{
  for (int sectno=0; sectno<conf.GetSectionCount(); ++sectno){
    ConfSection *sect = conf.GetSection(sectno);
    if (sect->GetName() == "global"){
      for (int valno=0; valno<sect->GetValueCount(); ++valno){
        ConfValue* confval = sect->GetValue(valno);
	if (confval){
          string valname = confval->GetName();
          bool valid;
	  if (valname == "logfile"){
            log.SetLogFilename(confval->GetString(valid));
          }
	  if (valname == "pidfile"){
            pid_file_name = confval->GetString(valid);
          }
	  if (valname == "loglevel"){
	    int LL=confval->GetInt(valid);
	    if (valid && (LL>=0) && (LL<=5)){
              log.SetLogLevel((LogLevel)LL);
	    }
          }
	  if (valname == "console-log"){
	    log.SetConsoleLog(confval->GetBool(valid));
          }
	  if (valname == "file-log"){
	    log.SetFileLog(confval->GetBool(valid));
          }
          if (valname == "unix-socket"){
            unix_sock_name = confval->GetString(valid);
            if (! valid) unix_sock_name="";
          }
	  if (valname == "tcpip-port"){
            tcpip_port = confval->GetInt(valid);
            if (! valid) tcpip_port=0;
          }
        }
      }
    }  
  }
  if ( log.GetFileLog() && !log.GetLogFileValid() ) return RDSD_INVALID_LOGFILE;
  
  return RDSD_OK; 
}

int RDShandler::init_sources()
{
  clear_srclist();
  for (int sectno=0; sectno<conf.GetSectionCount(); ++sectno){
    ConfSection *sect = conf.GetSection(sectno);
    if (sect->GetName() == "source"){
      RDSsource* src = new RDSsource;
      if (src){
        src->SetLogHandler(&log);
        int ret=src->Init(sect);
        if (ret==RDSD_OK) srclist.push_back(src);
        else {
          delete src;
	  return ret;
        }
      }
    }
  } 
  if (srclist.size()==0) return RDSD_NO_SOURCES;
  return 0; 
}

int RDShandler::init_server()
{
  // init unix domain socket if a name is given
  if (! unix_sock_name.empty() ) {
    struct sockaddr_un unix_adr;
    unix_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_sock_fd < 0){
      log.LogMsg(LL_ERR,"Cannot create unix domain socket.");
      return RDSD_UNIX_SOCK_CREATE;
    }
    unlink(unix_sock_name.c_str());
    int fdflags = fcntl(unix_sock_fd,F_GETFD,0);
    fcntl(unix_sock_fd,F_SETFD,fdflags|O_NONBLOCK);
    memset(&unix_adr,0,sizeof(unix_adr));
    unix_adr.sun_family = AF_UNIX;
    strncpy(unix_adr.sun_path,unix_sock_name.c_str(),sizeof(unix_adr.sun_path)-1);
    int size = strlen(unix_adr.sun_path)+sizeof(unix_adr.sun_family);
    if (bind(unix_sock_fd,(struct sockaddr*)&unix_adr,size)<0){
      log.LogMsg(LL_ERR,"Unix domain socket bind() error.");
      return RDSD_UNIX_SOCK_CREATE;
    }
    if (listen(unix_sock_fd,5)<0){
      log.LogMsg(LL_ERR,"Unix domain socket listen() error.");
      return RDSD_UNIX_SOCK_CREATE;
    }
    FD_SET(unix_sock_fd,&all_fds);
    if (unix_sock_fd>maxfd) maxfd=unix_sock_fd;
    log.LogMsg(LL_INFO,"Unix domain socket created and listening.");
  }
  // init TCP/IP socket if a port is given
  if (tcpip_port > 0) {
    struct sockaddr_in inet_adr;
    tcpip_sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (tcpip_sock_fd < 0){
      log.LogMsg(LL_ERR,"Cannot create TCP/IP socket.");
      return RDSD_TCPIP_SOCK_CREATE;
    }
    int fdflags = fcntl(tcpip_sock_fd,F_GETFD,0);
    fcntl(tcpip_sock_fd,F_SETFD,fdflags|O_NONBLOCK);
    int optval=1;
    setsockopt(tcpip_sock_fd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    memset(&inet_adr,0,sizeof(inet_adr));
    inet_adr.sin_family = AF_INET;
    inet_adr.sin_port = htons(tcpip_port);
    
    if (bind(tcpip_sock_fd,(struct sockaddr*)&inet_adr,sizeof(inet_adr))<0){
      log.LogMsg(LL_ERR,"TCP/IP socket bind() error.");
      return RDSD_TCPIP_SOCK_CREATE;
    }
    if (listen(tcpip_sock_fd,5)<0){
      log.LogMsg(LL_ERR,"TCP/IP socket listen() error.");
      return RDSD_TCPIP_SOCK_CREATE;
    }
    FD_SET(tcpip_sock_fd,&all_fds);
    if (tcpip_sock_fd>maxfd) maxfd=tcpip_sock_fd;
    log.LogMsg(LL_INFO,"TCP/IP socket created and listening.");
  }
  return RDSD_OK;
}

int RDShandler::open_sources()
{
  unsigned int i=0;
  while (i < srclist.size()){
    RDSsource* src = srclist[i];
    if (src){
      int ret = src->Open();
      if (ret) return ret;
      int src_fd = src->GetFd();
      if (src_fd>=0) FD_SET(src_fd,&all_fds);
    }
    ++i;
  }
  return RDSD_OK;
}

void RDShandler::accept_unix_client()
{
  struct sockaddr_un unix_adr;
  socklen_t size = sizeof(unix_adr);
  int fd = accept(unix_sock_fd,(struct sockaddr*)&unix_adr,&size);
  if (fd>=0){
    int fdflags = fcntl(fd,F_GETFD,0);
    fcntl(fd,F_SETFD,fdflags|O_NONBLOCK);
    RDSclient* cli = new RDSclient;
    if (cli){
      cli->SetLogHandler(&log);
      cli->SetSrcList(&srclist);
      cli->SetFd(fd);
      clientlist.push_back(cli);
      FD_SET(fd,&all_fds);
      if (fd>maxfd) maxfd=fd;
      ostringstream msg;
      msg << "Unix client added (fd=" << fd << ").";
      log.LogMsg(LL_DEBUG,msg.str());
    }
  }
}

void RDShandler::accept_inet_client()
{
  struct sockaddr_in inet_adr;
  socklen_t size = sizeof(inet_adr);
  int fd = accept(tcpip_sock_fd,(struct sockaddr*)&inet_adr,&size);
  if (fd>=0){
    int fdflags = fcntl(fd,F_GETFD,0);
    fcntl(fd,F_SETFD,fdflags|O_NONBLOCK);
    
    RDSclient* cli = new RDSclient;
    if (cli){
      cli->SetLogHandler(&log);
      cli->SetSrcList(&srclist);
      cli->SetFd(fd);
      clientlist.push_back(cli);
      FD_SET(fd,&all_fds);
      if (fd>maxfd) maxfd=fd;
      ostringstream msg;
      msg << "TCP/IP client added (fd=" << fd << ").";
      log.LogMsg(LL_DEBUG,msg.str());
    }
  }
}

void RDShandler::delete_client(RDSclient* cli)
{
  if (cli){
    int fd = cli->GetFd();
    if (fd>=0) { 
      FD_CLR(fd,&all_fds);
      cli->Close();
    }
    RDSclientList::iterator it = find(clientlist.begin(),clientlist.end(),cli);
    if (it != clientlist.end()) clientlist.erase(it);
    delete cli;
    calc_maxfd();
    ostringstream msg;
    msg << "Client connection closed (fd=" << fd << ").";
    log.LogMsg(LL_DEBUG,msg.str());
  }
}

void RDShandler::calc_maxfd()
{
  maxfd = unix_sock_fd;
  if (tcpip_sock_fd>maxfd) maxfd=tcpip_sock_fd;
  for (uint i=0; i<srclist.size(); ++i){
    RDSsource* src = srclist[i];
    if (src){
      if (src->GetFd()>maxfd) maxfd=src->GetFd();
    }
  }
  for (uint i=0; i<clientlist.size(); ++i){
    RDSclient* cli = clientlist[i];
    if (cli){
      if (cli->GetFd()>maxfd) maxfd=cli->GetFd();
    }
  }
}

};
