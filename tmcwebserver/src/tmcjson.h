#include <string>

class TmcJson{
public:
	static std::string min_max_date(std::string *min, std::string *max);
	static bool tmc_request(std::string body, double& northEastLat, double& northEastLng, double& southWestLat, double& southWestLng, std::string& start, std::string& end);
	static std::string tmc_query(std::vector<struct TmcResult*>);
private:
};