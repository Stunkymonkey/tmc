#include <string>

class TmcPsql{
public:
	TmcPsql(std::string db_name, std::string user, std::string password, std::string hostaddr, std::string port);
	~TmcPsql();
	bool insertEvent();
	bool insertLcd();
private:
};