#include <string>
#include <fstream>

#include "tmcdata.h"

class TmcLcd{
public:
	TmcLcd(std::string filename, TmcData* new_data);
	~TmcLcd();
	void readLines();
	void addLine(std::string new_line);
private:
	TmcData *data;
	std::ifstream in;
};
