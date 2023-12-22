cmake_minimum_required(VERSION 3.19)
project(aas_core3)

set(CMAKE_CXX_STANDARD 11)

# NOTE (mristin, 2023-12-22):
# This is a subtle hack to make it easier for the developers to work on Windows where
# it is often really cumbersome to install the libraries system-wide.
set(DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS ${CMAKE_SOURCE_DIR}/dependencies-for-local-development-on-windows)
IF (WIN32 AND EXISTS "${DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS}")
    set(EXPAT_LIBRARY ${DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS}/expat-2.5.0/Bin/libexpat.lib)
    set(EXPAT_INCLUDE_DIR ${DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS}/expat-2.5.0/Source/lib)

    include_directories(${DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS}/nlohmann-json)
    include_directories(${DEPS_FOR_LOCAL_DEVELOPMENT_ON_WINDOWS}/tartanllama)
ELSE()
    find_package(nlohmann_json 3 REQUIRED)
    find_package(tl-optional 1 REQUIRED)
ENDIF()

find_package(EXPAT REQUIRED)

FILE(GLOB AAS_CORE_SOURCES src/*.cpp src/*.hpp)

add_library(aas_core3 ${AAS_CORE_SOURCES})
target_link_libraries(aas_core3 PUBLIC EXPAT::EXPAT)

# TODO: remove once testing in place
#
#add_executable(regex_playground regex_playground.cpp)
#
#add_executable(json_playground json.cpp)
#
#add_library(xmlization xmlization.cpp xmlization.hpp)
#target_link_libraries(xmlization PUBLIC EXPAT::EXPAT)
#
#include_directories(${CMAKE_SOURCE_DIR}/dependencies/tartanllama)
#add_library(aas_common common.cpp common.hpp)
#add_executable(aas_common_playground common_playground.cpp)
#target_link_libraries(aas_common_playground aas_common)
#
#include_directories(${CMAKE_SOURCE_DIR}/dependencies/nlohmann-json)
#include_directories(${CMAKE_SOURCE_DIR}/dependencies/catch-2.13.10)
#
#
#include(CTest)
#enable_testing()
#add_executable(xmlization_reader_test xmlization_reader_test.cpp)
#target_link_libraries(xmlization_reader_test xmlization)
#add_test(NAME xmlization_reader_test COMMAND xmlization_reader_test)
