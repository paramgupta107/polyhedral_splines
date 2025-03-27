cmake_minimum_required(VERSION 3.9)
project(PolyhedralSplines)

# Required by OpenMesh
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force make to stop after first error
if(UNIX)
    add_definitions(
        -Wfatal-errors # stop after first error
        -w
    )
endif()

add_compile_definitions(_USE_MATH_DEFINES)

################################################################################
# Clone and build install dependencies
################################################################################
include(ExternalProject)

# OpenMesh
set(OPENMESH_ROOT ${CMAKE_SOURCE_DIR}/External/OpenMesh)

ExternalProject_Add(
    OpenMesh
    PREFIX ${OPENMESH_ROOT}
    GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    GIT_TAG db8c0608830ae7324b2b2e1d91e4db565b2d3eae
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
               -DBUILD_APPS=FALSE
               -DBUILD_SHARED=FALSE
               -DBUILD_STATIC=TRUE
               -DCMAKE_BUILD_TYPE=RELEASE
)

################################################################################

link_directories(${OPENMESH_ROOT}/lib)

# Find source files
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(PolyhedralSplines src/main.cpp ${SOURCES})

# Exclude main.cpp from library
set(MAIN_SRC src/main.cpp)
list(REMOVE_ITEM SOURCES ${MAIN_SRC})

# Build static library
add_library(PolyhedralSplinesLib STATIC ${SOURCES})
add_dependencies(PolyhedralSplinesLib OpenMesh)

### Link OpenMesh ###
include_directories(SYSTEM ${OPENMESH_ROOT}/include)
target_link_libraries(PolyhedralSplines OpenMeshCore OpenMeshTools)

# OpenMesh need to be built before main program
add_dependencies(PolyhedralSplines OpenMesh)

target_link_libraries(PolyhedralSplinesLib OpenMeshCore OpenMeshTools)

# ------------------------------------------------------------------------------
# Embed table files into a generated .cpp file
# ------------------------------------------------------------------------------

set(TABLE_SRC_PATH ${CMAKE_SOURCE_DIR}/src/Patch/Table)
file(GLOB_RECURSE TABLE_FILES "${TABLE_SRC_PATH}/*.csv")

set(EMBEDDED_TABLE_CPP "${CMAKE_BINARY_DIR}/embedded_tables.cpp")
file(WRITE "${EMBEDDED_TABLE_CPP}" "#include <unordered_map>\n#include <string>\n\n")
file(APPEND "${EMBEDDED_TABLE_CPP}" "static std::unordered_map<std::string, const char*> g_embedded_tables = {\n")

foreach(TABLE_FILE ${TABLE_FILES})
    get_filename_component(FILENAME ${TABLE_FILE} NAME)
    file(READ ${TABLE_FILE} FILE_CONTENTS)

    # Escape backslashes, quotes, and newlines
    string(REPLACE "\\" "\\\\" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "\"" "\\\"" FILE_CONTENTS "${FILE_CONTENTS}")
    string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENTS "${FILE_CONTENTS}")

    file(APPEND "${EMBEDDED_TABLE_CPP}" "  { \"${FILENAME}\", \"${FILE_CONTENTS}\" },\n")
endforeach()

file(APPEND "${EMBEDDED_TABLE_CPP}" "};\n\n")
file(APPEND "${EMBEDDED_TABLE_CPP}" "const std::unordered_map<std::string, const char*>& get_embedded_tables() {\n")
file(APPEND "${EMBEDDED_TABLE_CPP}" "  return g_embedded_tables;\n}\n")

add_custom_target(EmbedTables DEPENDS "${EMBEDDED_TABLE_CPP}")
add_dependencies(PolyhedralSplines EmbedTables)
add_dependencies(PolyhedralSplinesLib EmbedTables)
target_sources(PolyhedralSplinesLib PRIVATE "${EMBEDDED_TABLE_CPP}")
target_sources(PolyhedralSplines PRIVATE "${EMBEDDED_TABLE_CPP}")

# ------------------------------------------------------------------------------
# Install headers and lib
# ------------------------------------------------------------------------------

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION include/PolyhedralSplines
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.hpp")

install(TARGETS PolyhedralSplinesLib
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
