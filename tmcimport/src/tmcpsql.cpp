#include <iostream>
#include <pqxx/pqxx>

#include "tmcpsql.h"

using namespace std;
using namespace pqxx;

TmcPsql::TmcPsql(string db_name, string user, string password, string hostaddr, string port) {
   try {
      connection C("dbname = testdb user = postgres password = cohondob \
      hostaddr = 127.0.0.1 port = 5432");
      if (C.is_open()) {
         cout << "Opened database successfully: " << C.dbname() << endl;
      } else {
         cout << "Can't open database" << endl;
         //return 1;
      }
      C.disconnect ();
   } catch (const std::exception &e) {
      cerr << e.what() << std::endl;
      //return 1;
   }
}

TmcPsql::~TmcPsql()
{
}

bool TmcPsql::insertEvent() {
   return true;
}

bool TmcPsql::insertLcd() {
   return true;
}