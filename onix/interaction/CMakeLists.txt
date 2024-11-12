onix_add_executable(onix-interaction onix-interaction.cc interaction_all.cpp)
target_link_libraries(onix-interaction PRIVATE PostgreSQL::PostgreSQL absl::flags absl::flags_parse absl::status absl::statusor)
