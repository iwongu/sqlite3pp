#include "queryresult.hpp"

Queryresult::Queryresult(sqlite3pp::query& qry) {
    std::string tmp = "";

    for (int i = 0; i < qry.column_count(); ++i) {
        headers.push_back(qry.column_name(i));
    }

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        for (int j = 0; j < qry.column_count(); ++j) {
            tmp += (*i).get<std::string>(j) + " ";
        }

        Queryrow qr(tmp);
        tmp = "";
        this->queryrows.push_back(qr);
    }
}

Queryresult::~Queryresult() { }

std::string Queryresult::operator()(unsigned int row, std::string s) {
    int column = std::distance(headers.begin(), std::find(headers.begin(), headers.end(), s));

    return this->queryrows[row].fetch()[column];
}

std::string Queryresult::operator()(unsigned int row, unsigned int column) {
    return this->queryrows[row].fetch()[column];
}

Queryrow Queryresult::operator[](unsigned int row) {
    return this->queryrows[row];
}

 std::size_t Queryresult::size() {
    return this->queryrows.size();
}
