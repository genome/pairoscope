cmake_minimum_required(VERSION 2.6)

# .deb packaging
set(ARCH "i686")
if(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    set(ARCH "x86_64")
endif ()

# The format of the description field is a short summary line followed by a
# longer paragraph indented by a single space on each line
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple static grapher of abnormal read pairs in BAM files.
 Pairoscope (formerly Yenta) was developed as a quick and simple way to generate diagrams indicating the relationship of paired end sequencing reads. It functions by displaying multiple genomic regions, their read depth at each base in the region and arcs within or between regions to indicate pairing information. Currently, Pairoscope supports the graphing of multiple regions, but they must be at different locations: the behavior of Pairoscope is undefined if attempting to graph the same region twice. It uses a bam file as input, and color codes the read pair graph to reflect different types of abnormally oriented reads. Individual reads are represented as vertical lines drawn at the genomic location where the read mapped. When both reads of a pair are present in the displayed region, the reads are linked by an arc. Reads lying at the same position are drawn on top of each other. In addition to displaying read pair information, Pairoscope also displays the read depth over the region as a separate graph. At the moment, Pairoscope only provides display of reads with abnormal orientations with respect to the reference; deletion and insertion events are not yet supported. In addition, it assumes orientations and colors reflecting the expected orientations for a standard Illumina Paired-End library. It is in beta right now and stability may be an issue.")    

set(CPACK_PACKAGE_NAME "pairoscope${EXE_VERSION_SUFFIX}")
set(CPACK_PACKAGE_VENDOR "The Genome Institute")
set(CPACK_PACKAGE_VERSION ${FULL_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "David Larson <dlarson@genome.wustl.edu>")
set(CPACK_SYSTEM_NAME "Linux-${ARCH}")
set(CPACK_TOPLEVEL_TAG "Linux-${ARCH}")
set(CPACK_DEBIAN_PACKAGE_SECTION science)
set(CPACK_DEBIAN_PACKAGE_PRIORITY optional)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcairo (>= 1.6.0)")
if (CMAKE_BUILD_TYPE MATCHES package)
    set(CPACK_GENERATOR "DEB")
else(CMAKE_BUILD_TYPE MATCHES package)
    set(CPACK_GENERATOR "TGZ")
endif(CMAKE_BUILD_TYPE MATCHES package)

configure_file(debian/postinst.in debian/postinst @ONLY)
configure_file(debian/prerm.in debian/prerm @ONLY)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "debian/postinst;debian/prerm")

include(CPack)
