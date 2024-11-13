onix_add_executable(onix-lead onix-lead.cc ws/shared.cpp
  ws/lead/create.cpp
  ws/lead/list.cpp
  ws/lead/create-seen.cpp
  ws/source/create.cpp
  ws/source/list.cpp)
target_link_libraries(onix-lead PRIVATE PostgreSQL::PostgreSQL)

add_executable(onix-lead-migration onix-lead-migration.cc)
target_link_libraries(onix-lead-migration PRIVATE PostgreSQL::PostgreSQL)
