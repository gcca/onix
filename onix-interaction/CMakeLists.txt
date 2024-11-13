onix_add_executable(onix-interaction
  onix-interaction.cc
  interfaces/contact-create.cpp
  interfaces/contact-list.cpp
  interfaces/interaction-create.cpp
  interfaces/interaction-list.cpp
  interfaces/type-create.cpp
  interfaces/type-search.cpp
  domain/models/contact.cpp
  domain/models/interaction.cpp
  domain/models/type.cpp)
target_link_libraries(onix-interaction PRIVATE PostgreSQL::PostgreSQL absl::flags absl::flags_parse absl::status absl::statusor)

add_executable(onix-interaction-migration migration.cc)
target_link_libraries(onix-interaction-migration PRIVATE PostgreSQL::PostgreSQL Boost::program_options)
