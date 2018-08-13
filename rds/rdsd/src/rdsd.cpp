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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdshandler.h"
#include <cstdlib>
#include <csignal>
#include <fcntl.h>
#include <cstring>
#include <sstream>

using namespace std;

RDShandler handler;

static void sig_proc(int signr)
{
  switch (signr){
    case SIGINT:
    case SIGTERM:
                 handler.Terminate();
		 break;
    case SIGHUP:
    case SIGPIPE:
                 break;
  }
}

static int check_pid_file(string pid_file_name)
{
  int fd;
  FILE *fil;
  int oldpid;
  if ((fil = fopen(pid_file_name.c_str(),"r"))!=0){
    if (fscanf(fil,"%d",&oldpid)>0){
      fclose(fil);
      return oldpid;
    }
    fclose(fil);
  }
  if ((fd = open(pid_file_name.c_str(),O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0) return -1;

  char pidbuf[10];
  sprintf(pidbuf,"%d\n",getpid());
  int len = strlen(pidbuf);
  if (write(fd,pidbuf,len) != len) return -4;
  
  close(fd);
  return 0;
}

static int daemonize()
{
  pid_t pid;
  if ((pid = fork())<0) return -1;
  else if (pid != 0) exit(0);
  
  setsid();
  chdir("/");
  umask(0);
  return 0;
}

static void clean_exit(int err_code)
{
  unlink(handler.GetPidFilename().c_str());
  ostringstream msg;
  if (err_code) msg << "rdsd terminating with error code " << err_code;
  else msg << "rdsd: normal shutdown.";
  handler.log.LogMsg(LL_INFO,msg.str());
  exit(err_code);
}

static void usage()
{
  cout << "Usage: rdsd [Options]" << endl;
  cout << "  Options: -c <file name> : use alternative conf file." << endl;
  cout << "           -d             : fork and run as daemon." << endl;
  cout << "           -h             : show this help and exit." << endl;
  cout << "           -v             : output version info and exit." << endl;
}

string conf_file_name = "/etc/rdsd.conf";
int  running_pid = -1;
bool do_daemonize = false;

static void parse_cmdline(int argc, char* argv[])
{
  int opt;
  while ((opt = getopt(argc,argv,"c:dhv")) != -1){
    switch (opt) {
      case 'c':  conf_file_name = optarg;
                 break;
      case 'd':  do_daemonize = true;
                 break;
      case 'h':  usage();
                 clean_exit(0);
                 break;
      case 'v':  cout << VERSION << endl;
                 clean_exit(0);
                 break;
      default:   usage();
                 clean_exit(1); 
    }
  }
}


int main(int argc, char* argv[])
{
  parse_cmdline(argc,argv);

  signal(SIGINT,sig_proc);
  signal(SIGTERM,sig_proc);
  signal(SIGHUP,sig_proc);
  signal(SIGPIPE,sig_proc);
  
  handler.log.SetConsoleLog(true);
  handler.log.SetFileLog(false);
  handler.log.SetLogLevel(LL_DEBUG);
  
  int ret;

  ret = handler.InitConf(conf_file_name);
  if (ret){
    handler.log.LogMsg(LL_EMERG,"Cannot initialize RDS handler.");
    clean_exit(1);
  }
  else handler.log.LogMsg(LL_DEBUG,"RDS handler initialized.");
  
  
  ret = check_pid_file(handler.GetPidFilename());
  running_pid = (ret>0) ? ret : -1;

  if (running_pid>0){
    handler.log.LogMsg(LL_EMERG,"Cannot initialize PID file (already running ?).");
    clean_exit(2);
  }

  if (do_daemonize) {
    unlink(handler.GetPidFilename().c_str());
    handler.log.SetConsoleLog(false);
    if (daemonize()<0){
      handler.log.LogMsg(LL_EMERG,"Ooops! Cannot fork()...");
      clean_exit(3);
    } 
    check_pid_file(handler.GetPidFilename()); //We have a new PID now...
  }
  
  ret = handler.Init();
  if (ret) clean_exit(ret);
  
  ret = handler.WorkLoop();
  
  clean_exit(ret);
}



