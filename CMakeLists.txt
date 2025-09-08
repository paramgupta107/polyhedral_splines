cmake_minimum_required(VERSION 3.15)
project(PolyhedralSplines)

#-------------------------------------------------------------------------------
# Configuration options
#-------------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# Required by OpenMesh
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_definitions(_USE_MATH_DEFINES OM_STATIC_BUILD)

# Unix compilation settings
if(UNIX)
    add_compile_options(-Wfatal-errors -w)
endif()

#-------------------------------------------------------------------------------
# Emscripten configuration
#-------------------------------------------------------------------------------
if(EMSCRIPTEN)
    message(STATUS "Building for Emscripten")
    message(STATUS "EMSCRIPTEN_ROOT: ${EMSCRIPTEN_ROOT}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

    set(EM_COMPILE_OPTIONS -fexceptions)
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
else()
    message(STATUS "Building for native")
endif()

#-------------------------------------------------------------------------------
# External dependencies
#-------------------------------------------------------------------------------
set(OPENMESH_ROOT ${CMAKE_SOURCE_DIR}/External/OpenMesh)


if (NOT CMAKE_BUILD_TYPE)
  message (STATUS "Defaulting to 'Release' mode as none was specified")
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type" FORCE)
endif()

include(ExternalProject)
ExternalProject_Add(
    OpenMesh
    PREFIX ${OPENMESH_ROOT}
    GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    GIT_TAG f13a3bf79f8dc91cd453b74baa9dc6f97a5a3062
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
               -DBUILD_APPS=FALSE
               -DOPENMESH_BUILD_SHARED=${BUILD_SHARED_LIBS}
               -DCMAKE_CXX_STANDARD=17
               -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
               -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DWIN32:BOOL=TRUE
)


set(OPENMESH_LIBS
    debug OpenMeshCored optimized OpenMeshCore
    debug OpenMeshToolsd optimized OpenMeshTools
)

link_directories(${OPENMESH_ROOT}/lib)
include_directories(SYSTEM ${OPENMESH_ROOT}/include)

#-------------------------------------------------------------------------------
# Collect source files
#-------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES src/*.cpp)


# Create the library target
if(BUILD_SHARED_LIBS)
    add_library(PolyhedralSplinesLib SHARED ${SOURCES})
else()
    add_library(PolyhedralSplinesLib STATIC ${SOURCES})
endif()

# Remove main.cpp from the library sources
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Add conditional source files
if(BUILD_SHARED_LIBS)
    target_sources(PolyhedralSplinesLib PRIVATE csharp/csBindings.cpp)
endif()

if(NOT EMSCRIPTEN)
    add_executable(PolyhedralSplines src/main.cpp)
else()
    add_executable(PolyhedralSplines wasm/wasmBindings.cpp)
    target_sources(PolyhedralSplinesLib PRIVATE wasm/wasmBindings.cpp)
endif()

# Create the executable target
target_sources(PolyhedralSplines PRIVATE ${SOURCES})

#-------------------------------------------------------------------------------
# Configure targets
#-------------------------------------------------------------------------------
foreach(TARGET PolyhedralSplines PolyhedralSplinesLib)
    target_include_directories(${TARGET} PRIVATE src)
    target_link_libraries(${TARGET} ${OPENMESH_LIBS})
    add_dependencies(${TARGET} OpenMesh)
    
    if(EMSCRIPTEN)
        target_compile_options(${TARGET} PRIVATE ${EM_COMPILE_OPTIONS})
        target_link_options(${TARGET} PRIVATE ${EM_LINK_OPTIONS})
    endif()
endforeach()

#-------------------------------------------------------------------------------
# Embed table files into a generated .cpp file
#-------------------------------------------------------------------------------
set(TABLE_SRC_PATH ${CMAKE_SOURCE_DIR}/src/Patch/Table)
set(EMBEDDED_TABLE_CPP "${CMAKE_BINARY_DIR}/EmbeddedTables.cpp")

file(GLOB_RECURSE TABLE_FILES "${TABLE_SRC_PATH}/*.csv")
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

target_sources(PolyhedralSplinesLib PRIVATE "${EMBEDDED_TABLE_CPP}")
target_sources(PolyhedralSplines PRIVATE "${EMBEDDED_TABLE_CPP}")
add_dependencies(PolyhedralSplines EmbedTables)
add_dependencies(PolyhedralSplinesLib EmbedTables)

#-------------------------------------------------------------------------------
# Python Bindings
#-------------------------------------------------------------------------------
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ 
        DESTINATION include/PolyhedralSplines
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.hpp")

install(TARGETS PolyhedralSplinesLib
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

if(BUILD_SHARED_LIBS)
    install(CODE "
        file(INSTALL
        DESTINATION \"${CMAKE_SOURCE_DIR}/csharp\"
        TYPE FILE
        FILES \"\$<TARGET_FILE:PolyhedralSplinesLib>\")
    ")
endif()