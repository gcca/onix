#include <cstdint>
#include <iostream>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <crow_all.h>

#include "interaction_all.hpp"

ABSL_FLAG(std::uint16_t, port, 80, "Port to listen on");
ABSL_FLAG(bool, nothread, false, "Disable threading");
ABSL_FLAG(bool, dbg, false, "Enable debugging mode");

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage("Contact interaction service");
  absl::ParseCommandLine(argc, argv);

  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "🙂"; });

  CROW_ROUTE(app, "/contact").methods("GET"_method)([]() {
    auto contact_repository = onix::domain::models::ContactRepository::Make();
    auto status = contact_repository->All();

    if (!status.ok()) {
      return crow::response(
          crow::status::INTERNAL_SERVER_ERROR,
          crow::json::wvalue{
              {"errors", crow::json::wvalue::list{"Internal server error"}}});
    }

    auto &contacts = status.value();

    crow::json::wvalue::list x;
    for (const auto &contact : contacts) {
      x.push_back({
          {"name", contact.name},
          {"email", contact.email},
          {"phone", contact.phone},
      });
    }

    return crow::response(crow::status::OK, crow::json::wvalue{x});
  });

  CROW_ROUTE(app, "/contact")
      .methods("POST"_method)([](const crow::request &req) {
        auto x = crow::json::load(req.body);
        if (!x) {
          return crow::response(crow::status::BAD_REQUEST);
        }

        auto name = x["name"].s();
        auto email = x["email"].s();
        auto phone = x["phone"].s();

        onix::domain::models::Contact contact{
            name.s_,
            email.s_,
            phone.s_,
        };

        std::cout << "Store contact:\n"
                  << "    name: " << name << '\n'
                  << "    email: " << email << '\n'
                  << "    phone: " << phone << std::endl;

        auto contact_repository =
            onix::domain::models::ContactRepository::Make();

        auto status = contact_repository->Store(contact);
        if (!status.ok()) {
          return crow::response(
              crow::status::INTERNAL_SERVER_ERROR,
              crow::json::wvalue{{"errors", crow::json::wvalue::list{
                                                "Internal server error"}}});
        }

        return crow::response(crow::status::CREATED);
      });

  std::uint16_t port = absl::GetFlag(FLAGS_port);
  bool nothread = absl::GetFlag(FLAGS_nothread);

  std::cout << "ONIX-Interaction\n\nArguments:\n"
            << "    port: " << port << '\n'
            << "    nothread: " << (nothread ? "true" : "false") << '\n'
            << "    dbg: " << (absl::GetFlag(FLAGS_dbg) ? "true" : "false")
            << '\n'
            << std::endl;

  if (absl::GetFlag(FLAGS_dbg)) {
    app.loglevel(crow::LogLevel::DEBUG);
    std::cout << "Debugging mode enabled\n";
  } else {
    app.loglevel(crow::LogLevel::INFO);
    std::cout << "Debugging mode disabled\n";
  }

  app.port(port);

  if (!nothread) {
    std::cout << "Multithreaded mode enabled\n";
    app.multithreaded();
  } else {
    std::cout << "Single-threaded mode enabled\n";
  }

  std::cout << std::endl;

  app.run();

  return EXIT_SUCCESS;
}
