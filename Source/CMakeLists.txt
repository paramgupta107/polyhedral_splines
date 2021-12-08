cmake_minimum_required (VERSION 3.9)
project (SemiStructuredSpline)

# Required by OpenMesh
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Force make to stop after first error
add_definitions(
    -Wfatal-errors # stop after first error
    -w
)

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
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_CXX_FLAGS="-w"
    BUILD_COMMAND make -j6
    INSTALL_COMMAND make install -j6
)

################################################################################

# Needed by macOS (@rpath issue)
link_directories(${OPENMESH_ROOT}/lib)

# Find source files
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(SemiStructuredSpline src/main.cpp ${SOURCES})


### Link OpenMesh ###
include_directories(SYSTEM ${OPENMESH_ROOT}/include)

if(APPLE)
    set(LIB_EXTENSION dylib)
else()
    set(LIB_EXTENSION so)
endif()

target_link_libraries(
    SemiStructuredSpline
    ${OPENMESH_ROOT}/lib/libOpenMeshCore.${LIB_EXTENSION}
    ${OPENMESH_ROOT}/lib/libOpenMeshTools.${LIB_EXTENSION}
)


# OpenMesh need to be built before main program
add_dependencies(SemiStructuredSpline OpenMesh)
