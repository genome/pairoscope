cmake_minimum_required(VERSION 2.8)

project(pairoscope)

set(CMAKE_INSTALL_PREFIX "/usr")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build-common/cmake")
include(TestHelper)
include(VersionHelper)
include(ProjectHelper)

configure_file(version/version.h.in version/version.h @ONLY)
include_directories(${PROJECT_BINARY_DIR}/version)

find_package(Samtools 0.1.9 REQUIRED)
include_directories(${Samtools_INCLUDE_DIRS})

find_package(Cairo REQUIRED)
include_directories(${Cairo_INCLUDE_DIRS})

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "-Wall")
endif ()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-Wall -ansi -pedantic -std=c++0x")
endif ()

# make sure to pick up headers from library dirs
include_directories("src/exe/pairoscope")

# main project

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_projects(src/exe)

include(PackageInfo.cmake)
