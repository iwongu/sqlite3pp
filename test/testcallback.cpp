#include <exception>
#include <functional>
#include <iostream>
#include "sqlite3pp.h"

using namespace std;
using namespace std::placeholders;

int handle_authorize(int evcode, char const* p1, char const* p2, char const* dbname, char const* tvname);

struct handler
{
  handler() : cnt_(0) {}

  void handle_update(int opcode, char const* dbname, char const* tablename, long long int rowid) {
    cout << "handle_update(" << opcode << ", " << dbname << ", " << tablename << ", " << rowid << ") - " << cnt_++ << endl;
  }
  int cnt_;
};

int handle_authorize(int evcode, char const* /*p1*/, char const* /*p2*/, char const* /*dbname*/, char const* /*tvname*/) {
  cout << "handle_authorize(" << evcode << ")" << endl;
  return 0;
}

struct rollback_handler
{
  void operator()() {
    cout << "handle_rollback" << endl;
  }
};

int main()
{
  try {
    sqlite3pp::database db("test.db");

    {
      db.set_commit_handler([]{cout << "handle_commit\n"; return 0;});
      db.set_rollback_handler(rollback_handler());
    }

    handler h;

    db.set_update_handler(std::bind(&handler::handle_update, &h, _1, _2, _3, _4));

    db.set_authorize_handler(&handle_authorize);

    db.execute("INSERT INTO contacts (name, phone) VALUES ('AAAA', '1234')");

    {
      sqlite3pp::transaction xct(db);

      sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");

      cout << cmd.bind(1, "BBBB", sqlite3pp::copy) << endl;
      cout << cmd.bind(2, "1234", sqlite3pp::copy) << endl;
      cout << cmd.execute() << endl;

      cout << cmd.reset() << endl;

      cmd.binder() << "CCCC" << "1234";

      cout << cmd.execute() << endl;

      xct.commit();
    }

    {
      sqlite3pp::transaction xct(db);

      sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (:name, :name)");

      cout << cmd.bind(":name", "DDDD", sqlite3pp::copy) << endl;

      cout << cmd.execute() << endl;
    }
  }
  catch (std::exception& ex) {
    cout << ex.what() << endl;
  }

}
