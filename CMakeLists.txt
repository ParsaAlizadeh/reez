cmake_minimum_required(VERSION 3.27)
project(reez VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)

add_executable(reez reez.c)
target_include_directories(reez PUBLIC "${PROJECT_BINARY_DIR}")
