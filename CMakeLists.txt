cmake_minimum_required(VERSION 3.31)
project(onix VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

option(ONIX_DISABLE_TESTS "Disable tests" OFF)

list(APPEND CMAKE_PREFIX_PATH "${ONIX_CROW_DIR}/lib/cmake/Crow")
list(APPEND CMAKE_PREFIX_PATH "${ONIX_ABSL_DIR}/lib/cmake/absl")

find_package(absl REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Crow)

function(onix_add_executable name)
  add_executable(${name} ${ARGN})
  target_compile_options(${name} PRIVATE -DCROW_USE_BOOST -DCROW_DISABLE_STATIC_DIR -DCROW_JSON_NO_ERROR_CHECK)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR})
  target_link_libraries(${name} PRIVATE Crow::Crow)
endfunction()

add_subdirectory(onix-interaction)
add_subdirectory(onix-lead)
