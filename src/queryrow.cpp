#include "queryrow.hpp"

Queryrow::Queryrow(std::string tmp) : values(tmp) {
    while (true) {
        std::string::size_type pos = this->values.find(" ", 0);
        if (pos == std::string::npos) {
            tokens.push_back(this->values);
            break;
        }

        tokens.push_back(this->values.substr(0, pos));
        this->values.erase(0, pos + 1);
    }
}

Queryrow::~Queryrow() { }

std::vector <std::string> Queryrow::fetch() {
    return this->tokens;
}
