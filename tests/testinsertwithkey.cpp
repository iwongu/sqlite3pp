#include <iostream>
#include "sqlite3pp.h"

using namespace std;

#define TEST_DB_KEY "secrect key"
#define TEST_DB_KEY_LEN (sizeof(TEST_DB_KEY) - 1)

int main(int argc, char* argv[])
{
  try {
    sqlite3pp::database db("test.db");
    db.key(TEST_DB_KEY, TEST_DB_KEY_LEN);

    {
      db.execute("CREATE TABLE contacts (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(128), phone VARCHAR(64));");

      db.execute("INSERT INTO contacts (name, phone) VALUES ('AAAA', '1234')");
    }

    {
      sqlite3pp::transaction xct(db);

      sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");

      cout << cmd.bind(1, "BBBB") << endl;
      cout << cmd.bind(2, "1234") << endl;
      cout << cmd.execute() << endl;

      cout << cmd.reset() << endl;

      cmd.binder() << "CCCC" << "1234";

      cout << cmd.execute() << endl;

      xct.commit();
    }

    {
      sqlite3pp::transaction xct(db, true);

      sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (:name, :name)");

      cout << cmd.bind(":name", "DDDD") << endl;

      cout << cmd.execute() << endl;
    }
  } catch (exception& ex) {
    cout << ex.what() << endl;
  }
}
// vim: set ts=2 sw=2 sts=2 et:
