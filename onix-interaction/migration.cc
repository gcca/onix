#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>
#include <libpq-fe.h>

static const char *migrate_q = R"(DO $$
BEGIN
CREATE TABLE IF NOT EXISTS onix_migrations (
  id SERIAL PRIMARY KEY,
  app VARCHAR(128) NOT NULL,
  count INT NOT NULL DEFAULT 0,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

IF (SELECT COUNT(*) FROM onix_migrations WHERE app = 'onix-interaction') = 0 THEN
  INSERT INTO onix_migrations (app, count) VALUES ('onix-interaction', 0);
END IF;

CREATE TABLE IF NOT EXISTS onix_interaction_contact (
  id SERIAL PRIMARY KEY,
  name VARCHAR(100),
  email VARCHAR(100),
  phone VARCHAR(20)
);

CREATE TABLE IF NOT EXISTS onix_interaction_type (
  id SERIAL PRIMARY KEY,
  name VARCHAR(100) UNIQUE
);

CREATE TABLE IF NOT EXISTS onix_interaction_interaction (
  id SERIAL PRIMARY KEY,
  contact_id INT REFERENCES onix_interaction_contact(id),
  type_id INT REFERENCES onix_interaction_type(id),
  details TEXT,
  created_at TIMESTAMP DEFAULT NOW()
);

END $$;)";

static const char *rollback_q = R"(DO $$
BEGIN
  DROP TABLE IF EXISTS onix_interaction_interaction;
  DROP TABLE IF EXISTS onix_interaction_type;
  DROP TABLE IF EXISTS onix_interaction_contact;
  DROP TABLE IF EXISTS onix_migrations;
END $$;)";

static const char *host = "localhost", *db = "onix", *user = "", *pass = "",
                  *port = "5432";

int ExecAction(const std::string &q) {
  std::ostringstream oss;
  oss << "host=" << host << " dbname=" << db << " port=" << port;
  PGconn *conn = PQconnectdb(oss.str().c_str());

  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << "Connection to database failed: " << PQerrorMessage(conn)
              << std::endl;
    PQfinish(conn);
    return EXIT_FAILURE;
  }

  PGresult *res = PQexec(conn, q.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    std::cerr << "Error: " << PQresultErrorMessage(res) << std::endl;

  PQclear(res);
  PQfinish(conn);

  return EXIT_SUCCESS;
}

int main(int argc, const char *argv[]) {
  boost::program_options::options_description desc(
      "ONIX Interaction migration tool");
  desc.add_options()("help,h", "produce help message")(
      "database,d",
      boost::program_options::value<std::string>(),
      "database name")(
      "user,u", boost::program_options::value<std::string>(), "database user")(
      "password,p",
      boost::program_options::value<std::string>(),
      "database password")(
      "host,h", boost::program_options::value<std::string>(), "database host")(
      "port,P", boost::program_options::value<std::string>(), "database port")(
      "show", "show script")("action", "action: migrate or rollback");

  boost::program_options::positional_options_description p;
  p.add("action", 2);

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv)
          .options(desc)
          .positional(p)
          .run(),
      vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  if (!vm.count("action")) {
    std::cerr << "Action is required" << std::endl;
    return EXIT_FAILURE;
  }

  const std::string action = vm["action"].as<std::string>();

  if (action == "migrate") {
    if (vm.count("show")) {
      std::cout << migrate_q << std::endl;
      return EXIT_SUCCESS;
    } else {
      std::cout << "Migrating..." << std::endl;
      return ExecAction(migrate_q);
    }
  } else if (action == "rollback") {
    if (vm.count("show")) {
      std::cout << rollback_q << std::endl;
      return EXIT_SUCCESS;
    } else {
      std::cout << "Rolling back..." << std::endl;
      return ExecAction(rollback_q);
    }
  } else {
    std::cerr << "Invalid action" << std::endl;
  }

  return EXIT_FAILURE;
}
