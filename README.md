> #### PROJECT STATUS
> ##### Only the `headeronly_src` directory is currently maintained and updated. Other directories such as `src`, `boost_src`, and the top-level `test` directory are obsolete and kept only for historical reference.

sqlite3pp
=========

This library makes SQLite3 API more friendly to C++ users. It supports almost all of SQLite3 features using C++ classes such as database, command, query, and transaction. The query class supports iterator concept for fetching records.

With ext::function class, it's also easy to use the sqlite3's functions and aggregations in C++.

# Usage

## database
```cpp
// Open a database file or :memory:
sqlite3pp::database db("test.db");

// Simple execution
db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')");

// Move semantics
sqlite3pp::database db2 = std::move(db);
```

## command
```cpp
// Using stream-like binder (automatically uses sqlite3pp::copy)
sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
cmd.binder() << "Mike" << "555-1234";
cmd.execute();
```

```cpp
// Using positional binding with copy/nocopy semantics
sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
cmd.bind(1, "Mike", sqlite3pp::nocopy);
cmd.bind(2, "555-1234", sqlite3pp::nocopy);
cmd.execute();
```

```cpp
// Using named parameters
sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone, age) VALUES (:user, :phone, :age)");
cmd.bind(":user", "Mike", sqlite3pp::nocopy);
cmd.bind(":phone", "555-1234", sqlite3pp::copy);
cmd.bind(":age", 30);
cmd.execute();
```

```cpp
// Executing multiple statements (semicolon separated)
sqlite3pp::command cmd(
  db,
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-0000');"
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-1111');"
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-2222')");
cmd.bind(":user", "Mike", sqlite3pp::nocopy);
cmd.execute_all();
```

## transaction

```cpp
sqlite3pp::transaction xct(db);
{
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
  cmd.binder() << "Mike" << "555-1234";
  cmd.execute();
}
// Automatically rollbacks if not committed
xct.commit(); 
```

## query

```cpp
sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");

// Accessing metadata
for (int i = 0; i < qry.column_count(); ++i) {
  cout << qry.column_name(i) << "\t";
}
```

```cpp
// Using range-based for loop
for (auto row : qry) {
  int id;
  string name, phone;
  row.getter() >> id >> name >> phone;
  cout << id << "\t" << name << "\t" << phone << endl;
}
```

```cpp
// Using std::tie for multiple columns
for (auto row : qry) {
  int id;
  char const *name, *phone;
  std::tie(id, name, phone) = row.get_columns<int, char const*, char const*>(0, 1, 2);
  cout << id << "\t" << name << "\t" << phone << endl;
}
```

```cpp
// Using sqlite3pp::ignore to skip columns
for (auto row : qry) {
  string name, phone;
  row.getter() >> sqlite3pp::ignore >> name >> phone;
  cout << "\t" << name << "\t" << phone << endl;
}
```

## attach

```cpp
sqlite3pp::database db("main.db");
db.attach("other.db", "other");

sqlite3pp::query qry(db, "SELECT m.* FROM contacts m, other.contacts o WHERE m.id = o.id");
```

## backup

```cpp
sqlite3pp::database db("test.db");
sqlite3pp::database backupdb("backup.db");

// Simple backup
db.backup(backupdb);

// Backup with progress handler
db.backup(backupdb, [](int remaining, int pagecount, int rc) {
  cout << remaining << "/" << pagecount << " pages left..." << endl;
});
```

## callback hooks

```cpp
sqlite3pp::database db(":memory:");

db.set_commit_handler([]{ 
  cout << "Committed!\n"; 
  return 0; 
});

db.set_update_handler([](int opcode, char const* dbname, char const* tablename, long long int rowid) {
  cout << "Updated " << tablename << " at " << rowid << "\n";
});
```

## function (Extensions)

```cpp
sqlite3pp::ext::function func(db);

// Register a C++ lambda as a SQL function
func.create<int (int, int)>("cpp_add", [](int a, int b) { 
  return a + b; 
});

// Support for void return types (returns NULL to SQL)
func.create<void (string)>("log_text", [](string s) {
  clog << "SQL Log: " << s << endl;
});

// Use it in queries
sqlite3pp::query qry(db, "SELECT cpp_add(1, 2), log_text('hello')");
```

## aggregate (Extensions)

```cpp
struct sum_aggr
{
  void step(int n) {
    total_ += n;
  }
  int finish() {
    return total_;
  }
  int total_ = 0;
};

sqlite3pp::ext::aggregate aggr(db);
aggr.create<sum_aggr, int>("cpp_sum");

sqlite3pp::query qry(db, "SELECT cpp_sum(id) FROM contacts");
```

## loadable extension

```cpp
#define SQLITE3PP_LOADABLE_EXTENSION
#include <sqlite3ppext.h>

extern "C" int sqlite3_extension_init(
  sqlite3 *pdb,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi) {
  SQLITE_EXTENSION_INIT2(pApi);
  
  // Borrow the raw sqlite3 pointer
  sqlite3pp::database db(sqlite3pp::ext::borrow(pdb));
  
  // Register functions...
  return SQLITE_OK;
}
```

# How to run tests

To run the comprehensive test suite:

```bash
g++ -std=c++11 -Iheaderonly_src headeronly_src/test_all.cpp -lsqlite3 -o test_all && ./test_all
```

# Important Note
Only the files in `headeronly_src` directory are maintained. All other source and test directories (`src`, `boost_src`, `test`) are deprecated and should not be used for new projects.

# See also
* http://www.sqlite.org/
* https://github.com/iwongu/sqlite3pp/wiki
