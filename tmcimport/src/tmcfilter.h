#include <string>
#include <vector>

#include "tmcdata.h"

class TmcFilter {
public:
	TmcFilter(TmcData *new_data, bool DropGFData);
	~TmcFilter();
	void addChunk(std::string new_string);
private:
	std::vector <std::string> old_strings;
	void printEvent(time_t time, std::string line, bool isNew, int index);
	void processLine(time_t time, std::string line, bool isNew, int index);
	TmcData *data;
	bool dropGFData;
	// TODO test
	int ci_index[5] = { };
};
