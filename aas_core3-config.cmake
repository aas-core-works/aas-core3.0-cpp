# NOTE (mristin):
# This file is based on:
# https://www.foonathan.net/2016/03/cmake-install/

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/aas_core3.cmake)

include(CMakeFindDependencyMacro)

find_dependency(nlohmann_json 3 REQUIRED)
find_dependency(expat 2 REQUIRED)
find_dependency(tl-optional 1 REQUIRED)
find_dependency(tl-expected 1 REQUIRED)
