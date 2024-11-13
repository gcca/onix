#include "../interaction_all.hpp"
#include "../interfaces.hpp"

crow::response onix::interaction::interfaces::ContactCreate(const crow::request &req) {
  auto x = crow::json::load(req.body);
  if (!x) { return crow::response(crow::status::BAD_REQUEST); }

  auto name = x["name"].s();
  auto email = x["email"].s();
  auto phone = x["phone"].s();

  domain::models::Contact contact{
      name.s_,
      email.s_,
      phone.s_,
  };

  auto contact_repository = domain::models::ContactRepository::Make();

  auto status = contact_repository->Store(contact);
  if (!status.ok()) {
    return crow::response(
        crow::status::INTERNAL_SERVER_ERROR,
        crow::json::wvalue{
            {"errors", crow::json::wvalue::list{status.message().data()}}});
  }

  return crow::response(crow::status::CREATED);
}
