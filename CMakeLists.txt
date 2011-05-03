#Test cmake list file for pairoscope
PROJECT(pairoscope)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
SET(SOURCES src/pairoscope.cpp src/YAlignmentFetcher.cpp src/YGeneView.cpp
    src/YPairView.cpp src/YView.cpp src/YDepthView.cpp src/YGenomeView.cpp
    src/YTranscriptFetcher.cpp)
INCLUDE_DIRECTORIES(src)
ADD_EXECUTABLE(pairoscope ${SOURCES})

find_package(Cairo REQUIRED)
include_directories(${Cairo_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Cairo_LIBRARIES})


find_package(Samtools REQUIRED)
include_directories(${Samtools_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Samtools_LIBRARIES})

target_link_libraries(pairoscope ${LIBS})
