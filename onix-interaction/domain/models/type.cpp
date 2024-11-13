#include <memory>
#include <onix-core/pq.hpp>

#include "../../infrastructure/persistence/pq.hpp"
#include "type.hpp"

using namespace onix::interaction::infrastructure::persistence::pq;
using namespace onix::interaction::domain::models::type;
using namespace onix::core;

namespace {

class BoundTypesPq final : public BoundTypes {
public:
  explicit BoundTypesPq(PGconn *conn, PGresult *res) : conn_{conn}, res_{res} {}

  Iterator begin() const noexcept final {
    return std::make_unique<ItPq<BoundTypes, BoundType>>(res_);
  }

  Iterator end() const noexcept final {
    return std::make_unique<ItPq<BoundTypes, BoundType>>(res_, PQntuples(res_));
  }

private:
  PGconn *conn_;
  PGresult *res_;
};

class PqTypeRepository : public TypeRepository, RepositoryPq {
public:
  explicit PqTypeRepository() {}

  absl::Status Add(const std::string_view &name) noexcept final {
    PGconn *conn = ConnectPG();

    std::array<const char *, 1> values = {name.data()};
    std::array<int, 1> lengths = {static_cast<int>(name.size())};

    PGresult *res =
        ExecInsertPQ(conn,
                     "INSERT INTO onix_interaction_type (name) VALUES ($1)",
                     1,
                     values.data(),
                     lengths.data());

    return absl::OkStatus();
  }

  absl::StatusOr<std::unique_ptr<BoundTypes>>
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

    return std::make_unique<BoundTypesPq>(conn, res);
  }
};

} // namespace

std::unique_ptr<TypeRepository> TypeRepository::Make() {
  return std::make_unique<PqTypeRepository>();
}
