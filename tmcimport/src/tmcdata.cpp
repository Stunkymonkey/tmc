#include "tmcdata.h"
#include <boost/tokenizer.hpp>
#include <iostream>

using namespace std;

TmcData::TmcData()
{
}

TmcData::~TmcData()
{
}

void TmcData::addChunk(string new_string) {
	// this function checks what updates are in the new string
	typedef boost::tokenizer<boost::char_separator<char> >tokenizer;
	boost::char_separator<char> sep("\n");
	tokenizer tokens(new_string, sep);

	tokenizer::iterator tok_iter = tokens.begin();
	string header = *tok_iter;
	tok_iter++;

	// TODO parse header and extract time or create it

	vector<string>::iterator it = old_strings.begin();

	while (it != old_strings.end()) {
		if (*it == *tok_iter){
			tok_iter++;
			it++;
		} else {
			// removed lines
			cout << "removed: " << *it << endl;
			old_strings.erase(it);
		}
	}
	while (tok_iter != tokens.end()) {
		// new lines
		old_strings.push_back(*tok_iter);
		cout << "new: " << *tok_iter << endl;
		tok_iter++;
	}
}
