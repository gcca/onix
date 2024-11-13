#include <absl/status/statusor.h>

#include <onix-core/it.hpp>
#include <onix-core/macros.hpp>

namespace onix::interaction::domain::models::contact {

class Contact {
public:
  std::string_view name;
  std::string_view email;
  std::string_view phone;

  static Contact Raw(const char *name, const char *email, const char *phone) {
    return Contact{name, email, phone};
  }
};

class Contacts {
  ONIX_PROTO(Contacts)
public:
  ONIX_ITERATOR(Contact)

  [[nodiscard]] virtual Iterator begin() const noexcept = 0;
  [[nodiscard]] virtual Iterator end() const noexcept = 0;
};

class ContactRepository {
  ONIX_PROTO(ContactRepository)
public:
  [[nodiscard]] virtual absl::Status Store(const Contact &contact) noexcept = 0;

  [[nodiscard]] virtual absl::StatusOr<std::unique_ptr<Contacts>>
  All() const noexcept = 0;

  [[nodiscard]] static std::unique_ptr<ContactRepository> Make();
};

} // namespace onix::interaction::domain::models::contact
