#include <string>
#include <vector>

class TmcData {
public:
	TmcData();
	~TmcData();
	void addChunk(std::string new_string);
private:
	std::vector <std::string> old_strings;
	void new_line(std::string time_str, std::string new_line);
	void old_line(std::string time_str, std::string old_line);
};
