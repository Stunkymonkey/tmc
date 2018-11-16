#include <string>
#include <fstream>

using namespace std;

class RdsWriter{
public:
	RdsWriter(string filename, bool append);
	~RdsWriter();
	bool write(string s);
private:
	ofstream myfile;
};
