#include <absl/status/statusor.h>

#include <memory>
#include <onix-core/it.hpp>
#include <onix-core/macros.hpp>

namespace onix::interaction::domain::models::interaction {

class Interaction {
public:
  std::int32_t contact_id;
  std::int32_t type_id;
  std::string_view details;
  std::string_view created_at;

  static Interaction Raw(const char *contact_id,
                         const char *type_id,
                         const char *details,
                         const char *created_at) {
    return Interaction{
        .contact_id = std::atoi(contact_id),
        .type_id = std::atoi(type_id),
        .details = details,
        .created_at = created_at,
    };
  }
};

class Interactions {
  ONIX_PROTO(Interactions)
public:
  ONIX_ITERATOR(Interaction)

  [[nodiscard]] virtual Iterator begin() const noexcept = 0;
  [[nodiscard]] virtual Iterator end() const noexcept = 0;
};

class InteractionRepository {
  ONIX_PROTO(InteractionRepository)
public:
  [[nodiscard]] virtual absl::Status
  Create(std::int32_t contact_id,
         std::int32_t type_id,
         const std::string_view &details) noexcept = 0;

  [[nodiscard]] virtual absl::StatusOr<std::unique_ptr<Interactions>>
  All() const noexcept = 0;

  [[nodiscard]] static std::unique_ptr<InteractionRepository> Make();
};

} // namespace onix::interaction::domain::models::interaction
