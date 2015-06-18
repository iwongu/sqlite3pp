#pragma once 
#include <cstddef> 

namespace sqlite3pp { namespace details {

    // if our string is longer than what an int can hold, truncate it.
    int trim_strlen(const std::size_t len);
}}
