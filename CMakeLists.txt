# Copyright 2020 Lawrence Livermore National Security, LLC and other CLIPPy
# Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: MIT

# Works with 3.11 and tested through 3.15 (not tested yet)
cmake_minimum_required(VERSION 3.14)
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
cmake_policy(SET CMP0135 NEW)
endif()
set(CMAKE_OSX_SYSROOT)
project(TestJL
    VERSION 0.01
    DESCRIPTION "Tests JSONLogic"
    LANGUAGES CXX)

include(FetchContent) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

endif()

#
# Boost
# find_package(Boost 1.83 REQUIRED COMPONENTS)


# Download and build Boost::json
set(BOOST_URL 
    "https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.gz" 
    CACHE STRING "URL to fetch Boost tarball")


set(BOOST_INCLUDE_LIBRARIES json)
set(BUILD_SHARED_LIBS ON)

FetchContent_Declare(
    Boost 
    URL ${BOOST_URL})
FetchContent_MakeAvailable(Boost)


# set(Boost_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/boost-install) # needed for jsonlogic
set(BUILD_TESTING OFF) # Disable Faker tests
FetchContent_Declare(faker
    GIT_REPOSITORY https://github.com/cieslarmichal/faker-cxx.git
    GIT_TAG main
)

FetchContent_MakeAvailable(faker)

FetchContent_Declare(jsonlogic
    GIT_REPOSITORY https://github.com/LLNL/jsonlogic.git
    GIT_TAG master
    SOURCE_SUBDIR cpp
)
FetchContent_MakeAvailable(jsonlogic)


message(STATUS "jsonlogic source dir: ${jsonlogic_SOURCE_DIR}")

    
### Require out-of-source builds
# file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
# if(EXISTS "${LOC_PATH}")
#     message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
# endif()

include_directories("${PROJECT_SOURCE_DIR}/include")
message(STATUS "adding executable")
add_executable(testjl src/testjl-variant.cpp)
add_executable(testjl-2ints src/testjl-2ints.cpp)
add_executable(testjl-strset src/testjl-strset.cpp)
add_executable(benchjl-2ints src/benchjl-2ints.cpp)
add_executable(benchjl-generic src/testbench-generic.cpp)


# message(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
target_link_libraries(testjl PRIVATE jsonlogic Boost::json faker-cxx)
target_include_directories(testjl PRIVATE ${jsonlogic_SOURCE_DIR}/cpp/include)


target_link_libraries(testjl-2ints PRIVATE jsonlogic faker-cxx)
target_include_directories(
    testjl-2ints PRIVATE 
    ${jsonlogic_SOURCE_DIR}/cpp/include
    ${faker_SOURCE_DIR}/include
    )

target_compile_options(testjl-strset PRIVATE -g)

target_link_libraries(testjl-strset PRIVATE jsonlogic Boost::json faker-cxx)
target_include_directories(
    testjl-strset PRIVATE 
    ${jsonlogic_SOURCE_DIR}/cpp/include
    ${faker_SOURCE_DIR}/include
    )

target_compile_options(testjl-strset PRIVATE -g)

target_link_libraries(benchjl-2ints PRIVATE jsonlogic faker-cxx)
target_include_directories(
    benchjl-2ints PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${jsonlogic_SOURCE_DIR}/cpp/include
    ${faker_SOURCE_DIR}/include
)

target_compile_options(benchjl-generic PRIVATE -O3)
target_link_libraries(benchjl-generic PRIVATE Boost::json jsonlogic faker-cxx)
target_include_directories(
    benchjl-generic PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${jsonlogic_SOURCE_DIR}/cpp/include
    ${faker_SOURCE_DIR}/include
)