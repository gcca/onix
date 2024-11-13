#include "../interaction_all.hpp"
#include "../interfaces.hpp"


crow::response onix::interaction::interfaces::TypeCreate(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  auto name = x["name"].s();

  domain::models::Type type{name.s_};

  auto type_repository = domain::models::TypeRepository::Make();

  auto status = type_repository->Store(type);
  if (!status.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{status.message().data()}}});
  }

  return crow::response(crow::status::CREATED);
}
