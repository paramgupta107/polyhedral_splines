cmake_minimum_required(VERSION 3.15)
project(PolyhedralSplines)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Required by OpenMesh
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True )

if(EMSCRIPTEN)
    message(STATUS "Building for Emscripten")
    message(STATUS "EMSCRIPTEN_ROOT: ${EMSCRIPTEN_ROOT}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(STATUS "Building for native")
endif()

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
    GIT_TAG f13a3bf79f8dc91cd453b74baa9dc6f97a5a3062
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
               -DBUILD_APPS=FALSE
               -DOPENMESH_BUILD_SHARED=FALSE
               -DCMAKE_BUILD_TYPE=RELEASE
               -DCMAKE_CXX_STANDARD=17
               -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
               -DBUILD_SHARED_LIBS=FALSE
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DWIN32:BOOL=TRUE
)

################################################################################

link_directories(${OPENMESH_ROOT}/lib)

# Find source files
file(GLOB_RECURSE SOURCES src/*.cpp)

if(EMSCRIPTEN)
    list(APPEND SOURCES wasm/wasmBindings.cpp)
endif()

add_executable(PolyhedralSplines src/main.cpp ${SOURCES})

target_include_directories(PolyhedralSplines PRIVATE src)

# Exclude main.cpp from library
set(MAIN_SRC src/main.cpp)
list(REMOVE_ITEM SOURCES ${MAIN_SRC})

# Build library
if (BUILD_SHARED_LIBS)
    add_library(PolyhedralSplinesLib SHARED ${SOURCES})
else()
    add_library(PolyhedralSplinesLib STATIC ${SOURCES})
endif()
target_include_directories(PolyhedralSplinesLib PRIVATE src)
add_dependencies(PolyhedralSplinesLib OpenMesh)
set_property(TARGET PolyhedralSplinesLib PROPERTY POSITION_INDEPENDENT_CODE ON)
### Link OpenMesh ###
include_directories(SYSTEM ${OPENMESH_ROOT}/include)
target_link_libraries(PolyhedralSplines OpenMeshCore OpenMeshTools)

# OpenMesh need to be built before main program
add_dependencies(PolyhedralSplines OpenMesh)

target_link_libraries(PolyhedralSplinesLib PUBLIC OpenMeshCore OpenMeshTools)

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
# build the Python module
# ------------------------------------------------------------------------------
add_subdirectory(python)

# ------------------------------------------------------------------------------
# Install headers and lib
# ------------------------------------------------------------------------------

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ DESTINATION include/PolyhedralSplines
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.hpp")

install(TARGETS PolyhedralSplinesLib
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

if(EMSCRIPTEN)    
    set(EM_COMPILE_OPTIONS
    -fexceptions
    )

    set(EM_LINK_OPTIONS
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"UTF8ToString\",\"FS\", \"HEAPF32\", \"HEAPU32\", \"HEAPU8\"]"
    "-sEXPORTED_FUNCTIONS=[\"_malloc\",\"_free\"]"
    "-sNO_DISABLE_EXCEPTION_CATCHING"
    "-fexceptions"
    "-sINITIAL_MEMORY=1GB"
    "-sMAXIMUM_MEMORY=4GB"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMODULARIZE"
    "-sEXPORT_NAME=createModule"
    )
    target_compile_options(PolyhedralSplines PRIVATE ${EM_COMPILE_OPTIONS})
    target_link_options(PolyhedralSplines PRIVATE ${EM_LINK_OPTIONS})

    target_compile_options(PolyhedralSplinesLib PRIVATE ${EM_COMPILE_OPTIONS})
    target_link_options(PolyhedralSplinesLib PRIVATE ${EM_LINK_OPTIONS})
endif()