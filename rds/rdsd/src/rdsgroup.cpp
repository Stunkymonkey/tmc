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
#include "rdsgroup.h"

namespace std {

RDSgroup::RDSgroup()
{
  byte_buf.resize(8);
  Clear();
}


RDSgroup::~RDSgroup()
{
}

void RDSgroup::Clear()
{
  for (int i=0; i<byte_buf.size(); i++) byte_buf[i] = 0;
  group_status = GS_EMPTY;
  group_type = GROUP_UNKNOWN;
  next_expected_block = 0;
  last_block_num = -1;
}

void RDSgroup::AddBlock(unsigned char b0, unsigned char b1, unsigned char b2)
{
  
  if ((b2 & 0x80)!=0){ //erroneous block
    group_status = GS_ERROR;
    return;
  }
  
  int blocknum = b2 & 0x07; // What's the differnce between "Received Offset"
                            // and "Offset Name" in V4L2 spec ???
  if (blocknum == 4) blocknum = 2; // Treat C' as C
  if ((blocknum == 5)||(blocknum==6)) return; // ignore E Blocks
  if (blocknum == 7){ //invalid block
    group_status = GS_ERROR;
    return;
  }
  if (blocknum == last_block_num) return;
  if ((group_status == GS_EMPTY)&&(blocknum != 0)) return;
  if (blocknum != next_expected_block){
    group_status = GS_ERROR;
    return;
  }

  byte_buf[2*blocknum]   = b0; //LSB
  byte_buf[2*blocknum+1] = b1; //MSB
  group_status = GS_INCOMPLETE;

  if (blocknum == 1){
    group_type = (RDSGroupType)(b1 >> 3);
  }

  last_block_num = blocknum;
  next_expected_block = blocknum+1;
  if (next_expected_block > 3) group_status = GS_COMPLETE;
  return;
}

GroupStatus RDSgroup::GetGroupStatus()
{
  return group_status;
}

RDSGroupType RDSgroup::GetGroupType()
{
  return group_type;
}

int RDSgroup::GetByte(int blocknum, int bytenum)
{
  return byte_buf[2*blocknum+bytenum];
}

int RDSgroup::GetWord(int blocknum)
{
  return (GetByte(blocknum,1) << 8) | GetByte(blocknum,0);
}


}
