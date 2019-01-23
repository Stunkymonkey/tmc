#include <string>
#include <list>

#include "tmcdata.h"

class TmcFilter {
public:
	TmcFilter(TmcData *new_data, bool DropGFData);
	~TmcFilter();
	void addChunk(std::string new_string);
private:
	std::list<std::string> old_strings;
	std::list<int> indexes;
	void printEvent(time_t time, std::string line, bool isNew, int index);
	void processLine(time_t time, std::string line, bool isNew, int index);
	TmcData *data;
	bool dropGFData;
	std::array<int,6> ci_index = { };
};
