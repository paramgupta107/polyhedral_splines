find_package(pybind11 REQUIRED)

pybind11_add_module(polyhedral_net_splines
    pybind.cpp
)

target_include_directories(polyhedral_net_splines
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(polyhedral_net_splines
    PRIVATE
    PolyhedralSplinesLib
)

install(TARGETS polyhedral_net_splines LIBRARY DESTINATION polyhedral_net_splines)