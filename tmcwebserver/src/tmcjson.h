#include <string>

class TmcJson{
public:
	static std::string min_max_date(std::string *min, std::string *max);
	static bool tmc_request(std::string body, float& northEastLat, float& northEastLng, float& southWestLat, float& southWestLng, std::string& start_date, std::string& end_date, std::string& start_time, std::string& end_time);
	static std::string tmc_query(std::vector<struct TmcResult*>);
private:
};