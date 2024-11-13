#include <crow/http_request.h>
#include <crow/http_response.h>

namespace onix::lead::ws {

namespace source {
crow::response Create(const crow::request &req);
crow::response List(const crow::request &req);
} // namespace source

namespace lead {
crow::response Create(const crow::request &req);
crow::response List(const crow::request &req);
crow::response CreateSeen(const crow::request &req, int lead_id);
} // namespace lead

} // namespace onix::lead::ws
