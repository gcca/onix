#include "../domain/models/interaction.hpp"
#include "../interfaces.hpp"

using namespace onix::interaction::domain::models::interaction;

crow::response onix::interaction::interfaces::InteractionList() {
  auto interaction_repository = InteractionRepository::Make();
  auto result = interaction_repository->All();

  if (!result.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors",
             crow::json::wvalue::list{result.status().message().data()}}});
  }

  const auto &interactions = *result.value();
  crow::json::wvalue::list x;

  for (const auto &interaction : interactions) {
    x.push_back({
        {"contact_id", interaction.contact_id},
        {"type_id", interaction.type_id},
        {"details", interaction.details.data()},
        {"created_at", interaction.created_at.data()},
    });
  }

  return crow::response(crow::status::OK, crow::json::wvalue{x});
}
