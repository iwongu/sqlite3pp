#include <string>
#include <iostream>
#include <boost/lambda/lambda.hpp>
#include "sqlite3pp.h"
#include "sqlite3ppext.h"

using namespace std;
using namespace boost::lambda;

int test0()
{
  return 100;
}

void test1(sqlite3pp::ext::context& ctx)
{
  ctx.result(200);
}

void test2(sqlite3pp::ext::context& ctx)
{
  std::string args = ctx.get<std::string>(0);
  ctx.result(args);
}

void test3(sqlite3pp::ext::context& ctx)
{
  ctx.result_copy(0);
}

std::string test5(std::string const& value)
{
  return value;
}

std::string test6(std::string const& s1, std::string const& s2, std::string const& s3)
{
  return s1 + s2 + s3;
}

int main(int argc, char* argv[])
{
  try {
    sqlite3pp::database db("test.db");

    sqlite3pp::ext::function func(db);
    cout << func.create<int ()>("h0", &test0) << endl;
    cout << func.create("h1", &test1) << endl;
    cout << func.create("h2", &test2, 1) << endl;
    cout << func.create("h3", &test3, 1) << endl;
    cout << func.create<int ()>("h4", constant(500)) << endl;
    cout << func.create<int (int)>("h5", _1 + constant(1000)) << endl;
    cout << func.create<string (string, string, string)>("h6", &test6) << endl;

    sqlite3pp::query qry(db, "SELECT h0(), h1(), h2('x'), h3('y'), h4(), h5(10), h6('a', 'b', 'c')");

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
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
}
