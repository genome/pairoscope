cmake_minimum_required(VERSION 2.8)

project(pairoscope)

set(SOURCES 
    YAlignmentFetcher.cpp
    YAlignmentFetcher.h
    YColor.h
    YDepthView.cpp
    YDepthView.h
    YGeneView.cpp
    YGeneView.h
    YGenomeView.cpp
    YGenomeView.h
    YMatePair.h
    YPairView.cpp
    YPairView.h
    YPoint.h
    YRect.h
    YTranscript.h
    YTranscriptFetcher.cpp
    YTranscriptFetcher.h
    YTranscriptSubStructure.h
    YView.cpp
    YView.h
    pairoscope.cpp
    stdhash.hh
)

set(EXECUTABLE_NAME pairoscope)
add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${Samtools_LIBRARIES} ${Cairo_LIBRARIES})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES PACKAGE_OUTPUT_NAME ${EXECUTABLE_NAME}${EXE_VERSION_SUFFIX})
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin/)
