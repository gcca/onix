#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <libpq-fe.h>

#include "../shared.hpp"

using namespace onix::lead::ws::shared;

namespace onix::lead::ws::source {

crow::response Create(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  PGconn *conn = ConnectPG();

  if (PQstatus(conn) != CONNECTION_OK) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{PQerrorMessage(conn)}}});
  }

  const char *command = "INSERT INTO onix_lead_source (name) VALUES ($1)";
  const char *values[] = {x["name"].s().s_};

  PGresult *res =
      PQexecParams(conn, command, 1, nullptr, values, nullptr, nullptr, 0);

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

} // namespace onix::lead::ws::source
