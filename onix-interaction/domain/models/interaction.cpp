#include <cstdint>
#include <memory>
#include <onix-core/pq.hpp>

#include "../../infrastructure/persistence/pq.hpp"
#include "interaction.hpp"

using namespace onix::interaction::infrastructure::persistence::pq;
using namespace onix::interaction::domain::models::interaction;
using namespace onix::core;

namespace {

class InteractionsPq final : public Interactions {
public:
  explicit InteractionsPq(PGconn *conn, PGresult *res)
      : conn_{conn},
        res_{res} {}

  Iterator begin() const noexcept final {
    return std::make_unique<ItPq<Interactions, Interaction>>(res_);
  }

  Iterator end() const noexcept final {
    return std::make_unique<ItPq<Interactions, Interaction>>(res_,
                                                             PQntuples(res_));
  }

private:
  PGconn *conn_;
  PGresult *res_;
};

class PqInteractionRepository : public InteractionRepository, RepositoryPq {
public:
  explicit PqInteractionRepository() {}

  absl::Status Create(std::int32_t contact_id,
                      std::int32_t type_id,
                      const std::string_view &details) noexcept final {
    PGconn *conn = ConnectPG();

    NTOHL(contact_id);
    NTOHL(type_id);

    std::array<const char *, 3> values = {
        reinterpret_cast<const char *>(&contact_id),
        reinterpret_cast<const char *>(&type_id),
        details.data()};
    std::array<int, 3> lengths = {
        sizeof(std::int32_t), sizeof(std::int32_t), 0};
    std::array<int, 3> formats = {1, 1, 0};

    PGresult *res =
        ExecInsertfPQ(conn,
                      "INSERT INTO onix_interaction_interaction"
                      " (contact_id, type_id, details) VALUES ($1, $2, $3)",
                      3,
                      values.data(),
                      lengths.data(),
                      formats.data());

    return absl::OkStatus();
  }

  absl::StatusOr<std::unique_ptr<Interactions>> All() const noexcept final {
    PGconn *conn = ConnectPG();

    PGresult *res =
        ExecSelectPQ(conn,
                     "SELECT contact_id, type_id, details, created_at FROM "
                     "onix_interaction_interaction");

    return std::make_unique<InteractionsPq>(conn, res);
  }
};

} // namespace

std::unique_ptr<InteractionRepository> InteractionRepository::Make() {
  return std::make_unique<PqInteractionRepository>();
}
