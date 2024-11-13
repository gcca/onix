#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <libpq-fe.h>

#include "../shared.hpp"

namespace onix::lead::ws::source {

crow::response List(const crow::request &req) {
  PGconn *conn = onix::lead::ws::shared::ConnectPG();

  if (PQstatus(conn) != CONNECTION_OK) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{PQerrorMessage(conn)}}});
  }

  const char *command = "SELECT id, name FROM onix_lead_source";
  PGresult *res = PQexec(conn, command);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    const std::string s = PQerrorMessage(conn);
    PQclear(res);
    PQfinish(conn);
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{{"errors", crow::json::wvalue::list{s}}});
  }

  crow::json::wvalue::list j;
  for (int i = 0; i < PQntuples(res); i++) {
    crow::json::wvalue source;
    source["id"] = std::stoi(PQgetvalue(res, i, 0));
    source["name"] = PQgetvalue(res, i, 1);
    j.emplace_back(source);
  }

  PQclear(res);
  PQfinish(conn);

  return crow::response{crow::json::wvalue{j}};
}

} // namespace onix::lead::ws::source
