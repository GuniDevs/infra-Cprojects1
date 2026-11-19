cmake_minimum_required(VERSION 3.16)
project(EIMS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_library(eims_core
    src/database.cpp
    src/inventory_service.cpp
    src/inventory_repository.cpp
)
target_include_directories(eims_core PUBLIC include)
target_link_libraries(eims_core PUBLIC SQLite::SQLite3)
target_compile_options(eims_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wconversion>
)

add_executable(eims src/main.cpp)
target_link_libraries(eims PRIVATE eims_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(eims_tests tests/inventory_service_tests.cpp)
    target_link_libraries(eims_tests PRIVATE eims_core)
    add_test(NAME eims_tests COMMAND eims_tests)
endif()
