cmake_minimum_required(VERSION 3.14)

project(helium_3_mining_sim VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# Core system
# Build core as a static lib so the main executable and test executables can
# link against the same compiled core
add_library(helium_3_mining_sim_core STATIC
    src/mining_truck.cpp
    src/unload_station.cpp
)
target_include_directories(helium_3_mining_sim_core PUBLIC inc)

# Main executable
add_executable(helium_3_mining_sim src/main.cpp)
target_link_libraries(helium_3_mining_sim PRIVATE helium_3_mining_sim_core)

# Test executable
add_executable(helium_3_mining_sim_tests tests/tests.cpp)
target_link_libraries(helium_3_mining_sim_tests PRIVATE helium_3_mining_sim_core)