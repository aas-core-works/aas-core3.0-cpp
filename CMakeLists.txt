cmake_minimum_required(VERSION 3.19)
project(aas_core3)

set(AAS_CORE3_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(AAS_CORE3_VERSION_MINOR 0 CACHE STRING "minor version" FORCE)
set(AAS_CORE3_VERSION_PATCH 1 CACHE STRING "patch version" FORCE)
set(AAS_CORE3_VERSION_SUFFIX alpha.1 CACHE STRING "patch version" FORCE)

if (NOT "${AAS_CORE3_VERSION_SUFFIX}" STREQUAL "")
    set(AAS_CORE3_VERSION
            ${AAS_CORE3_VERSION_MAJOR}.${AAS_CORE3_VERSION_MINOR}.${AAS_CORE3_VERSION_PATCH}-${AAS_CORE3_VERSION_SUFFIX}
            CACHE STRING "version" FORCE
            )
else ()
    set(AAS_CORE3_VERSION
            ${AAS_CORE3_VERSION_MAJOR}.${AAS_CORE3_VERSION_MINOR}.${AAS_CORE3_VERSION_PATCH}
            CACHE STRING "version" FORCE
            )
endif ()

set(PROJECT_VERSION ${AAS_CORE3_VERSION})

OPTION(BUILD_TESTS "Build tests for aas-core-cpp" FALSE)

if (${BUILD_TESTS})
    # NOTE (mristin):
    # We need to use C++17 for the <filesystem> in tests.
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 11)
endif()

# NOTE (mristin):
# See: https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(nlohmann_json 3 CONFIG REQUIRED)
find_package(expat 2 CONFIG REQUIRED)
find_package(tl-optional 1 CONFIG REQUIRED)
find_package(tl-expected 1 CONFIG REQUIRED)

# NOTE (mristin):
# Since there is a lot of source code, we have to compile to large object files
# with MSVC; otherwise the error C1128 is raised.
# See: https://learn.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/fatal-error-c1128
IF (MSVC)
    add_compile_options(/bigobj)
ENDIF ()

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unknown-pragmas
            -Wno-unused-parameter
            -Wno-implicit-fallthrough
            -Wno-switch
    )
endif ()


# NOTE (mristin):
# We take the following tutorial as inspiration:
# https://www.foonathan.net/2016/03/cmake-install/

