#include <onix-core/pq.hpp>

#include "../../infrastructure/persistence/pq.hpp"
#include "contact.hpp"

using namespace onix::interaction::infrastructure::persistence::pq;
using namespace onix::interaction::domain::models::contact;
using namespace onix::core;

namespace {

class ContactsPq final : public Contacts {
public:
  explicit ContactsPq(PGconn *conn, PGresult *res) : conn_{conn}, res_{res} {}

  Iterator begin() const noexcept final {
    return std::make_unique<ItPq<Contacts, Contact>>(res_);
  }

  Iterator end() const noexcept final {
    return std::make_unique<ItPq<Contacts, Contact>>(res_, PQntuples(res_));
  }

private:
  PGconn *conn_;
  PGresult *res_;
};

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

} // namespace

std::unique_ptr<ContactRepository> ContactRepository::Make() {
  return std::make_unique<PqContactRepository>();
}
