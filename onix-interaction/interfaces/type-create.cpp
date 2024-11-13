#include "../domain/models/type.hpp"
#include "../interfaces.hpp"

using namespace onix::interaction::domain::models::type;

crow::response
onix::interaction::interfaces::TypeCreate(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  auto type_repository = TypeRepository::Make();

  auto status = type_repository->Add(x["name"].s().s_);
  if (!status.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{status.message().data()}}});
  }

  return crow::response(crow::status::CREATED);
}