SET(HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/aas_core/aas_3_0")
SET(HEADER
        ${HEADER_PATH}/common.hpp
        ${HEADER_PATH}/constants.hpp
        ${HEADER_PATH}/enhancing.hpp
        ${HEADER_PATH}/iteration.hpp
        ${HEADER_PATH}/jsonization.hpp
        ${HEADER_PATH}/stringification.hpp
        ${HEADER_PATH}/types.hpp
        ${HEADER_PATH}/verification.hpp
        ${HEADER_PATH}/visitation.hpp
        ${HEADER_PATH}/wstringification.hpp
        ${HEADER_PATH}/xmlization.hpp
        )

SET(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
SET(SRC
        ${SRC_PATH}/common.cpp
        ${SRC_PATH}/constants.cpp
        ${SRC_PATH}/iteration.cpp
        ${SRC_PATH}/jsonization.cpp
        ${SRC_PATH}/stringification.cpp
        ${SRC_PATH}/types.cpp
        ${SRC_PATH}/verification.cpp
        ${SRC_PATH}/visitation.cpp
        ${SRC_PATH}/wstringification.cpp
        ${SRC_PATH}/xmlization.cpp
        )

# NOTE (mristin)
# MSVC does not automatically build .lib for DLLs.
# See: https://stackoverflow.com/questions/64088046/missing-lib-file-when-creating-shared-library-with-cmake-and-visual-studio-2019
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(aas_core3_static STATIC ${HEADER} ${SRC})
set_target_properties(aas_core3_static PROPERTIES
        PUBLIC_HEADER ${HEADER}
        )
# NOTE (mristin):
# We need to distinguish between BUILD and INSTALL interface,
# see: https://stackoverflow.com/questions/25676277/cmake-target-include-directories-prints-an-error-when-i-try-to-add-the-source
target_include_directories(aas_core3_static
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
target_link_libraries(aas_core3_static
        PRIVATE
        expat::expat
        PUBLIC
        nlohmann_json::nlohmann_json
        tl::optional
        tl::expected
        )

add_library(aas_core3 SHARED ${HEADER} ${SRC})

# NOTE (mristin):
# We need to distinguish between BUILD and INSTALL interface,
# see: https://stackoverflow.com/questions/25676277/cmake-target-include-directories-prints-an-error-when-i-try-to-add-the-source
target_include_directories(aas_core3
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )
set_target_properties(aas_core3 PROPERTIES
        PUBLIC_HEADER ${HEADER}
        )
target_link_libraries(aas_core3
        PRIVATE
        expat::expat
        PUBLIC
        nlohmann_json::nlohmann_json
        tl::optional
        tl::expected
        )

# Testing
OPTION(BUILD_TESTS "Build tests for aas-core-cpp" FALSE)

if (${BUILD_TESTS})
    # NOTE (mristin):
    # We need to use C++17 for the <filesystem>.
    set(CMAKE_CXX_STANDARD 17)

    include(CTest)
    enable_testing()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test-external)

    # region Common in test
    add_library(common_in_test test/common.hpp test/common.cpp)
    target_link_libraries(common_in_test aas_core3_static)

    add_library(common_jsonization_in_test
            test/common_jsonization.cpp
            test/common_jsonization.hpp
    )
    target_link_libraries(common_jsonization_in_test aas_core3_static)

    add_library(common_xmlization_in_test
            test/common_xmlization.cpp
            test/common_xmlization.hpp
            )
    target_link_libraries(common_xmlization_in_test aas_core3_static)

    add_library(common_examples_in_test
            test/common_examples.generated.hpp
            test/common_examples.generated.cpp
            )
    target_link_libraries(common_examples_in_test
            common_in_test
            common_xmlization_in_test
    )
    # endregion Common in test

    add_executable(test_examples test/test_examples.cpp)
    target_link_libraries(test_examples aas_core3_static)
    add_test(
        NAME test_examples
        COMMAND $<TARGET_FILE:test_examples>
    )

    # region Stringification
    add_executable(
            test_stringification_base64
            test/test_stringification_base64.cpp
    )
    target_link_libraries(test_stringification_base64 aas_core3_static)
    add_test(
            NAME test_stringification_base64
            COMMAND $<TARGET_FILE:test_stringification_base64>
    )

    add_executable(
            test_stringification_of_enums
            test/test_stringification_of_enums.generated.cpp
    )
    target_link_libraries(test_stringification_of_enums aas_core3_static)
    add_test(
            NAME test_stringification_of_enums
            COMMAND $<TARGET_FILE:test_stringification_of_enums>
    )
    # endregion Stringification

    # region Wstringification
    add_executable(
            test_wstringification_of_enums
            test/test_wstringification_of_enums.generated.cpp
    )
    target_link_libraries(test_wstringification_of_enums aas_core3_static)
    add_test(
            NAME test_wstringification_of_enums
            COMMAND $<TARGET_FILE:test_wstringification_of_enums>
    )
    # endregion Wstringification

    # region Jsonization
    add_executable(
            test_jsonization_of_concrete_classes
            test/test_jsonization_of_concrete_classes.generated.cpp
    )
    target_link_libraries(
            test_jsonization_of_concrete_classes
            aas_core3_static
            common_in_test
            common_jsonization_in_test
    )
    add_test(
            NAME test_jsonization_of_concrete_classes
            COMMAND $<TARGET_FILE:test_jsonization_of_concrete_classes>
    )

    add_executable(
            test_jsonization_dispatch
            test/test_jsonization_dispatch.generated.cpp
    )
    target_link_libraries(
            test_jsonization_dispatch
            aas_core3_static
            common_examples_in_test
            common_jsonization_in_test
    )
    add_test(
            NAME test_jsonization_dispatch
            COMMAND $<TARGET_FILE:test_jsonization_dispatch>
    )

    add_executable(
            test_jsonization_of_concrete_classes_outside_container
            test/test_jsonization_of_concrete_classes_outside_container.generated.cpp
    )
    target_link_libraries(
            test_jsonization_of_concrete_classes_outside_container
            aas_core3_static
            common_examples_in_test
            common_jsonization_in_test
    )
    add_test(
            NAME test_jsonization_of_concrete_classes_outside_container
            COMMAND $<TARGET_FILE:test_jsonization_of_concrete_classes_outside_container>
    )
    # endregion
    
    # region Xmlization
    add_executable(
            test_xmlization_of_concrete_classes
            test/test_xmlization_of_concrete_classes.generated.cpp
    )
    target_link_libraries(
            test_xmlization_of_concrete_classes
            aas_core3_static
            common_in_test
            common_xmlization_in_test
    )
    add_test(
            NAME test_xmlization_of_concrete_classes
            COMMAND $<TARGET_FILE:test_xmlization_of_concrete_classes>
    )

    add_executable(
            test_xmlization_dispatch
            test/test_xmlization_dispatch.generated.cpp
    )
    target_link_libraries(
            test_xmlization_dispatch
            aas_core3_static
            common_in_test
            common_examples_in_test
    )
    add_test(
            NAME test_xmlization_dispatch
            COMMAND $<TARGET_FILE:test_xmlization_dispatch>
    )

    add_executable(
            test_xmlization_of_concrete_classes_outside_container
            test/test_xmlization_of_concrete_classes_outside_container.generated.cpp
    )
    target_link_libraries(
            test_xmlization_of_concrete_classes_outside_container
            aas_core3_static
            common_in_test
            common_examples_in_test
    )
    add_test(
            NAME test_xmlization_of_concrete_classes_outside_container
            COMMAND $<TARGET_FILE:test_xmlization_of_concrete_classes_outside_container>
    )
    # endregion
endif()

# Set the version

configure_file(aas_core3-config-version.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/aas_core3-config-version.cmake @ONLY
        )

# Installation

include(GNUInstallDirs)

install(TARGETS aas_core3_static
        EXPORT aas_core3
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aas_core/aas_3_0
        )
install(TARGETS aas_core3
        EXPORT aas_core3
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aas_core/aas_3_0
        )
install(FILES
        aas_core3-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/aas_core3-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aas_core3
        )

install(EXPORT aas_core3
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/aas_core3"
        )
