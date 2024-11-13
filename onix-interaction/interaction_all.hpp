#include <absl/status/status.h>
#include <absl/status/statusor.h>

#include <onix-core/macros.hpp>

namespace onix::interaction::domain::models {

class Contact {
public:
  std::string_view name;
  std::string_view email;
  std::string_view phone;
};

class Contacts {
  ONIX_PROTO(Contacts)
public:
  class It {
    ONIX_PROTO(It)
  public:
    virtual Contact operator*() const noexcept = 0;
    virtual It &operator++() noexcept = 0;
    virtual bool operator!=(const It &other) const noexcept = 0;
  };

  class Iterator {
  public:
    template <class I>
      requires std::derived_from<I, It>
    Iterator(std::unique_ptr<I> it) : it_{std::move(it)} {}

    Contact operator*() const noexcept;
    Iterator &operator++() noexcept;
    bool operator!=(const Iterator &other) const noexcept;

  private:
    std::unique_ptr<It> it_;
  };

  [[nodiscard]] virtual Iterator begin() const noexcept = 0;
  [[nodiscard]] virtual Iterator end() const noexcept = 0;
};

class ContactRepository {
  ONIX_PROTO(ContactRepository)
public:
  [[nodiscard]] virtual absl::Status Store(const Contact &contact) noexcept = 0;

  [[nodiscard]] virtual absl::StatusOr<std::unique_ptr<Contacts>>
  All() const noexcept = 0;

  static std::unique_ptr<ContactRepository> Make();
};

class Interaction {
public:
  std::string_view name;
  std::string_view details;
  std::size_t type_id;
  std::size_t contact_id;
};

class Type {
public:
  std::string_view name;
};

class TypeRepository {
  ONIX_PROTO(TypeRepository)
public:
  [[nodiscard]] virtual absl::Status Store(const Type &type) noexcept = 0;

  [[nodiscard]] virtual absl::StatusOr<
      std::vector<std::pair<std::size_t, std::string_view>>>
  Search(const std::string_view &name) const noexcept = 0;

  static std::unique_ptr<TypeRepository> Make();
};

} // namespace onix::interaction::domain::models
