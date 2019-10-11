#include <iostream>

#include "tmcdata.h"
#include "tmcresult.h"

using namespace std;

std::tuple<std::string, std::string> TmcData::minMaxDate() {
	int buffer_len = 11;
	char buffer [buffer_len];
	struct tm * tmp_timeinfo;

	tmp_timeinfo = localtime(&min_date);
	strftime(buffer, buffer_len, "%F", tmp_timeinfo);
	std::string min = buffer;

	tmp_timeinfo = localtime(&max_date);
	strftime(buffer, buffer_len, "%F", tmp_timeinfo);
	std::string max = buffer;

	return make_tuple(min, max);
}

void TmcData::query(std::vector<struct TmcResult*>& out,
					float northEastLat,
					float northEastLng,
					float southWestLat,
					float southWestLng,
					string start_date,
					string end_date,
					string start_time,
					string end_time) {

	time_t t_start_date = getDate(start_date, false);
	time_t t_end_date = getDate(end_date, true);
	int i_start_time = std::stoi(start_time);
	int i_end_time = std::stoi(end_time);

	std::vector<int> indexes = getGridIndexes(northEastLat, northEastLng, southWestLat, southWestLng);

	std::vector<long> events = std::vector<long>(0);
	for (std::vector<int>::iterator i = indexes.begin(); i != indexes.end(); ++i)
	{
		std::vector<long> new_events = getEventsOfIndex(t_start_date, t_end_date, i_start_time, i_end_time, *i);
		events.insert( events.end(), new_events.begin(), new_events.end() );
	}
	std::sort(events.begin(), events.end(), [&](long i, long j) { return event_start.at(i) < event_start.at(j); });

	for (std::vector<long>::iterator i = events.begin(); i != events.end(); ++i)
	{
		TmcResult *tmp = new TmcResult();
		tmp->event = event_desc.at(event_type.at(*i));
		tmp->start = formatTime(event_start.at(*i));
		tmp->end = formatTime(event_end.at(*i));
		std::vector<int> path = getPath(event_lcd.at(*i), event_extension.at(*i), event_dir_negative.at(*i));
		for (std::vector<int>::iterator j = path.begin(); j != path.end(); ++j) {
			tmp->path.push_back(make_pair(lcd_x.at(*j), lcd_y.at(*j)));
		}
		out.push_back(tmp);
	}
}

time_t TmcData::getDate(std::string s, bool add_day) {
	struct tm tmp_time = {0};
	strptime(s.c_str(), "%F", &tmp_time);
	tmp_time.tm_hour += 1;
	if (add_day) {
		tmp_time.tm_mday += 1;
	}
	return mktime(&tmp_time);
}

std::string TmcData::formatTime(time_t rawtime) {
	struct tm *timeinfo;
	char buffer[20];

	timeinfo = gmtime(&rawtime);

	strftime(buffer,sizeof(buffer),"%F %T",timeinfo);
	return buffer;
}

std::vector<int> TmcData::getGridIndexes(float northEastLat, float northEastLng, float southWestLat, float southWestLng) {
	std::vector<int> tmp = std::vector<int>(0);
	// cout << "x:" << getXIndex(southWestLat) << "-" << getXIndex(northEastLat) << endl;
	// cout << "y:" << getYIndex(southWestLng) << "-" << getYIndex(northEastLng) << endl;
	for (int i = getYIndex(southWestLng); i < getYIndex(northEastLng); ++i) {
		for (int j = getXIndex(southWestLat); j < getXIndex(northEastLat); ++j) {
			if (i >= 0 && j >= 0 && i < grid_size_y && j < grid_size_x ) {
				tmp.push_back(getGridIndex(j, i));
			}
		}
	}
	return tmp;
}

int TmcData::getGridIndex(int x, int y) {
	return y * grid_size_x + x;
}

int TmcData::getXIndex(float x) {
	float percent = (x - lcd_x_min) / x_range;
	return (percent * grid_size_x) - 1;
}

int TmcData::getYIndex(float y) {
	float percent = (y - lcd_y_min) / y_range;
	return (percent * grid_size_y) - 1;
}

vector<long> TmcData::getEventsOfIndex(time_t start_date, time_t end_date, int start_time, int end_time, int grid_id) {
	vector<long> cell = grid.at(grid_id);
	vector<long>::iterator begin, end;

	begin = events_upper_bound(cell.begin(), cell.end(), start_date);
	end = events_lower_bound(cell.begin(), cell.end(), end_date);

	// if no hour is selected
	if (start_time == end_time){
		return vector<long>(begin, end);
	} else {
		// else only specific hours
		vector<long> result(0);
		vector<vector<pair<long,int>>> hours = hour_index.at(grid_id);
		// iterate over all wanted hours
		for (int hour = start_time; hour != end_time; hour = (hour + 1) % 24) {
			vector<pair<long,int>> tmp = hours.at(hour);
			// search for intervall in hour_index which should be used
			long cell_index_begin = distance(cell.begin(), begin);
			long cell_index_end = distance(cell.begin(), end);
			vector<pair<long,int>>::iterator hour_begin, hour_end;
			auto compare = [](const pair<long,int> &l, const pair<long,int> &r)->bool{ return l.first <= r.first; };
			hour_begin = upper_bound(tmp.begin(), tmp.end(), make_pair(cell_index_begin, 0), compare);
			hour_end = lower_bound(tmp.begin(), tmp.end(), make_pair(cell_index_end, 0), compare);

			// append all events based on the hour index
			for (vector<pair<long,int>>::iterator i = hour_begin; i != hour_end; ++i)
			{
				vector<long>::iterator first_event = cell.begin() + i->first;
				result.insert(result.end(), first_event, first_event + i->second);
			}
		}
		return result;
	}
	
}

vector<int> TmcData::getPath(int start_lcd, int extension, bool is_negative) {
	vector<int> path =  std::vector<int>(0);
	int tmp = start_lcd;
	path.push_back(start_lcd);
	for (int i = 0; i < extension; ++i)
	{
		if (is_negative) {
			tmp = offset_neg.at(tmp);
		} else {
			tmp = offset_pos.at(tmp);
		}
		path.push_back(tmp);
	}
	return path;
}

vector<long>::iterator TmcData::events_upper_bound (std::vector<long>::iterator first, std::vector<long>::iterator last, const long& val)
{
	vector<long>::iterator it;
	iterator_traits<std::vector<long>::iterator>::difference_type count, step;
	count = distance(first, last);
	while (count > 0)
	{
		it = first;
		step = count / 2;
		advance (it, step);
		if (!(val < event_start.at(*it))) {
			first = ++it; count -= step + 1;
		}
		else count = step;
	}
	return first;
}

vector<long>::iterator TmcData::events_lower_bound (std::vector<long>::iterator first, std::vector<long>::iterator last, const long& val)
{
	std::vector<long>::iterator it;
	iterator_traits<std::vector<long>::iterator>::difference_type count, step;
	count = distance(first, last);
	while (count > 0)
	{
		it = first;
		step = count / 2;
		advance (it, step);
		if (event_start.at(*it) < val) {
			first = ++it;
			count -= step + 1;
		}
		else count = step;
	}
	return first;
}