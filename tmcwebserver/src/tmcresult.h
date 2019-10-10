#ifndef TMCSTRUCT_H
#define TMCSTRUCT_H

struct TmcResult {
	std::string event;
	std::string start;
	std::string end;
	std::vector<std::pair<double,double>> path;
};

#endif