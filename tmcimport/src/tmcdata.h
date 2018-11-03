#include <string>

class TmcData {
public:
	TmcData();
	~TmcData();
	void insert(std::string new_string);
private:
	std::string old = "";
};
