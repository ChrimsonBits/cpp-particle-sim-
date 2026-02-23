cmake_minimum_required(VERSION 3.20)
project(particle_sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(particle_sim
  src/main.cpp
  src/Sim.cpp
)

target_include_directories(particle_sim PRIVATE include)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

target_link_libraries(particle_sim PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::System
)

# Helpful warnings for "professional" builds
if (MSVC)
  target_compile_options(particle_sim PRIVATE /W4 /permissive-)
else()
  target_compile_options(particle_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()
