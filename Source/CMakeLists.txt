cmake_minimum_required (VERSION 3.0)
project (SemiStructuredSpline)

# Required by OpenMesh
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Force make to stop after first error
add_definitions(
    -Wfatal-errors # stop after first error
)

# Find source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Add executable
add_executable(SemiStructuredSpline src/main.cpp ${SOURCES})

# Add OpenMesh & Eigen
include(ExternalProject)

# Clone and install OpenMesh in External folder
set(OPENMESH_ROOT ${CMAKE_SOURCE_DIR}/External/OpenMesh)

ExternalProject_Add(
    OpenMesh
	PREFIX ${OPENMESH_ROOT}
	GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
	GIT_TAG db8c0608830ae7324b2b2e1d91e4db565b2d3eae
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

include_directories(${OPENMESH_ROOT}/include)

# The extension for dynamic library in macOS and Linux is different
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


# Clone and install OpenMesh in External folder
set(EIGEN_ROOT ${CMAKE_SOURCE_DIR}/External/Eigen)

ExternalProject_Add(
    Eigen
	PREFIX ${EIGEN_ROOT}
	GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
	GIT_TAG 53a7864c48a2b2fc226c69e9184d7cc8e6b36b6d
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

include_directories(${EIGEN_ROOT}/include/eigen3)

# Copy testAllFiles.sh to build folder
add_custom_command(
    TARGET SemiStructuredSpline
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/testAllFiles.sh ${CMAKE_SOURCE_DIR}/../build
)
