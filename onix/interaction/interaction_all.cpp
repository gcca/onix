#include <iostream>
#include <sstream>
#include <vector>

#include <libpq-fe.h>

#include "interaction_all.hpp"

#include <absl/status/status.h>
#include <absl/status/statusor.h>

namespace {

static inline void CreateIfNotExists(PGconn *conn) {
  if (PGresult *res =
          PQexec(conn, "CREATE TABLE IF NOT EXISTS onix_interaction_contacts ("
                       "id SERIAL PRIMARY KEY, "
                       "name VARCHAR(100), "
                       "email VARCHAR(100), "
                       "phone VARCHAR(20))")) {
    PQclear(res);
  }
}

class RepositoryPq {
protected:
  static inline PGconn *Connect() {
    static const char *host = "localhost", *db = "onix", *user = "", *pass = "",
                      *port = "5432";

    std::ostringstream oss;
    oss << "host=" << host << " dbname=" << db << " port=" << port;

    PGconn *conn = PQconnectdb(oss.str().c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
      std::cerr << "Connection to database failed: " << PQerrorMessage(conn)
                << std::endl;
      return nullptr;
    }

#ifdef ONIX_AUTOMIGRATE
    CreateIfNotExists(conn);
#endif

    return conn;
  }
};

} // namespace

namespace onix::domain::models {

class PqContactRepository : public ContactRepository, RepositoryPq {
public:
  explicit PqContactRepository() {}

  absl::Status Store(const Contact &contact) noexcept final {
    PGconn *conn = Connect();

    std::ostringstream query;
    query << "INSERT INTO onix_interaction_contacts (name, email, phone) "
             "VALUES ('"
          << contact.name << "', '" << contact.email << "', '" << contact.phone
          << "')";

    if (PGresult *res = PQexec(conn, query.str().c_str())) {
      PQclear(res);
    }

    PQfinish(conn);

    return absl::OkStatus();
  }

  absl::StatusOr<std::vector<Contact>> All() const noexcept final {
    PGconn *conn = Connect();

    PGresult *res = PQexec(
        conn, "SELECT name, email, phone FROM onix_interaction_contacts");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      PQclear(res);
      PQfinish(conn);

      std::ostringstream oss;
      oss << "Query failed: " << PQerrorMessage(conn);

      return absl::InternalError(oss.str());
    }

    int len = PQntuples(res);

    std::vector<Contact> contacts;
    contacts.reserve(len);

    for (int i = 0; i < len; ++i)
      contacts.emplace_back(PQgetvalue(res, i, 0), PQgetvalue(res, i, 1),
                            PQgetvalue(res, i, 1));

    PQclear(res);
    PQfinish(conn);

    return std::move(contacts);
  }
};

std::unique_ptr<ContactRepository> ContactRepository::Make() {
  return std::make_unique<PqContactRepository>();
}

} // namespace onix::domain::models
