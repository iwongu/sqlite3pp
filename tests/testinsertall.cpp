#include <iostream>
#include "sqlite3pp.h"

using namespace std;

int main(int argc, char* argv[])
{
  try {
    sqlite3pp::database db("test.db");
    {
      sqlite3pp::transaction xct(db);
      {
	sqlite3pp::command cmd(db,
			       "INSERT INTO contacts (name, phone) VALUES (:name, '1234');"
			       "INSERT INTO contacts (name, phone) VALUES (:name, '5678');"
			       "INSERT INTO contacts (name, phone) VALUES (:name, '9012');"
			       );
	{
	  cout << cmd.bind(":name", "user") << endl;
	  cout << cmd.execute_all() << endl;
	}
      }
      xct.commit();
    }
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }

}
