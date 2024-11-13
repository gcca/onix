#pragma once

#include <absl/strings/str_cat.h>
#include <libpq-fe.h>

#define ConnectPG()                                                            \
  Connect();                                                                   \
  if (PQstatus(conn) != CONNECTION_OK) {                                       \
    return absl::InternalError(absl::StrCat("Connection to database failed: ", \
                                            PQerrorMessage(conn)));            \
  }

#define ExecPQ(conn, q, status)                                                \
  PQexec(conn, (q));                                                           \
  if (!res) {                                                                  \
    PQfinish(conn);                                                            \
    return absl::InternalError("Query execution failed");                      \
  }                                                                            \
  if (PQresultStatus(res) != status) {                                         \
    const std::string s =                                                      \
        absl::StrCat("Query result failed: ", PQerrorMessage(conn));           \
    PQclear(res);                                                              \
    PQfinish(conn);                                                            \
    return absl::InternalError(s);                                             \
  }

#define ExecParamsPQ(conn, q, n, vals, lens, status)                           \
  PQexecParams(conn, (q), n, nullptr, vals, lens, nullptr, 0);                 \
  if (!res) {                                                                  \
    PQfinish(conn);                                                            \
    return absl::InternalError("Query execution failed");                      \
  }                                                                            \
  if (PQresultStatus(res) != status) {                                         \
    const std::string s =                                                      \
        absl::StrCat("Query result failed: ", PQerrorMessage(conn));           \
    PQclear(res);                                                              \
    PQfinish(conn);                                                            \
    return absl::InternalError(s);                                             \
  }

#define ExecParamsfPQ(conn, q, n, vals, lens, fmts, status)                    \
  PQexecParams(conn, (q), n, nullptr, vals, lens, fmts, 0);                    \
  if (!res) {                                                                  \
    PQfinish(conn);                                                            \
    return absl::InternalError("Query execution failed");                      \
  }                                                                            \
  if (PQresultStatus(res) != status) {                                         \
    const std::string s =                                                      \
        absl::StrCat("Query result failed: ", PQerrorMessage(conn));           \
    PQclear(res);                                                              \
    PQfinish(conn);                                                            \
    return absl::InternalError(s);                                             \
  }

#define ExecInsertPQ(conn, q, n, vals, lens)                                   \
  ExecParamsPQ(conn, q, n, vals, lens, PGRES_COMMAND_OK);                      \
  PQclear(res);                                                                \
  PQfinish(conn)

#define ExecInsertfPQ(conn, q, n, vals, lens, fmts)                            \
  ExecParamsfPQ(conn, q, n, vals, lens, fmts, PGRES_COMMAND_OK);               \
  PQclear(res);                                                                \
  PQfinish(conn)

#define ExecSelectPQ(conn, q) ExecPQ(conn, q, PGRES_TUPLES_OK)

namespace onix::core {

template <class Cs, class C> class ItPq final : public Cs::It {
  template <class> struct ItLen;
  template <class R, class... Args> struct ItLen<R (*)(Args...)> {
    static constexpr std::size_t value = sizeof...(Args);
  };

public:
  ItPq() : res_{nullptr}, idx_{0} {}
  ItPq(PGresult *res) : res_{res}, idx_{0} {}
  ItPq(PGresult *res, int idx) : res_{res}, idx_{idx} {}

  C operator*() const noexcept final {
    return DoC(std::make_index_sequence<ItLen<decltype(&C::Raw)>::value>());
  }

  ItPq &operator++() noexcept final {
    ++idx_;
    return *this;
  }

  bool operator!=(const Cs::It &other) const noexcept final {
    return idx_ != static_cast<const ItPq &>(other).idx_;
  }

private:
  PGresult *res_;
  int idx_;

  template <std::size_t... I>
  inline C DoC(std::index_sequence<I...>) const noexcept {
    return C::Raw(PQgetvalue(res_, idx_, I)...);
  }
};

} // namespace onix::core
