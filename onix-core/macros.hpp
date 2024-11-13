#pragma once

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
