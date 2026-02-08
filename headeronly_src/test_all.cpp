#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "sqlite3pp.h"
#include "sqlite3ppext.h"

using namespace std;

void test_database_basic() {
    cout << "Testing database basic operations..." << endl;
    sqlite3pp::database db(":memory:");
    int rc = db.execute("CREATE TABLE test (id INTEGER PRIMARY KEY, val TEXT)");
    assert(rc == SQLITE_OK);

    rc = db.execute("INSERT INTO test (val) VALUES ('hello')");
    assert(rc == SQLITE_OK);
    assert(db.changes() == 1);
    assert(db.last_insert_rowid() == 1);
}

void test_database_move() {
    cout << "Testing database move semantics..." << endl;
    sqlite3pp::database db1(":memory:");
    db1.execute("CREATE TABLE test (id INTEGER PRIMARY KEY)");
    
    sqlite3pp::database db2(std::move(db1));
    int rc = db2.execute("INSERT INTO test VALUES (1)");
    assert(rc == SQLITE_OK);

    sqlite3pp::database db3;
    db3 = std::move(db2);
    rc = db3.execute("INSERT INTO test VALUES (2)");
    assert(rc == SQLITE_OK);
}

void test_statement_binding() {
    cout << "Testing statement binding..." << endl;
    sqlite3pp::database db(":memory:");
    db.execute("CREATE TABLE test (id INTEGER, f DOUBLE, t TEXT, b BLOB, n TEXT)");

    sqlite3pp::command cmd(db, "INSERT INTO test VALUES (?, ?, ?, ?, ?)");
    cmd.bind(1, 123);
    cmd.bind(2, 45.6);
    cmd.bind(3, "string", sqlite3pp::copy);
    cmd.bind(4, "blob", 4, sqlite3pp::copy);
    cmd.bind(5); // null
    assert(cmd.execute() == SQLITE_OK);

    // Named parameters
    sqlite3pp::command cmd2(db, "INSERT INTO test VALUES (:id, :f, :t, :b, :n)");
    cmd2.bind(":id", 456);
    cmd2.bind(":f", 78.9);
    cmd2.bind(":t", string("string2"), sqlite3pp::copy);
    cmd2.bind(":b", "blob2", 5, sqlite3pp::copy);
    cmd2.bind(":n", sqlite3pp::null_type());
    assert(cmd2.execute() == SQLITE_OK);

    // Bindstream
    sqlite3pp::command cmd3(db, "INSERT INTO test VALUES (?, ?, ?, ?, ?)");
    cmd3.binder() << 789 << 10.11 << "stream" << nullptr << nullptr;
    assert(cmd3.execute() == SQLITE_OK);
}

void test_query() {
    cout << "Testing query operations..." << endl;
    sqlite3pp::database db(":memory:");
    db.execute("CREATE TABLE test (id INTEGER, val TEXT)");
    db.execute("INSERT INTO test VALUES (1, 'one')");
    db.execute("INSERT INTO test VALUES (2, 'two')");

    sqlite3pp::query qry(db, "SELECT id, val FROM test ORDER BY id");
    int count = 0;
    for (auto i = qry.begin(); i != qry.end(); ++i) {
        auto row = *i;
        int id;
        string val;
        row.getter() >> id >> val;
        if (count == 0) {
            assert(id == 1);
            assert(val == "one");
        } else {
            assert(id == 2);
            assert(val == "two");
        }
        count++;
    }
    assert(count == 2);

    // get_columns with tuple
    sqlite3pp::query qry2(db, "SELECT id, val FROM test WHERE id = 1");
    auto it = qry2.begin();
    int id;
    string val;
    std::tie(id, val) = (*it).get_columns<int, string>(0, 1);
    assert(id == 1);
    assert(val == "one");
}

void test_transaction() {
    cout << "Testing transactions..." << endl;
    sqlite3pp::database db(":memory:");
    db.execute("CREATE TABLE test (id INTEGER)");

    {
        sqlite3pp::transaction xct(db);
        db.execute("INSERT INTO test VALUES (1)");
        // auto rollback
    }

    {
        sqlite3pp::query qry(db, "SELECT COUNT(*) FROM test");
        assert((*qry.begin()).get<int>(0) == 0);
    }

    {
        sqlite3pp::transaction xct(db);
        db.execute("INSERT INTO test VALUES (1)");
        xct.commit();
    }

    {
        sqlite3pp::query qry(db, "SELECT COUNT(*) FROM test");
        assert((*qry.begin()).get<int>(0) == 1);
    }
}

