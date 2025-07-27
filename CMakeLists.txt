cmake_minimum_required(VERSION 3.10)

# Project name
project(MTDS)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Define paths for headers and source files
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PRE_PROC_SRC_DIR ${CMAKE_SOURCE_DIR}/preProcessing/src)
set(PRE_PROC_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/preProcessing/include)

# Include directories
include_directories(${INCLUDE_DIR} ${PRE_PROC_INCLUDE_DIR})

# Collect all source files
file(GLOB SOURCES
    ${SRC_DIR}/*.cpp
    ${PRE_PROC_SRC_DIR}/*.cpp
)

# Collect all header files (optional, for IDEs)
file(GLOB HEADERS
    ${INCLUDE_DIR}/*.h
    ${PRE_PROC_INCLUDE_DIR}/*.h
)





add_executable(${PROJECT_NAME}
    main.cpp
    ${SOURCES}
    ${HEADERS}  # Optional: Include headers for IDE indexing
    ${UI}
)
target_include_directories(MTDS PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

