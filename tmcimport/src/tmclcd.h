#include <string>
#include <fstream>

#include "tmcdata.h"

class TmcLcd{
public:
	TmcLcd(std::string points, std::string poffset, TmcData* new_data);
	~TmcLcd();
	void readLines();
	void addLine(std::string new_line);
private:
	TmcData *data;
	std::ifstream in;
};
