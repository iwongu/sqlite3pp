// $ g++ testdb.cpp -I ../headeronly_src -lsqlite3 --std=c++11
// $ g++ testdb.cpp -I ../headeronly_src -lsqlite3 --std=c++17

#include <iostream>
#include "sqlite3pp.h"
#include "sqlite3ppext.h"

using namespace std;

#define expect_true(arg) \
  do { \
    if(!(arg)) { \
      std::cout << "Unexpected false at " \
		<< __FILE__ << ", " << __LINE__ << ", " << __func__ << ": " \
		<< #arg << std::endl; }	\
  } while(false);

#define expect_eq(arg1, arg2) \
  do { \
    if(!((arg1) == (arg2))) {		  \
      std::cout << "Unexpected false at " \
		<< __FILE__ << ", " << __LINE__ << ", " << __func__ << ": " \
		<< #arg1 << ", " << #arg2 << std::endl; }		\
  } while(false);

sqlite3pp::database contacts_db();

void test_insert_execute() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> name >> phone;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
}

void test_insert_execute_all() {
  auto db = contacts_db();
  sqlite3pp::command cmd(
    db,
    "INSERT INTO contacts (name, phone) VALUES (:user, '555-0000');"
    "INSERT INTO contacts (name, phone) VALUES (:user, '555-1111');"
    "INSERT INTO contacts (name, phone) VALUES (:user, '555-2222')");
  cmd.bind(":user", "Mike", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute_all());

  sqlite3pp::query qry(db, "SELECT COUNT(*) FROM contacts");
  auto iter = qry.begin();
  int count = (*iter).get<int>(0);
  expect_eq(3, count);
}

void test_insert_binder() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
  cmd.binder() << "Mike" << "555-1234";
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> name >> phone;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
}

void test_insert_bind1() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
  cmd.bind(1, "Mike", sqlite3pp::nocopy);
  cmd.bind(2, "555-1234", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> name >> phone;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
}

void test_insert_bind2() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?100, ?101)");
  cmd.bind(100, "Mike", sqlite3pp::nocopy);
  cmd.bind(101, "555-1234", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> name >> phone;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
}

void test_insert_bind3() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (:user, :phone)");
  cmd.bind(":user", "Mike", sqlite3pp::nocopy);
  cmd.bind(":phone", "555-1234", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> name >> phone;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
}

void test_insert_bind_null() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone, address) VALUES (:user, :phone, :address)");
  cmd.bind(":user", "Mike", sqlite3pp::nocopy);
  cmd.bind(":phone", "555-1234", sqlite3pp::nocopy);
  cmd.bind(":address");
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone, address FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  const char* address = nullptr;
  (*iter).getter() >> name >> phone >> address;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
  expect_eq(nullptr, address);
}

void test_insert_binder_null() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone, address) VALUES (?, ?, ?)");
  cmd.binder() << "Mike" << "555-1234" << nullptr;
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT name, phone, address FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  const char* address = nullptr;
  (*iter).getter() >> name >> phone >> address;
  expect_eq("Mike", name);
  expect_eq("555-1234", phone);
  expect_eq(nullptr, address);
}

void test_query_columns() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");
  expect_eq(3, qry.column_count());
  expect_eq(string("id"), qry.column_name(0));
  expect_eq(string("name"), qry.column_name(1));
  expect_eq(string("phone"), qry.column_name(2));
}

void test_query_get() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");
  auto iter = qry.begin();
  expect_eq(string("Mike"), (*iter).get<char const*>(1));
  expect_eq(string("555-1234"), (*iter).get<char const*>(2));
}

void test_query_tie() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");
  auto iter = qry.begin();
  char const* name, *phone;
  std::tie(name, phone) = (*iter).get_columns<char const*, char const*>(1, 2);
  expect_eq(string("Mike"), name);
  expect_eq(string("555-1234"), phone);
}

