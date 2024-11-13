#include "../interaction_all.hpp"
#include "../interfaces.hpp"

crow::response onix::interaction::interfaces::ContactList() {
  auto contact_repository = domain::models::ContactRepository::Make();
  auto result = contact_repository->All();

  if (!result.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors",
             crow::json::wvalue::list{result.status().message().data()}}});
  }

  const auto &contacts = *result.value();
  crow::json::wvalue::list x;

  for (const auto &contact : contacts) {
    x.push_back({
        {"name", contact.name.data()},
        {"email", contact.email.data()},
        {"phone", contact.phone.data()},
    });
  }

  return crow::response(crow::status::OK, crow::json::wvalue{x});
}
