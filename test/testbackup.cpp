#include <iostream>
#include "sqlite3pp.h"

using namespace std;

int main()
{
  try {
    sqlite3pp::database db("test.db");
    sqlite3pp::database backupdb("backup.db");

    db.backup(
      backupdb,
      [](int pagecount, int remaining, int rc) {
        cout << pagecount << "/" << remaining << endl;
        if (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
          // sleep(250);
        }
      });
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }

}
