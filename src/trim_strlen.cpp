#include "./trim_strlen.h"
#include <limits>

namespace sqlite3pp { namespace details {

    int trim_strlen(const std::size_t len)
    {
        return
            len >= std::numeric_limits<int>::max()
                ?  std::numeric_limits<int>::max()
                :  static_cast<int>(len);
    }
}}
