#include "../interaction_all.hpp"
#include "../interfaces.hpp"

crow::response
onix::interaction::interfaces::TypeSearch(const crow::request &req,
                                          const std::string &name) {
  auto type_repository = domain::models::TypeRepository::Make();
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

  for (const auto &p : types) {
    x.push_back({
        {"id", static_cast<unsigned int>(p.first)},
        {"name", p.second.data()},
    });
  }

  return crow::response(crow::status::OK, crow::json::wvalue{x});
}
