#include <string>
#include <fstream>

#include "tmcdata.h"

class TmcLCL{
public:
	TmcLCL(std::string points, std::string poffset, TmcData* new_data);
	~TmcLCL();
	void readPoints();
	void readOffsets();
private:
	void addPoint(std::string new_line);
	void addOffset(std::string new_line);
	TmcData *data;
	std::ifstream file_points;
	std::ifstream file_poffset;
};
