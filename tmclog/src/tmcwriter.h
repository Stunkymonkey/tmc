#include <string>
#include <fstream>

class RdsWriter{
public:
	RdsWriter(std::string filename, bool append);
	~RdsWriter();
	bool write(std::string s);
private:
	std::ofstream myfile;
	std::string last_s = "";
};
