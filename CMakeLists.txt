cmake_minimum_required(VERSION 3.10)
project(meowpass VERSION 1.0.0 LANGUAGES C)

# C11 standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -pedantic)

# Build type defaults to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Release optimizations
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

# Source files
set(SOURCES
    src/main.c
    src/config.c
    src/password.c
    src/complexity.c
    src/catnames.c
    src/display.c
    src/update.c
    src/random.c
    tests/test_meowpass.c
)

# Create executable
add_executable(meowpass ${SOURCES})

# Link math library
target_link_libraries(meowpass m)

# Install targets
include(GNUInstallDirs)

install(TARGETS meowpass
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install man page
install(FILES meowpass.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "meowpass")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cat Name Based Secure Password Generator")
set(CPACK_PACKAGE_VENDOR "MeowPassword")

include(CPack)
