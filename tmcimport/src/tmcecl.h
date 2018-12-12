#include <string>
#include <fstream>

#include "tmcdata.h"

class TmcECL{
public:
	TmcECL(std::string events, TmcData *new_data);
	~TmcECL();
	void readEventTypes();
private:
	void addEventType(std::string new_line);
	TmcData *data;
	std::ifstream file_events;
};
