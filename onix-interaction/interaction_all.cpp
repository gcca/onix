#include <libpq-fe.h>

#include "interaction_all.hpp"

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_cat.h>

namespace {

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

} // namespace

namespace onix::interaction::domain::models {

class ItPq final : public Contacts::It {
public:
  ItPq() : res_{nullptr}, idx_{0} {}
  ItPq(PGresult *res) : res_{res}, idx_{0} {}
  ItPq(PGresult *res, int idx) : res_{res}, idx_{idx} {}

  Contact operator*() const noexcept final {
    return Contact(PQgetvalue(res_, idx_, 0),
                   PQgetvalue(res_, idx_, 1),
                   PQgetvalue(res_, idx_, 2));
  }

  ItPq &operator++() noexcept final {
    ++idx_;
    return *this;
  }

  bool operator!=(const It &other) const noexcept final {
    return idx_ != static_cast<const ItPq &>(other).idx_;
  }

private:
  PGresult *res_;
  int idx_;
};

namespace {

class ContactsPq final : public Contacts {
public:
  explicit ContactsPq(PGconn *conn, PGresult *res) : conn_{conn}, res_{res} {}

  Iterator begin() const noexcept final { return std::make_unique<ItPq>(res_); }

  Iterator end() const noexcept final {
    return std::make_unique<ItPq>(res_, PQntuples(res_));
  }

private:
  PGconn *conn_;
  PGresult *res_;
};

} // namespace

Contact Contacts::Iterator::operator*() const noexcept { return **it_; }

Contacts::Iterator &Contacts::Iterator::operator++() noexcept {
  ++*it_;
  return *this;
}

bool Contacts::Iterator::operator!=(
    const Contacts::Iterator &other) const noexcept {
  return *it_ != *other.it_;
}

#define ConnectPG()                                                            \
  Connect();                                                                   \
  if (PQstatus(conn) != CONNECTION_OK) {                                       \
    return absl::InternalError(absl::StrCat("Connection to database failed: ", \
                                            PQerrorMessage(conn)));            \
  }

#define ExecPQ(conn, q, status)                                                \
  PQexec(conn, (q));                                                           \
  if (!res) {                                                                  \
    PQfinish(conn);                                                            \
    return absl::InternalError("Query execution failed");                      \
  }                                                                            \
  if (PQresultStatus(res) != status) {                                         \
    const std::string s =                                                      \
        absl::StrCat("Query result failed: ", PQerrorMessage(conn));           \
    PQclear(res);                                                              \
    PQfinish(conn);                                                            \
    return absl::InternalError(s);                                             \
  }

#define ExecParamsPQ(conn, q, n, vals, lens, status)                           \
  PQexecParams(conn, (q), n, nullptr, vals, lens, nullptr, 0);                 \
  if (!res) {                                                                  \
    PQfinish(conn);                                                            \
    return absl::InternalError("Query execution failed");                      \
  }                                                                            \
  if (PQresultStatus(res) != status) {                                         \
    const std::string s =                                                      \
        absl::StrCat("Query result failed: ", PQerrorMessage(conn));           \
    PQclear(res);                                                              \
    PQfinish(conn);                                                            \
    return absl::InternalError(s);                                             \
  }

#define ExecInsertPQ(conn, q, n, vals, lens)                                   \
  ExecParamsPQ(conn, q, n, vals, lens, PGRES_COMMAND_OK);                      \
  PQclear(res);                                                                \
  PQfinish(conn)

#define ExecSelectPQ(conn, q) ExecPQ(conn, q, PGRES_TUPLES_OK)

class PqContactRepository : public ContactRepository, RepositoryPq {
public:
  explicit PqContactRepository() {}

  absl::Status Store(const Contact &contact) noexcept final {
    PGconn *conn = ConnectPG();

    std::array<const char *, 3> values = {
        contact.name.data(), contact.email.data(), contact.phone.data()};
    std::array<int, 3> lengths = {static_cast<int>(contact.name.size()),
                                  static_cast<int>(contact.email.size()),
                                  static_cast<int>(contact.phone.size())};

    PGresult *res = ExecInsertPQ(conn,
                                 "INSERT INTO onix_interaction_contact"
                                 " (name, email, phone) VALUES ($1, $2, $3)",
                                 3,
                                 values.data(),
                                 lengths.data());

    return absl::OkStatus();
  }

  absl::StatusOr<std::unique_ptr<Contacts>> All() const noexcept final {
    PGconn *conn = ConnectPG();

    PGresult *res = ExecSelectPQ(
        conn, "SELECT name, email, phone FROM onix_interaction_contact");

    return std::make_unique<ContactsPq>(conn, res);
  }
};

class PqTypeRepository : public TypeRepository, RepositoryPq {
public:
  explicit PqTypeRepository() {}

  absl::Status Store(const Type &type) noexcept final {
    PGconn *conn = ConnectPG();

    std::array<const char *, 1> values = {type.name.data()};
    std::array<int, 1> lengths = {static_cast<int>(type.name.size())};

    PGresult *res =
        ExecInsertPQ(conn,
                     "INSERT INTO onix_interaction_type (name) VALUES ($1)",
                     1,
                     values.data(),
                     lengths.data());

    return absl::OkStatus();
  }

  absl::StatusOr<std::vector<std::pair<std::size_t, std::string_view>>>
  Search(const std::string_view &name) const noexcept {
    PGconn *conn = ConnectPG();

    std::string name_match = "%" + std::string(name) + "%";

    std::array<const char *, 1> values = {name_match.data()};
    std::array<int, 1> lengths = {static_cast<int>(name_match.size())};

    PGresult *res = ExecParamsPQ(
        conn,
        "SELECT id, name FROM onix_interaction_type WHERE name ILIKE $1",
        1,
        values.data(),
        lengths.data(),
        PGRES_TUPLES_OK);

    std::vector<std::pair<std::size_t, std::string_view>> types;
    for (int i = 0; i < PQntuples(res); ++i) {
      types.emplace_back(std::stoul(PQgetvalue(res, i, 0)),
                         PQgetvalue(res, i, 1));
    }

    PQclear(res);
    PQfinish(conn);

    return types;
  }
};

std::unique_ptr<ContactRepository> ContactRepository::Make() {
  return std::make_unique<PqContactRepository>();
}

std::unique_ptr<TypeRepository> TypeRepository::Make() {
  return std::make_unique<PqTypeRepository>();
}

} // namespace onix::interaction::domain::models
