#include <string>
#include <vector>

class TmcData {
public:
	TmcData();
	~TmcData();
	void addChunk(std::string new_string);
private:
	std::vector <std::string> old_strings;
	void printEvent(time_t time, std::string line, bool isNew);
	void processLine(time_t time, std::string line, bool isNew);
};
