#include <sstream>

#include <libpq-fe.h>

#include "shared.hpp"

namespace onix::lead::ws::shared {

PGconn *ConnectPG() {
  static const char *host = "localhost", *db = "onix", *user = "", *pass = "",
                    *port = "5432";

  std::ostringstream oss;
  oss << "host=" << host << " dbname=" << db << " port=" << port;

  PGconn *conn = PQconnectdb(oss.str().c_str());

  return conn;
}

} // namespace onix::lead::ws::shared
