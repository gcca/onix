onix_add_executable(onix-interaction
  onix-interaction.cc interaction_all.cpp
  interfaces/contact-list.cpp
  interfaces/contact-create.cpp
  interfaces/type-create.cpp
  interfaces/type-search.cpp)
target_link_libraries(onix-interaction PRIVATE PostgreSQL::PostgreSQL absl::flags absl::flags_parse absl::status absl::statusor)

add_executable(onix-interaction-migration migration.cc)
target_link_libraries(onix-interaction-migration PRIVATE PostgreSQL::PostgreSQL Boost::program_options)
