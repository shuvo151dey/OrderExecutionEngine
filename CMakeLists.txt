cmake_minimum_required(VERSION 3.15)
project(MyCppProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Conan toolchain (Conan 2.x)
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Add executable
add_executable(MyCppProject src/main.cpp)

# Link Conan libraries to the executable
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
# ...
target_link_libraries(MyCppProject nlohmann_json::nlohmann_json)