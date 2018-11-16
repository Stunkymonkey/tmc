#include "tmcqueryhandler.h"
#include <iostream>
#include <cstdlib>

using namespace std;

RdsQueryHandler::RdsQueryHandler()
{
	handle = 0;
	src_num = -1;
	show_debug_on_error = true;
	infinite_record_count = true;
}

RdsQueryHandler::~RdsQueryHandler()
{
}

void RdsQueryHandler::ShowError(int rds_err_num)
{
	if ((rds_err_num>=0)&&(rds_err_num<=RDS_ILLEGAL_TIMEOUT))
		cout << RdsErrorStrings[rds_err_num] << endl;
	else
		cout << "Error #" << rds_err_num << endl;
	if (show_debug_on_error) show_debug();
}

string RdsQueryHandler::ShowTMCList()
{
	size_t bufsize=0;
	int ret = rds_get_tmc_buffer(handle,src_num,0, &bufsize);
	if (ret){
		ShowError(ret);
		return "";
	}
	if (bufsize>0){
		vector<char> buf(bufsize);
		ret = rds_get_tmc_buffer(handle,src_num,&buf[0], &bufsize);
		if (ret){
			ShowError(ret);
			return "";
		}
		string s(buf.begin(),buf.begin()+bufsize);
		return s;
	}
	return "";
}

void RdsQueryHandler::show_debug()
{
	size_t buf_size = 0;
	int ret = rds_get_debug_text(handle,0, buf_size); // query required size
	if (ret){
		ShowError(ret);
		return;
	}
	if (buf_size>0){
		vector<char> buf(buf_size);
		rds_get_debug_text(handle,&buf[0], buf_size);
		string s(buf.begin(),buf.begin()+buf_size);
		cerr << s << endl;
	}
	else cerr << "(No debug messages)" << endl;
}
