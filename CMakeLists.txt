cmake_minimum_required(VERSION 2.8)

project(pairoscope)

set(CMAKE_INSTALL_PREFIX "/usr")
set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_SOURCE_DIR}/build-common/cmake
)

include(TestHelper)
include(VersionHelper)
include(ProjectHelper)

configure_file(version/version.h.in version/version.h @ONLY)
include_directories(${PROJECT_BINARY_DIR}/version)

include(BuildSamtools)
include_directories(${Samtools_INCLUDE_DIRS})

find_package(Cairo REQUIRED)
include_directories(${Cairo_INCLUDE_DIRS})

if(${CMAKE_C_COMPILER_ID} MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic -fPIC")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -std=c++0x")
else()
    message(
        WARNING
        "Unknown compiler: ${CMAKE_C_COMPILER_ID}, we'll see how this goes..."
    )
endif()

# make sure to pick up headers from library dirs
include_directories("src/exe/pairoscope")

# main project

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_projects(src/exe)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    message("Doxygen found, documentation generation enabled")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
else(DOXYGEN_FOUND)
    message("Doxygen not found, documentation generation disabled")
endif(DOXYGEN_FOUND)

include(PackageInfo.cmake)
