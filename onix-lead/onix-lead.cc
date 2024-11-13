#include <stdexcept>

#include <crow/app.h>

#include <getopt.h>

#include "ws.hpp"

using namespace onix::lead;

int main(int argc, char *argv[]) {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "🙂"; });

  CROW_ROUTE(app, "/lead/v1/source").methods("POST"_method)(ws::source::Create);
  CROW_ROUTE(app, "/lead/v1/source").methods("GET"_method)(ws::source::List);

  CROW_ROUTE(app, "/lead/v1/lead").methods("POST"_method)(ws::lead::Create);
  CROW_ROUTE(app, "/lead/v1/lead").methods("GET"_method)(ws::lead::List);
  CROW_ROUTE(app, "/lead/v1/lead/<int>/seen")
      .methods("POST"_method)(ws::lead::CreateSeen);

  std::int16_t port = 8000;
  bool threading = false;

  const option options[] = {{"port", required_argument, nullptr, 'p'},
                            {"threading", no_argument, nullptr, 't'}};

  while (const auto opt = getopt_long(argc, argv, "p:t", options, nullptr)) {
    if (opt == -1) break;
    switch (opt) {
    case 'p':
      try {
        port = std::stoi(optarg);
        break;
      } catch (std::invalid_argument &e) {
        std::cerr << "Invalid port number: " << optarg << std::endl;
        return EXIT_FAILURE;
      }
    case 't': threading = true; break;
    case '?': return EXIT_FAILURE;
    }
  }

  if (threading) { app.multithreaded(); }

  app.port(port).run();

  return EXIT_SUCCESS;
}
