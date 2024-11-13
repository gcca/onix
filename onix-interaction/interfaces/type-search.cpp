#include "../domain/models/type.hpp"
#include "../interfaces.hpp"

using namespace onix::interaction::domain::models::type;

crow::response
onix::interaction::interfaces::TypeSearch(const crow::request &req,
                                          const std::string &name) {
  auto type_repository = TypeRepository::Make();
  auto result = type_repository->Search(name);

  if (!result.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors",
             crow::json::wvalue::list{result.status().message().data()}}});
  }

  const auto &types = result.value();
  crow::json::wvalue::list x;

  for (const auto &type : *types) {
    x.push_back({
        {"id", type.id},
        {"name", type.name.data()},
    });
  }

  return crow::response(crow::status::OK, crow::json::wvalue{x});
}
