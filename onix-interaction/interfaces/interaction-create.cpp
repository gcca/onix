#include "../domain/models/interaction.hpp"
#include "../interfaces.hpp"

using namespace onix::interaction::domain::models::interaction;

crow::response
onix::interaction::interfaces::InteractionCreate(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  auto contact_id = x["contact_id"].i();
  auto type_id = x["type_id"].i();
  auto details = x["details"].s();

  auto interaction_repository = InteractionRepository::Make();

  auto status = interaction_repository->Create(contact_id, type_id, details.s_);
  if (!status.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{status.message().data()}}});
  }

  return crow::response(crow::status::CREATED);
}
