#include <string>
#include <vector>

#include "tmcdata.h"

class TmcFilter {
public:
	TmcFilter(TmcData* new_data);
	~TmcFilter();
	void addChunk(std::string new_string);
private:
	std::vector <std::string> old_strings;
	void printEvent(time_t time, std::string line, bool isNew);
	void processLine(time_t time, std::string line, bool isNew);
	TmcData *data;
	int ci_index[6] = { };
};
