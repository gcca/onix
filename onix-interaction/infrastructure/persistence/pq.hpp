#pragma once

#include <sstream>

#include <libpq-fe.h>

namespace onix::interaction::infrastructure::persistence::pq {

class RepositoryPq {
protected:
  static inline PGconn *Connect() {
    static const char *host = "localhost", *db = "onix", *user = "", *pass = "",
                      *port = "5432";

    std::ostringstream oss;
    oss << "host=" << host << " dbname=" << db << " port=" << port;

    PGconn *conn = PQconnectdb(oss.str().c_str());

    return conn;
  }
};

} // namespace onix::interaction::infrastructure::persistence::pq
