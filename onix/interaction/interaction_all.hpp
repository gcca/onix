#include <memory>
#include <string>
#include <vector>

#include <absl/status/status.h>
#include <absl/status/statusor.h>

#define ONIX_PROTO(T)                                                          \
  T(const T &) = delete;                                                       \
  T(T &&) = delete;                                                            \
  T &operator=(const T &) = delete;                                            \
  T &operator=(T &&) = delete;                                                 \
                                                                               \
public:                                                                        \
  virtual ~T() = default;                                                      \
                                                                               \
protected:                                                                     \
  T() = default;

namespace onix::domain::models {

class Contact {
public:
  std::string name;
  std::string email;
  std::string phone;
};

class ContactRepository {
  ONIX_PROTO(ContactRepository)

public:
  [[nodiscard]] virtual absl::Status Store(const Contact &contact) noexcept = 0;
  [[nodiscard]] virtual absl::StatusOr<std::vector<Contact>>
  All() const noexcept = 0;

  static std::unique_ptr<ContactRepository> Make();
};

} // namespace onix::domain::models
