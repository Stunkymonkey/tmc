#ifndef STDRDSQUERYHANDLER_H
#define STDRDSQUERYHANDLER_H

#include <string>
#include <vector>
#include <librds.h>

using namespace std;

const string RdsErrorStrings[] = {
	"Success, no errors.",
	"Error, reason is not clear.",
	"Connection type is neither CONN_TYPE_TCPIP nor CONN_TYPE_UNIX.",
	"The rdsd server could not be found.",
	"The socket() system call failed.",
	"The connect() system call failed.",
	"The open() system call failed.",
	"There is no valid socket file descriptor for a connection.",
	"The close() system call failed.",
	"Attempt to open a connection while there is still an open socket.",
	"The bind() system call failed.",
	"The chmod() system call failed.",
	"The write() system call failed.",
	"The read() system call failed.",
	"An illegal source number was given.",
	"An internal error in the command list. Must never happen!.",
	"There was no response from rdsd within the time limit.",
	"The response from rdsd is not what was expected.",
	"Attempt to set a timeout value that is too big or too small.",
	"Attempt to get/set the RX frequency of a source that is no radio.",
	"An illegal value for the tuner frequency was given.",
	"The requested feature is not implemented (yet)."
};


class RdsQueryHandler{
public:
	RdsQueryHandler();
	~RdsQueryHandler();
	void SetHandle(RDSConnectionHandle hnd) { handle = hnd; }
	RDSConnectionHandle GetHandle() { return handle; }
	void SetSourceNum(int src) { src_num = src; }
	int GetSourceNum() { return src_num; }
	void SetShowDebugOnError(bool Show) {show_debug_on_error = Show; } 
	bool GetShowDebugOnError() {return show_debug_on_error; }
	void ShowError(int rds_err_num);
	string ShowTMCList();
private:
	RDSConnectionHandle handle;
	bool infinite_record_count;
	int src_num;
	bool show_debug_on_error;
	void show_debug();
};

#endif
