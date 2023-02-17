cmake_minimum_required(VERSION 3.10)

# Set the project name (and version?)
project(advent-of-code-2021)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Changes the binary, static lib, and libraries default paths of the CMake build, respectively
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)

INCLUDE_DIRECTORIES ( include )

# Days challenges
add_subdirectory(day_01)
add_subdirectory(day_02)
add_subdirectory(day_03)
add_subdirectory(day_04)
add_subdirectory(day_05)
add_subdirectory(day_06)
add_subdirectory(day_07)
add_subdirectory(day_08)
add_subdirectory(day_09)
add_subdirectory(day_10)
add_subdirectory(day_11)
add_subdirectory(day_12)
add_subdirectory(day_13)
add_subdirectory(day_14)
add_subdirectory(day_15)
add_subdirectory(day_16)
add_subdirectory(day_17)
add_subdirectory(day_18)
add_subdirectory(day_19)
add_subdirectory(day_20)
add_subdirectory(day_21)
