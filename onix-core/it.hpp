#include <concepts>
#include <memory>

#define ONIX_ITERATOR(T)                                                       \
  class It {                                                                   \
    ONIX_PROTO(It)                                                             \
  public:                                                                      \
    virtual T operator*() const noexcept = 0;                                  \
    virtual It &operator++() noexcept = 0;                                     \
    virtual bool operator!=(const It &other) const noexcept = 0;               \
  };                                                                           \
  using Iterator = onix::core::Iterator<It, T>;

namespace onix::core {

template <class It, class T> class Iterator {
public:
  template <class I>
    requires std::derived_from<I, It>
  Iterator(std::unique_ptr<I> it) : it_{std::move(it)} {}

  T operator*() const noexcept { return **it_; }

  Iterator &operator++() noexcept {
    ++*it_;
    return *this;
  }

  bool operator!=(const Iterator &other) const noexcept {
    return *it_ != *other.it_;
  }

private:
  std::unique_ptr<It> it_;
};

} // namespace onix::core
