#include <crow_all.h>

namespace onix::interaction::interfaces {

crow::response ContactList();
crow::response ContactCreate(const crow::request &req);

crow::response TypeCreate(const crow::request &req);
crow::response TypeSearch(const crow::request &req, const std::string &name);

crow::response InteractionCreate(const crow::request &req);
crow::response InteractionList();

} // namespace onix::interaction::interfaces
