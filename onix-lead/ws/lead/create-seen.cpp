#include <crow/http_request.h>
#include <crow/http_response.h>
#include <sstream>

#include <crow/json.h>
#include <libpq-fe.h>

#include "../shared.hpp"

using namespace onix::lead::ws::shared;

namespace onix::lead::ws::lead {

crow::response CreateSeen(const crow::request &req, int lead_id) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  PGconn *conn = ConnectPG();

  if (PQstatus(conn) != CONNECTION_OK) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{PQerrorMessage(conn)}}});
  }

  // check
  std::ostringstream oss;
  oss << "SELECT EXISTS (SELECT 1 FROM onix_lead_lead WHERE id = " << lead_id
      << ")";

  PGresult *res = PQexec(conn, oss.str().c_str());

  if (PQresultStatus(res) != PGRES_TUPLES_OK or !PQntuples(res)) {
    PQclear(res);
    PQfinish(conn);
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{PQerrorMessage(conn)}}});
  }

  if (PQgetvalue(res, 0, 0)[0] != 't') {
    PQclear(res);
    PQfinish(conn);
    return crow::response(
        crow::status::NOT_FOUND,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{"Lead not found"}}});
  }

  PQclear(res);

  // create
  const char *command =
      "INSERT INTO onix_lead_seen (lead_id, details) VALUES ($1, $2)";
  const char *values[] = {reinterpret_cast<const char *>(&lead_id),
                          x["details"].s().s_};
  const int lengths[] = {sizeof(lead_id), 0};
  const int formats[] = {1, 0};

  HTONL(lead_id);
  res = PQexecParams(conn, command, 2, nullptr, values, lengths, formats, 0);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    const std::string s = PQerrorMessage(conn);
    PQclear(res);
    PQfinish(conn);
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{{"errors", crow::json::wvalue::list{s}}});
  }

  PQclear(res);
  PQfinish(conn);

  return crow::response(crow::status::CREATED);
}

} // namespace onix::lead::ws::lead
