cmake_minimum_required(VERSION 3.15)

project(bytecode-vm)

add_executable(bytecode-vm src/main.cpp src/assembler.cpp src/run.cpp)

target_include_directories(bytecode-vm PRIVATE "${PROJECT_SOURCE_DIR}/include")
