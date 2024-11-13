cmake_minimum_required(VERSION 3.31)
project(onix VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/abseil-cpp")
  message(FATAL_ERROR "Not exist abseil-cpp directory.")
endif()

add_subdirectory(abseil-cpp)

option(ONIX_DISABLE_TESTS "Disable tests" OFF)

find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

function(onix_add_executable name)
  add_executable(${name} ${ARGN})
  target_compile_options(${name} PRIVATE -DCROW_USE_BOOST -DCROW_DISABLE_STATIC_DIR -DCROW_JSON_NO_ERROR_CHECK)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/resources ${CMAKE_SOURCE_DIR})
endfunction()

add_subdirectory(onix-interaction)
