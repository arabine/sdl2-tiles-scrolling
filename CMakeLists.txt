cmake_minimum_required(VERSION 3.5)

set(PROJ_NAME sdl2-tiles-scrolling )

project(${PROJ_NAME} LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2)

set(CMAKE_INSTALL_RPATH "\$ORIGIN/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

add_executable(${PROJ_NAME} main.cpp glad.c)
target_include_directories(${PROJ_NAME} PUBLIC glad)
target_link_libraries(${PROJ_NAME} SDL2 dl)

set(BINARY_DESTINATION_DIR ".")
set(DATA_DESTINATION_DIR ".")
set(DOCS_DESTINATION_DIR ".")
set(MAN_DESTINATION_DIR ".")



set(CPACK_SYSTEM_NAME "x86_64")

set(CPACK_PACKAGE_NAME ${PROJ_NAME})
set(CPACK_PACKAGE_VENDOR "Anthony Rabine")
set(CPACK_PACKAGE_DESCRIPTION "sdl2-tiles-scrolling")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "sdl2-tiles-scrolling")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.d8s.eu")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "sdl2-tiles-scrolling")
set(CPACK_PACKAGE_CHECKSUM "SHA256")

set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_MONOLITHIC_INSTALL YES)
set(CPACK_PACKAGE_EXECUTABLES ${PROJ_NAME})
set(CPACK_STRIP_FILES YES)
set(CPACK_SET_DESTDIR true)
set(CPACK_INSTALL_PREFIX /) # suppress /usr/lib

set(CPACK_GENERATOR "TXZ")

include(CPack)


# Transfer the executable name to "install" world
install(CODE "set(PROJ_NAME \"${PROJ_NAME}\")")

# Install all dependencies we can resolve, with the exception of ones that
# every Linux machine should have. This should make this build as generic
# as possible, where it runs on any machine with the same or newer libc
# than the one this is compiled with.
# We copy these libraries into lib/ folder, so they can be found on game
# startup. See comment in root CMakeLists.txt for how this works exactly.
install(CODE [[
    file(GET_RUNTIME_DEPENDENCIES
            RESOLVED_DEPENDENCIES_VAR DEPENDENCIES
            UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPENDENCIES
            EXECUTABLES ${PROJ_NAME}
            POST_EXCLUDE_REGEXES "ld-linux|libc.so|libdl.so|libm.so|libgcc_s.so|libpthread.so|librt.so|libstdc...so")
    file(INSTALL
            DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
            FILES ${DEPENDENCIES}
            FOLLOW_SYMLINK_CHAIN)

    # This should not be possible, but error out when a dependency cannot
    # be resolved.
    list(LENGTH UNRESOLVED_DEPENDENCIES UNRESOLVED_LENGTH)
    if(${UNRESOLVED_LENGTH} GREATER 0)
        message(FATAL_ERROR "Unresolved dependencies: ${UNRESOLVED_DEPENDENCIES}")
    endif()
]])

install(TARGETS ${PROJ_NAME}
        RUNTIME
            DESTINATION ${BINARY_DESTINATION_DIR}
            COMPONENT Runtime
        )

install(FILES
            ${CMAKE_SOURCE_DIR}/background.png
            ${CMAKE_SOURCE_DIR}/tile.png
        DESTINATION ${DOCS_DESTINATION_DIR}
        COMPONENT assets)
