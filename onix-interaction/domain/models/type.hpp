#include <absl/status/statusor.h>

#include <memory>
#include <onix-core/it.hpp>
#include <onix-core/macros.hpp>

namespace onix::interaction::domain::models::type {

class Type {
public:
  std::string_view name;
  std::int32_t score;
};

class BoundType {
public:
  int id;
  std::string_view name;

  static BoundType Raw(const char *id, const char *name) {
    return BoundType{std::stoi(id), name};
  }
};

class BoundTypes {
  ONIX_PROTO(BoundTypes)
public:
  ONIX_ITERATOR(BoundType)

  [[nodiscard]] virtual Iterator begin() const noexcept = 0;
  [[nodiscard]] virtual Iterator end() const noexcept = 0;
};

class TypeRepository {
  ONIX_PROTO(TypeRepository)
public:
  [[nodiscard]] virtual absl::Status
  Add(const std::string_view &name) noexcept = 0;

  [[nodiscard]] virtual absl::StatusOr<std::unique_ptr<BoundTypes>>
  Search(const std::string_view &name) const noexcept = 0;

  [[nodiscard]] static std::unique_ptr<TypeRepository> Make();
};

} // namespace onix::interaction::domain::models::type
