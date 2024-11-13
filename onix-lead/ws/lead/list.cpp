#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <libpq-fe.h>

#include "../shared.hpp"

namespace onix::lead::ws::lead {

crow::response List(const crow::request &req) {
  PGconn *conn = onix::lead::ws::shared::ConnectPG();

  if (PQstatus(conn) != CONNECTION_OK) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{PQerrorMessage(conn)}}});
  }

  const char *command = "SELECT L.id, S.name, L.name,"
                        " (SELECT string_agg(details, '\001')"
                        "    FROM onix_lead_seen WHERE lead_id = L.id)"
                        " FROM onix_lead_lead L"
                        " JOIN onix_lead_source S ON L.source_id = S.id";
  PGresult *res = PQexec(conn, command);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    const std::string s = PQerrorMessage(conn);
    PQclear(res);
    PQfinish(conn);
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{{"errors", crow::json::wvalue::list{s}}});
  }

  crow::json::wvalue::list leads;
  for (int i = 0; i < PQntuples(res); i++) {
    crow::json::wvalue::list seens;

    char *s = PQgetvalue(res, i, 3);
    if (*s) {
      char *r = s;
      for (; *s; s++)
        if (*s == 1) {
          *s = '\0';
          seens.emplace_back(r);
          r = s + 1;
        }
      seens.emplace_back(r);
    }

    leads.emplace_back(
        crow::json::wvalue{{"id", std::stoll(PQgetvalue(res, i, 0))},
                           {"source", PQgetvalue(res, i, 1)},
                           {"name", PQgetvalue(res, i, 2)},
                           {"seens", std::move(seens)}});
  }

  PQclear(res);
  PQfinish(conn);

  return crow::response{crow::json::wvalue{leads}};
}

} // namespace onix::lead::ws::lead