void test_query_getter() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");
  auto iter = qry.begin();
  string name, phone;
  (*iter).getter() >> sqlite3pp::ignore >> name >> phone;
  expect_eq(string("Mike"), name);
  expect_eq(string("555-1234"), phone);
}

void test_query_iterator() {
  auto db = contacts_db();
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");
  for (auto row : qry) {
    string name, phone;
    row.getter() >> sqlite3pp::ignore >> name >> phone;
    expect_eq(string("Mike"), name);
    expect_eq(string("555-1234"), phone);
  }
}

void test_function() {
  auto db = contacts_db();
  sqlite3pp::ext::function func(db);
  expect_eq(0, func.create<int ()>("test_fn", []{return 100;}));

  sqlite3pp::query qry(db, "SELECT test_fn()");
  auto iter = qry.begin();
  int count = (*iter).get<int>(0);
  expect_eq(100, count);
}

void test_function_args() {
  auto db = contacts_db();
  sqlite3pp::ext::function func(db);
  func.create<string (string)>("test_fn", [](const string& name){return "Hello " + name;});

  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));

  sqlite3pp::query qry(db, "SELECT name, test_fn(name) FROM contacts");
  auto iter = qry.begin();
  string name, hello_name;
  (*iter).getter() >> name >> hello_name;
  expect_eq(string("Mike"), name);
  expect_eq(string("Hello Mike"), hello_name);
}

struct strlen_aggr {
  void step(const string& s) {
    total_len += s.size();
  }
  
  int finish() {
    return total_len;
  }
  int total_len = 0;
};

void test_aggregate() {
  auto db = contacts_db();
  sqlite3pp::ext::aggregate aggr(db);
  aggr.create<strlen_aggr, string>("strlen_aggr");

  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')"));
  expect_eq(0, db.execute("INSERT INTO contacts (name, phone) VALUES ('Janette', '555-4321')"));

  sqlite3pp::query qry(db, "SELECT strlen_aggr(name), strlen_aggr(phone) FROM contacts");
  auto iter = qry.begin();
  expect_eq(11, (*iter).get<int>(0));
  expect_eq(16, (*iter).get<int>(1));
}

void test_invalid_path() {
  try {
    sqlite3pp::database db("/test/invalid/path");
  } catch (sqlite3pp::database_error& e) {
    return;
  }
  expect_true(false);
}

void test_reset() {
  auto db = contacts_db();
  sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (:user, :phone)");
  cmd.bind(":user", "Mike", sqlite3pp::nocopy);
  cmd.bind(":phone", "555-1234", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute());

  cmd.reset();
  cmd.bind(":user", "Janette", sqlite3pp::nocopy);
  expect_eq(0, cmd.execute());

  sqlite3pp::query qry(db, "SELECT COUNT(*) FROM contacts");
  auto iter = qry.begin();
  int count = (*iter).get<int>(0);
  expect_eq(2, count);

  cmd.reset();
  cmd.clear_bindings();
  cmd.bind(":user", "Dave", sqlite3pp::nocopy);
  expect_eq(SQLITE_CONSTRAINT, cmd.execute());
}

int main()
{
  test_insert_execute();
  test_insert_execute_all();
  test_insert_binder();
  test_insert_bind1();
  test_insert_bind2();
  test_insert_bind3();
  test_insert_bind_null();
  test_insert_binder_null();
  test_query_columns();
  test_query_get();
  test_query_tie();
  test_query_getter();
  test_query_iterator();
  test_function();
  test_function_args();
  test_aggregate();
  test_invalid_path();
  test_reset();
}

sqlite3pp::database contacts_db() {
  sqlite3pp::database db(":memory:");
  db.execute(R"""(
    CREATE TABLE contacts (
      id INTEGER PRIMARY KEY,
      name TEXT NOT NULL,
      phone TEXT NOT NULL,
      address TEXT,
      UNIQUE(name, phone)
    );
  )""");
  return db;
}

