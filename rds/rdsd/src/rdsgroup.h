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
#ifndef STDRDSGROUP_H
#define STDRDSGROUP_H

#include <vector>

namespace std {

enum RDSGroupType {GROUP_0A=0,GROUP_0B,GROUP_1A,GROUP_1B,GROUP_2A,GROUP_2B,
                   GROUP_3A,GROUP_3B,GROUP_4A,GROUP_4B,GROUP_5A,GROUP_5B,
		   GROUP_6A,GROUP_6B,GROUP_7A,GROUP_7B,GROUP_8A,GROUP_8B,
		   GROUP_9A,GROUP_9B,GROUP_10A,GROUP_10B,GROUP_11A,GROUP_11B,
		   GROUP_12A,GROUP_12B,GROUP_13A,GROUP_13B,GROUP_14A,GROUP_14B,
		   GROUP_15A,GROUP_15B,GROUP_UNKNOWN};

enum GroupStatus {GS_EMPTY, GS_INCOMPLETE, GS_ERROR, GS_COMPLETE};

/**
@author Hans J. Koch
*/
class RDSgroup{
public:
  RDSgroup();
  ~RDSgroup();
  void Clear();
  void AddBlock(unsigned char b0, unsigned char b1, unsigned char b2);
  GroupStatus GetGroupStatus();
  RDSGroupType GetGroupType();
  int GetByte(int blocknum, int bytenum);
  int GetWord(int blocknum);
private:
  vector<unsigned char> byte_buf;
  GroupStatus group_status;
  RDSGroupType group_type;
  int next_expected_block;
  int last_block_num;
};

}

#endif
