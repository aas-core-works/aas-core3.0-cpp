vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO aas-core-works/aas-core3.0-cpp
        REF "${VERSION}"
        SHA512 0  # This is a temporary value. We will modify this value in the next section.
        HEAD_REF main
)


vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME "aas-core3-0-cpp")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
configure_file("${CMAKE_CURRENT_LIST_DIR}/usage-for-vcpkg" "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)
