################################################################################
# Copyright (c) 2020 Vladislav Trifochkin
#
# This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-rfc5870 CXX)

option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::rfc5870 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if (${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
