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
#include "rdssource.h"
#include "rdsd_errors.h"
#include <librds.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sstream>
#include <errno.h>

// We shouldn't include kernel headers...
#include <linux/types.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <cstring>


// The following define is stolen from linux/i2c.h to avoid including
// a kernel header. 
#define I2C_SLAVE	0x0703

namespace std {

RDSsource::RDSsource()
  : tuner_freq_khz(-1), src_type(SRCTYPE_NONE), fd(-1), getfreq(true),
    freq_factor(16), use_v4l1(false), status(SRCSTAT_CLOSED), log(0)
     
{

}


RDSsource::~RDSsource()
{
  Close();
}

string RDSsource::GetName()
{
  return srcname;
}

string RDSsource::GetPath()
{
  return srcpath;
}

void RDSsource::SetLogHandler(LogHandler *loghandler)
{
  log = loghandler;
  Data.SetLogHandler(loghandler);
}

string RDSsource::GetStatusStr()
{
  ostringstream result;
  result << (int)status << endl;
  switch (status){
    case SRCSTAT_OK:     result << "#OK, data available.";
                         break;
    case SRCSTAT_WAIT:   result << "#Waiting for data.";
                         break;
    case SRCSTAT_CLOSED: result << "#Source closed.";
                         break;
  }
  return result.str();
}

int RDSsource::GetFd()
{
  return fd;
}

int RDSsource::Open()
{
  Close();
  int ret;
  
  switch (src_type){
    case SRCTYPE_NONE:
           return RDSD_NO_SOURCE_TYPE;
           break;
    case SRCTYPE_RADIODEV:
    case SRCTYPE_FILE:
           ret = open(srcpath.c_str(),O_RDONLY|O_NONBLOCK);
           if (ret<0){
             LogMsg(LL_ERR,"Failed to open source: "+srcname);
             return RDSD_SOURCE_OPEN_ERROR;
           }
	   fd = ret;
	   if (src_type == SRCTYPE_RADIODEV){
	     freq_factor = 16;
	     use_v4l1 = false;
             struct v4l2_tuner tuner_v4l2;
             memset(&tuner_v4l2,0,sizeof(tuner_v4l2));
             ret = ioctl(fd, VIDIOC_G_TUNER, &tuner_v4l2);
             if (ret == 0){
               if (tuner_v4l2.capability & V4L2_TUNER_CAP_LOW) freq_factor = 16000;
               LogMsg(LL_DEBUG,"Using V4L2 for "+srcname);
             }
	     else{
	       struct video_tuner tuner_v4l1;
	       memset(&tuner_v4l1,0,sizeof(tuner_v4l1));
               ret = ioctl(fd, VIDIOCGTUNER, &tuner_v4l1);
               if (ret == 0){
                 use_v4l1 = true;
	         if (tuner_v4l1.flags & VIDEO_TUNER_LOW) freq_factor = 16000;
	         LogMsg(LL_DEBUG,"Using V4L1 for "+srcname);
	       }
	       else LogMsg(LL_WARN,"Neither V4L2 nor V4L1 works for "+srcname);
	     }
	     if (tuner_freq_khz > 0){
               ret = SetRadioFreq(tuner_freq_khz);
               if (ret != RDS_OK) return ret;
               ret = RadioUnMute();
               if (ret != RDS_OK) return ret;
	     }
	   }
           break;
    case SRCTYPE_I2CDEV:
           ret = open(srcpath.c_str(),O_RDWR); //We might need to be able to write bytes to SAA6588 registers
           if (ret<0){
             LogMsg(LL_ERR,"Failed to open source: "+srcname);
             return RDSD_SOURCE_OPEN_ERROR;
           }
	   fd = ret;
	   if (ioctl(fd,I2C_SLAVE,0x20 >> 1)){ //FIXME: 0x20 should be configurable
             LogMsg(LL_ERR,"I2C ioctl failed for source: "+srcname);
             close(fd);
             return RDSD_I2C_IOCTL;
	   }
           break;
  }
  LogMsg(LL_DEBUG,"Source opened: "+srcname);
  status = SRCSTAT_WAIT;
  return RDSD_OK;
}

void RDSsource::Close()
{
  if (fd>=0){
    if (tuner_freq_khz > 0) RadioMute();
    close(fd);
  }
  fd = -1;
  status = SRCSTAT_CLOSED;
}

int RDSsource::Process()
{
  if (status != SRCSTAT_CLOSED){
    CharBuf buf(300);
    int ret = 0;
    unsigned char tmp;
    unsigned char blocknum;
    switch (src_type){
      case SRCTYPE_NONE:
             return RDSD_NO_SOURCE_TYPE;
             break;
      case SRCTYPE_RADIODEV:
             int freq;
             ret = read(fd,&buf[0],buf.size());
             if (getfreq) {
               GetRadioFreq(freq);
               if (freq != tuner_freq_khz) {
                 Data.FreqChanged();
                 tuner_freq_khz = freq;
                 // the following is a bit of a hack; how will we when there's no more data coming from the old channel???
                 ret = 0;
               }
             }
             break;
      case SRCTYPE_FILE:
             ret = read(fd,&buf[0],3);
	     if ((ret>0)&&(ret!=3)) ret = -1;
	     usleep(20000);
             break;
      case SRCTYPE_I2CDEV:
             ret = read(fd,&buf[0],6);
	     if (ret==6){
               buf.resize(3); //use only first 3 bytes for the moment
               blocknum = buf[0] >> 5;
               tmp = buf[2];
	       buf[2] = buf[0];
	       buf[0] = tmp;

	       tmp = blocknum;
	       tmp |= blocknum << 3;	/* Received offset == Offset Name (OK ?) */
	       if ((buf[2] & 0x03) == 0x03)
		 tmp |= 0x80;	/* uncorrectable error */
	       else if ((buf[2] & 0x03) != 0x00)
		 tmp |= 0x40;	/* corrected error */
	       buf[2] = tmp;	/* Is this enough ? Should we also check other bits ? */
	       ret=3;
	     }
	     else if (ret>0) ret = -1;
             break;
    }
    if (ret<0) return RDSD_SOURCE_READ_ERROR;
    if (ret>0){
      buf.resize(ret);
      Data.AddBytes(&buf);
      status = SRCSTAT_OK;
      // RDS_EVENT_RX_FREQ and RDS_EVENT_RX_SIGNAL make only sense if we have a radio device:
      if (src_type != SRCTYPE_RADIODEV) Data.ClearEvents(RDS_EVENT_RX_FREQ | RDS_EVENT_RX_SIGNAL);
    }
    return RDSD_OK;
  }
  return 0;
}
  
int RDSsource::Init(ConfSection* sect)
{
  if (! sect) return -1;
  srcname = "";
  srcpath = "";
  tuner_freq_khz = -1;
  for (int valno=0; valno < sect->GetValueCount(); ++valno){
    ConfValue* val = sect->GetValue(valno);
    if (val){
      string valname = val->GetName();
      bool valid;
      if (valname == "name"){
        srcname = val->GetString(valid);
      }
      else if (valname == "path"){
        srcpath = val->GetString(valid);
      }
      else if (valname == "type"){
        string S = val->GetString(valid);
	if (S == "radiodev") src_type = SRCTYPE_RADIODEV;
	else if (S == "i2cdev") src_type = SRCTYPE_I2CDEV;
	else if (S == "file") src_type = SRCTYPE_FILE;
	else src_type = SRCTYPE_NONE;
      }
      else if (valname == "getradiofreq"){
	getfreq = val->GetBool(valid);
	if (! valid){
          getfreq = false;
          LogMsg(LL_ERR,"Illegal boolean setting for getradiofreq in source "+srcname);
	}
      }
      else if (valname == "tunerfreq"){
        tuner_freq_khz = val->GetInt(valid);
        if (! valid){
          tuner_freq_khz = -1;
          LogMsg(LL_ERR,"Illegal tuner frequency for source "+srcname);
          return RDS_ILLEGAL_TUNER_FREQ;
        }
      }
    }
  }
  if (srcname.empty()) return RDSD_NO_SOURCE_NAME;
  if (srcpath.empty()) return RDSD_NO_SOURCE_PATH;
  if (src_type == SRCTYPE_NONE) return RDSD_NO_SOURCE_TYPE;
  LogMsg(LL_DEBUG,"Added source definition: "+srcname);
  return RDSD_OK;
}

void RDSsource::LogMsg(LogLevel prio, string msg)
{
  if (log) log->LogMsg(prio,msg);
}

int RDSsource::SetRadioFreq(int freq_khz)
{
  if (src_type != SRCTYPE_RADIODEV) return RDS_NO_RADIO_SOURCE;
  int ifreq = freq_khz*freq_factor/1000;
  int ret;
  if (use_v4l1){
    ret = ioctl(fd, VIDIOCSFREQ, &ifreq); //V4L1
    if (ret != 0){
      show_sys_error("SetRadioFreq() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  else {
    struct v4l2_frequency freq_struct;
    memset(&freq_struct,0,sizeof(freq_struct));
    freq_struct.tuner = 0; 
    freq_struct.type = V4L2_TUNER_RADIO;
    freq_struct.frequency = ifreq;
    ret = ioctl(fd, VIDIOC_S_FREQUENCY, &freq_struct); //V4L2
    if (ret != 0){
      show_sys_error("SetRadioFreq() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  return RDS_OK;
}

int RDSsource::GetRadioFreq(int &freq_khz)
{
  if (src_type != SRCTYPE_RADIODEV) return RDS_NO_RADIO_SOURCE;
  int ifreq, ret;
  if (use_v4l1){
    ret = ioctl(fd, VIDIOCGFREQ, &ifreq); //V4L1
    if (ret != 0){
      show_sys_error("GetRadioFreq() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  else {
    struct v4l2_frequency freq_struct;
    memset(&freq_struct,0,sizeof(freq_struct));
    freq_struct.tuner = 0;
     ret = ioctl(fd, VIDIOC_G_FREQUENCY, &freq_struct); //V4L2
    if (ret != 0){
      show_sys_error("GetRadioFreq() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
     }
    ifreq = freq_struct.frequency;
  }
  freq_khz = 1000*ifreq/freq_factor;
  freq_khz = 878000;
  return RDS_OK;
}

int RDSsource::GetSignalStrength(int &signal_strength)
{
  if (src_type != SRCTYPE_RADIODEV) return RDS_NO_RADIO_SOURCE;
  int ret;
  if (use_v4l1){
    struct video_tuner vt;
    memset(&vt,0,sizeof(vt));
    ret = ioctl (fd, VIDIOCGTUNER, &vt); //V4L1
    if (ret != 0){
      show_sys_error("GetSignalStrength() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    signal_strength = vt.signal; // 0..65535
   }
  else{
    struct v4l2_tuner tuner_v4l2;
    memset(&tuner_v4l2,0,sizeof(tuner_v4l2));
    ret = ioctl(fd, VIDIOC_G_TUNER, &tuner_v4l2);
    if (ret != 0){
      show_sys_error("GetSignalStrength() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    signal_strength = tuner_v4l2.signal; // 0..65535
   signal_strength = 32768;
  }
  return RDS_OK;
}

int RDSsource::RadioMute()
{
  if (src_type != SRCTYPE_RADIODEV) return RDS_NO_RADIO_SOURCE;
  if (use_v4l1){
    struct video_audio vid_aud;
    if (ioctl(fd, VIDIOCGAUDIO, &vid_aud)){
      show_sys_error("RadioMute() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    vid_aud.flags |= VIDEO_AUDIO_MUTE;
    if (ioctl(fd, VIDIOCSAUDIO, &vid_aud)){
      show_sys_error("RadioMute() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  else{
    struct v4l2_queryctrl qctrl;
    qctrl.id = V4L2_CID_AUDIO_MUTE;
    if (ioctl(fd,VIDIOC_QUERYCTRL,&qctrl)){
      show_sys_error("RadioMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_AUDIO_MUTE;
    ctrl.value = qctrl.maximum;
    if (ioctl(fd,VIDIOC_S_CTRL,&ctrl)) {
      show_sys_error("RadioMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  return RDS_OK;
}

int RDSsource::RadioUnMute()
{
  if (src_type != SRCTYPE_RADIODEV) return RDS_NO_RADIO_SOURCE;
  if (use_v4l1){
    struct video_audio vid_aud;
    if (ioctl(fd, VIDIOCGAUDIO, &vid_aud)){
      show_sys_error("RadioUnMute() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    if (vid_aud.volume == 0) vid_aud.volume = 65535;
    vid_aud.flags &= ~VIDEO_AUDIO_MUTE;
    if (ioctl(fd, VIDIOCSAUDIO, &vid_aud)){
      show_sys_error("RadioUnMute() V4L1 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  else{
    struct v4l2_queryctrl qctrl;
    qctrl.id = V4L2_CID_AUDIO_VOLUME;
    if (ioctl(fd,VIDIOC_QUERYCTRL,&qctrl)){
      show_sys_error("RadioUnMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_AUDIO_VOLUME;
    if (ioctl(fd,VIDIOC_G_CTRL,&ctrl)){
      show_sys_error("RadioUnMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    if (ctrl.value == qctrl.minimum){
      ctrl.value = qctrl.maximum;
      if (ioctl(fd,VIDIOC_S_CTRL,&ctrl)){
      show_sys_error("RadioUnMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    }
    qctrl.id = V4L2_CID_AUDIO_MUTE;
    if (ioctl(fd,VIDIOC_QUERYCTRL,&qctrl)){
      show_sys_error("RadioUnMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
    ctrl.id = V4L2_CID_AUDIO_MUTE;
    ctrl.value = qctrl.minimum;
    if (ioctl(fd,VIDIOC_S_CTRL,&ctrl)){
      show_sys_error("RadioUnMute() V4L2 ioctl:");
      return RDSD_RADIO_IOCTL;
    }
  }
  return RDS_OK;
}

void RDSsource::show_sys_error(const string& msg)
{
  if (errno){
    LogMsg(LL_ERR,msg+strerror(errno));
  }
  else{
    LogMsg(LL_DEBUG,msg+" OK.");
  }
}

};
