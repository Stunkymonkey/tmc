#include "tmcdata.h"

using namespace std;

TmcData::TmcData()
{
}

TmcData::~TmcData()
{
}

void TmcData::insert(string new_string) {
	if (new_string == "") {
		old = new_string;
		//TODO handle first message seperately
		return;
	}
}
