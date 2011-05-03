# - Try to find Samtools 
# Once done, this will define
#
#  Samtools_FOUND - system has Samtools
#  Samtools_INCLUDE_DIRS - the Samtools include directories
#  Samtools_LIBRARIES - link these to use Samtools

include(LibFindMacros)

# Dependencies
libfind_package(Samtools ZLIB)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Samtools_PKGCONF samtools)

# Include dir
find_path(Samtools_INCLUDE_DIR
  NAMES bam.h
  PATHS ${Samtools_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(Samtools_LIBRARY
  NAMES bam libbam.a bam.a
  PATHS ${Samtools_PKGCONF_LIBRARY_DIRS} ${Samtools_INCLUDE_DIR}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this lib depends on.
set(Samtools_PROCESS_INCLUDES Samtools_INCLUDE_DIR ZLIB_INCLUDE_DIR)
set(Samtools_PROCESS_LIBS Samtools_LIBRARY ZLIB_LIBRARIES)
libfind_process(Samtools)

