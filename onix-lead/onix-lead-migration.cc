#include <iomanip>
#include <iostream>

#include <libpq-fe.h>
#include <sstream>

static const char *migrate_q = R"(DO $$
BEGIN
CREATE TABLE IF NOT EXISTS onix_migrations (
  id SERIAL PRIMARY KEY,
  app VARCHAR(128) NOT NULL,
  count INT NOT NULL DEFAULT 0,
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

IF (SELECT COUNT(*) FROM onix_migrations WHERE app = 'onix-lead') = 0 THEN
  INSERT INTO onix_migrations (app, count) VALUES ('onix-lead', 0);
END IF;

CREATE TABLE IF NOT EXISTS onix_lead_source (
  id SERIAL PRIMARY KEY,
  name VARCHAR(100) UNIQUE,
  created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS onix_lead_lead (
  id SERIAL PRIMARY KEY,
  name VARCHAR(100),
  source_id INT REFERENCES onix_lead_source(id),
  created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS onix_lead_seen (
  id SERIAL PRIMARY KEY,
  details TEXT,
  lead_id INT REFERENCES onix_lead_lead(id),
  created_at TIMESTAMP DEFAULT NOW()
);

INSERT INTO onix_lead_source (name) VALUES ('web'), ('email'), ('phone');

END $$;)";

static const char *rollback_q = R"(DO $$
BEGIN
  DROP TABLE IF EXISTS onix_lead_seen CASCADE;
  DROP TABLE IF EXISTS onix_lead_lead CASCADE;
  DROP TABLE IF EXISTS onix_lead_source;
END $$;)";

static const char *host = "localhost", *db = "onix", *user = "", *pass = "",
                  *port = "5432";

static int ExecQ(const std::string &q) {
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

void Usage() {
  std::cerr << "Usage: onix-lead-migration "
               "<action>\n\nactions:\n\t"
            << std::left << std::setw(12) << "migrate"
            << "Create db schema\n\t" << std::setw(12) << "rollback"
            << "Remove db schema\n\t" << std::setw(12) << "help"
            << "Show this message" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Usage();
    return EXIT_FAILURE;
  }

  const std::string action = argv[1];

  if (action == "migrate") {
    std::cout << "Migrating..." << std::endl;
    ExecQ(migrate_q);
    return EXIT_SUCCESS;
  } else if (action == "rollback") {
    std::cout << "Rolling back..." << std::endl;
    ExecQ(rollback_q);
    return EXIT_SUCCESS;
  } else if (action == "show") {
    std::cout << "\033[32mMigrate query:\033[0m\n" << migrate_q << std::endl;
    std::cout << "\n\033[32mRollback query:\033[0m\n"
              << rollback_q << std::endl;
    return EXIT_SUCCESS;
  } else if (action == "help") {
    Usage();
    return EXIT_SUCCESS;
  } else {
    Usage();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
