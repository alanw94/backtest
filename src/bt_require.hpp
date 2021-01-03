#ifndef _bt_require_hpp_
#define _bt_require_hpp_

#include <stdexcept>
#include <sstream>

#define bt_require(expr, message) \
{ \
  if (!(expr)) { \
    std::ostringstream bt_require_msg_oss; \
    bt_require_msg_oss << message; \
    throw std::runtime_error(bt_require_msg_oss.str()); \
  } \
}

#endif
