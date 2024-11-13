#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <crow_all.h>

#include "interfaces.hpp"

using namespace onix::interaction::interfaces;

ABSL_FLAG(std::uint16_t, port, 80, "Port to listen on");
ABSL_FLAG(bool, nothread, false, "Disable threading");
ABSL_FLAG(bool, dbg, false, "Enable debugging mode");

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage("Contact interaction service");
  absl::ParseCommandLine(argc, argv);

  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "🙂"; });

  CROW_ROUTE(app, "/v1/contact").methods("GET"_method)(ContactList);
  CROW_ROUTE(app, "/v1/contact").methods("POST"_method)(ContactCreate);

  CROW_ROUTE(app, "/v1/type").methods("POST"_method)(TypeCreate);
  CROW_ROUTE(app, "/v1/type/search/<string>").methods("GET"_method)(TypeSearch);

  CROW_ROUTE(app, "/v1/interaction").methods("POST"_method)(InteractionCreate);
  CROW_ROUTE(app, "/v1/interaction").methods("GET"_method)(InteractionList);

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
