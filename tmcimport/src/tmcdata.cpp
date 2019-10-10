#include <iostream>
#include "tmcdata.h"

using namespace std;

void TmcData::init() {
	int grid_size = grid_size_x * grid_size_y;
	lcd_x = vector<float>(pow(2, 16));
	lcd_y = vector<float>(pow(2, 16));
	offset_pos = vector<uint16_t>(pow(2, 16));
	offset_neg = vector<uint16_t>(pow(2, 16));
	event_desc = vector<string>(pow(2, 11));
	grid = vector<vector<long>>(grid_size);
	grid_ids = std::vector<int>(pow(2, 16));
	hour_index = vector<vector<vector<pair<long,int>>>>(pow(2, 16),vector<vector<pair<long,int>> >(24,vector <pair<long,int>>(0)));
}

void TmcData::generateGrid() {
	x_range = lcd_x_max - lcd_x_min;
	y_range = lcd_y_max - lcd_y_min;
	for (int i = 0; i < lcd_x.size(); ++i)
	{
		float x = lcd_x[i];
		float y = lcd_y[i];
		if (x != 0.0f && y != 0.0f) {
			grid_ids.at(i) = getGridIndex(getXIndex(x), getYIndex(y));
		} else {
			grid_ids.at(i) = -1;
		}
	}
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

void TmcData::insertLcd(int id, float x, float y) {
	lcd_x.at(id) = x;
	if (x < lcd_x_min) {
		lcd_x_min = x;
	} else if (x > lcd_x_max) {
		lcd_x_max = x;
	}
	lcd_y.at(id) = y;
	if (y < lcd_y_min) {
		lcd_y_min = y;
	} else if (y > lcd_y_max) {
		lcd_y_max = y;
	}
	return;
}

void TmcData::insertOffset(int lcd, int neg, int pos) {
	offset_neg.at(lcd) = neg;
	offset_pos.at(lcd) = pos;
}

void TmcData::insertEventType(int eventcode, string desc) {
	event_desc.at(eventcode) = desc;
}

void TmcData::updateMinMaxDate(time_t time) {
	if (time < min_date) {
		min_date = time;
	} else if (time > max_date) {
		max_date = time;
	}
}

int TmcData::startSingleEvent(time_t time, int loc, int event, int ext, bool dir) {
	long index = event_lcd.size();
	int grid_id = grid_ids.at(loc);
	// check if location code is defined
	if (grid_id == -1) {
		return -1;
	}
	vector<long> cell = grid.at(grid_id);
	vector<long>::iterator begin, end;
	begin = events_lower_bound(cell.begin(), cell.end(), time);
	end = events_upper_bound(cell.begin(), cell.end(), time);
	if (isEventExistent(begin, end, time, loc, event, ext, dir) && !add_duplicate_events) {
		cout << "warning event already imported. skipping" << endl;
		return -1;
	}
	event_start.push_back(time);
	event_end.push_back(-1);
	event_lcd.push_back(loc);
	event_type.push_back(event);
	event_extension.push_back(ext);
	event_dir_negative.push_back(dir);
	// add to correct time index
	cell.insert(end, index);
	grid.at(grid_id) = cell;
	updateMinMaxDate(time);
	return index;
}

void TmcData::endSingleEvent(int index, time_t time, int loc, int ext, bool dir) {
	// it has been ended before
	// TODO: the if should be removed : fix in tmcfilter
	if (index == -1) { 
		return;
	}
	if (event_end.at(index) == -1) {
		event_end.at(index) = time;
		updateMinMaxDate(time);
	}
}

int TmcData::startGroupEvent(time_t time, int loc, int event, int ext, bool dir) {
	// return id of event, because it is needed for additional group infos and end
	int event_id = startSingleEvent(time, loc, event, ext, dir);
	if (event_id == -1) {
		return -1;
	}
	additional_event_info.resize(event_id + 1);
	std::vector<uint16_t> additional_info(0);
	additional_event_info.at(event_id) = additional_info;
	return event_id;
}

void TmcData::endGroupEvent(int index, time_t time, int loc, int ext, bool dir) {
	endSingleEvent(index, time, loc, ext, dir);
}

void TmcData::addGroupEventInfo(int id, int gsi, int f1, int f2) {
	if (id == -1) {
		return;
	}
	//additional_event_info.at(id).push_back(gsi);
	additional_event_info.at(id).push_back(f1);
	additional_event_info.at(id).push_back(f2);
}

bool TmcData::isEventExistent(vector<long>::iterator begin, vector<long>::iterator end, time_t time, int loc, int event, int ext, bool dir) {
	// check if event is already in system
	for (; begin != end; ++begin) {
		if (loc == event_lcd.at(*begin) && event == event_type.at(*begin) && ext == event_extension.at(*begin) && dir == event_dir_negative.at(*begin)) {
			return true;
		}
	}
	return false;
}

void TmcData::generateHourIndex() {
	// iterate all cells
	// iterate all events
	// get hour
	// then check last item if index + offset == new_index -1
	// if yes increase offset by one
	// else add new pair with size 1 to the vector
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