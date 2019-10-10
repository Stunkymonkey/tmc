#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

class TmcData{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & grid_size_x;
		ar & grid_size_y;

		ar & grid;
		ar & grid_ids;

		ar & lcd_x;
		ar & lcd_y;

		ar & lcd_x_min;
		ar & lcd_x_max;
		ar & lcd_y_min;
		ar & lcd_y_max;
		ar & x_range;
		ar & y_range;

		ar & offset_pos;
		ar & offset_neg;

		ar & event_desc;

		ar & event_start;
		ar & event_end;
		ar & event_lcd;
		ar & event_type;
		ar & event_extension;
		ar & event_dir_negative;

		ar & additional_event_info;
		ar & hour_index;

		ar & min_date;
		ar & max_date;
	}
public:
	void init();
	void setGridSize(int grid_x, int grid_y) { grid_size_x = grid_x; grid_size_y = grid_y; }
	void AddDuplicateEvents(bool tmp) { add_duplicate_events = tmp; }
	void generateGrid();
	int getGridIndex(int x, int y);
	int getXIndex(float x);
	int getYIndex(float y);
	void updateMinMaxDate(time_t time);
	void generateHourIndex();
	// LCL based
	void insertLcd(int id, float x, float y);
	void insertOffset(int lcd, int neg, int pos);
	void insertEventType(int eventcode, std::string desc);
	// event based
	int startSingleEvent(time_t time, int loc, int event, int ext, bool dir);
	void endSingleEvent(int index, time_t time, int loc, int ext, bool dir);
	int startGroupEvent(time_t time, int loc, int event, int ext, bool dir);
	void endGroupEvent(int index, time_t time, int loc, int ext, bool dir);
	void addGroupEventInfo(int id, int gsi, int f1, int f2);

private:
	std::vector<long>::iterator events_upper_bound (std::vector<long>::iterator first, std::vector<long>::iterator last, const long& val);
	std::vector<long>::iterator events_lower_bound (std::vector<long>::iterator first, std::vector<long>::iterator last, const long& val);
	bool isEventExistent(std::vector<long>::iterator begin, std::vector<long>::iterator end, time_t time, int loc, int event, int ext, bool dir);

	bool add_duplicate_events;

	int grid_size_x;
	int grid_size_y;

	std::vector<std::vector<long>> grid;
	std::vector<int> grid_ids;

	std::vector<float> lcd_x;
	std::vector<float> lcd_y;

	float lcd_x_min = std::numeric_limits<float>::max();
	float lcd_x_max = std::numeric_limits<float>::min();
	float lcd_y_min = std::numeric_limits<float>::max();
	float lcd_y_max = std::numeric_limits<float>::min();
	float x_range;
	float y_range;

	std::vector<uint16_t> offset_pos;
	std::vector<uint16_t> offset_neg;

	std::vector<std::string> event_desc;

	std::vector<time_t> event_start;
	std::vector<time_t> event_end;
	std::vector<int> event_lcd;
	std::vector<int> event_type;
	std::vector<uint8_t> event_extension;
	std::vector<bool> event_dir_negative;

	std::vector<std::vector<uint16_t>> additional_event_info;

	std::vector<std::vector<std::vector<std::pair<long,int>>>> hour_index;
	// cell_id, hour, (vector, offset)

	time_t min_date = std::numeric_limits<long>::max();
	time_t max_date = std::numeric_limits<long>::min();
};

BOOST_CLASS_VERSION(TmcData, 1);