void test_extensions() {
    cout << "Testing extensions (functions and aggregates)..." << endl;
    sqlite3pp::database db(":memory:");
    
    // Function
    sqlite3pp::ext::function func(db);
    func.create<int (int, int)>("myadd", [](int a, int b) { return a + b; });
    func.create<string (string)>("myecho", [](string s) { return s; });
    func.create<void (int)>("mynoop", [](int) {});

    sqlite3pp::query qry(db, "SELECT myadd(1, 2), myecho('hello'), mynoop(1)");
    auto row = *qry.begin();
    assert(row.get<int>(0) == 3);
    assert(row.get<string>(1) == "hello");
    assert(row.column_type(2) == SQLITE_NULL);

    // Aggregate
    struct sum_aggr {
        void step(int n) { total += n; }
        int finish() { return total; }
        int total = 0;
    };

    sqlite3pp::ext::aggregate aggr(db);
    aggr.create<sum_aggr, int>("mysum");

    db.execute("CREATE TABLE nums (n INTEGER)");
    db.execute("INSERT INTO nums VALUES (10), (20), (30)");
    
    sqlite3pp::query qry_aggr(db, "SELECT mysum(n) FROM nums");
    assert((*qry_aggr.begin()).get<int>(0) == 60);
}

void test_hooks() {
    cout << "Testing hooks..." << endl;
    sqlite3pp::database db(":memory:");
    db.execute("CREATE TABLE test (id INTEGER)");

    bool updated = false;
    db.set_update_handler([&](int opcode, char const* dbname, char const* tablename, long long int rowid) {
        updated = true;
    });

    db.execute("INSERT INTO test VALUES (1)");
    assert(updated == true);

    // Test move with hooks
    updated = false;
    sqlite3pp::database db2 = std::move(db);
    db2.execute("INSERT INTO test VALUES (2)");
    assert(updated == true);
}

void test_errors() {
    cout << "Testing error handling..." << endl;
    sqlite3pp::database db(":memory:");
    
    // execute returns error code, doesn't throw
    int rc = db.execute("SELECT * FROM non_existent");
    assert(rc != SQLITE_OK);

    // statement/command constructor throws if prepare fails
    try {
        sqlite3pp::command cmd(db, "SYNTAX ERROR");
        assert(false);
    } catch (sqlite3pp::database_error& e) {
        cout << "Caught expected database_error: " << e.what() << endl;
    }

    // Multiple statements with mixed parameters (Issue #62)
    db.execute("CREATE TABLE issue62 (id INTEGER, val TEXT)");
    sqlite3pp::command cmd2(db, "INSERT INTO issue62 (id, val) VALUES (?, ?); DELETE FROM issue62 WHERE id = 100");
    cmd2.bind(1, 100);
    cmd2.bind(2, "temp", sqlite3pp::copy);
    int rc2 = cmd2.execute_all();
    assert(rc2 == SQLITE_OK);
    
    sqlite3pp::query qry(db, "SELECT COUNT(*) FROM issue62");
    assert((*qry.begin()).get<int>(0) == 0); // Inserted then deleted
}

void test_attach_backup() {
    cout << "Testing attach and backup..." << endl;
    sqlite3pp::database db1(":memory:");
    db1.execute("CREATE TABLE test1 (id INTEGER)");
    db1.execute("INSERT INTO test1 VALUES (100)");

    sqlite3pp::database db2(":memory:");
    db2.execute("CREATE TABLE test2 (id INTEGER)");
    db2.execute("INSERT INTO test2 VALUES (200)");

    // Test Backup
    int rc = db1.backup(db2);
    assert(rc == SQLITE_DONE || rc == SQLITE_OK);
    
    sqlite3pp::query qry(db2, "SELECT id FROM test1");
    assert((*qry.begin()).get<int>(0) == 100);

    // Test Attach
    sqlite3pp::database db3(":memory:");
    db3.execute("CREATE TABLE test3 (id INTEGER)");
    db3.execute("INSERT INTO test3 VALUES (300)");
    
    // We can't easily attach a ':memory:' db to another, but we can test the call
    rc = db1.attach(":memory:", "attached_db");
    assert(rc == SQLITE_OK);
}

void test_borrow() {
    cout << "Testing borrow..." << endl;
    sqlite3* pdb;
    int rc = sqlite3_open(":memory:", &pdb);
    assert(rc == SQLITE_OK);

    {
        sqlite3pp::database db = sqlite3pp::ext::borrow(pdb);
        db.execute("CREATE TABLE borrowed_test (id INTEGER)");
        db.execute("INSERT INTO borrowed_test VALUES (1)");
        // db destructor should not close pdb
    }

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(pdb, "SELECT id FROM borrowed_test", -1, &stmt, nullptr);
    assert(rc == SQLITE_OK);
    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    assert(sqlite3_column_int(stmt, 0) == 1);

    sqlite3_finalize(stmt);
    sqlite3_close(pdb);
}

int main() {
    try {
        test_database_basic();
        test_database_move();
        test_statement_binding();
        test_query();
        test_transaction();
        test_extensions();
        test_hooks();
        test_errors();
        test_attach_backup();
        test_borrow();
        cout << "All tests passed successfully!" << endl;
    } catch (exception& e) {
        cerr << "Test failed with exception: " << e.what() << endl;
        return 1;
    }
    return 0;
}
