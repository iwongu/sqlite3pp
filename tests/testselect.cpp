#include <string>
#include <iostream>
#include "sqlite3pp.h"

using namespace std;

int main(int argc, char* argv[])
{
  try {
    sqlite3pp::database db("test.db");

    sqlite3pp::transaction xct(db, true);

    {
      sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");

      for (int i = 0; i < qry.column_count(); ++i) {
	cout << qry.column_name(i) << "\t";
      }
      cout << endl;

      for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
	for (int j = 0; j < qry.column_count(); ++j) {
	  cout << (*i).get<char const*>(j) << "\t";
	}
	cout << endl;
      }
      cout << endl;

      qry.reset();

      for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
	int id;
	char const* name, *phone;
	boost::tie(id, name, phone) = (*i).get_columns<int, char const*, char const*>(0, 1, 2);
	cout << id << "\t" << name << "\t" << phone << endl;
      }
      cout << endl;

      qry.reset();

      for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
	int id;
	std::string name, phone;
	(*i).getter() >> sqlite3pp::ignore >> name >> phone;
	cout << id << "\t" << name << "\t" << phone << endl;
      }
    }
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
}
