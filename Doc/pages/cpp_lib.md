\page cpp_lib C++ Library

C++ Library to facilitate building interactive applications utilizing PnS.

### Public API reference 
- @ref api_group

@warning The Public API is uses PIMPL Idiom for binary compatibility. Use of any classes or functions outide the public library from pre-compiled binaries may face issues with binary compatibility.

## Header files

```cpp
#include "PolyhedralSplines/PnSpline.hpp"
#include "PolyhedralSplines/PnSPatch.hpp"
```

## Creating a PnSpline Object

@ref PnSpline object represent a Polyhedral-net-Spline surface. It is represented as peicewise polynomial patches in Bernstein–Bézier form. A PnSpline object can be created using the control-net data.

Create the data for the control net.

```cpp
std::vector< std::array< double, 3 > > cubeVertices = {
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1,  1},
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    {-1,  1, -1}
};

std::vector< std::vector< uint32_t > > cubeFaces = {
    {0, 1, 2, 3},
    {7, 6, 5, 4},
    {1, 0, 4, 5},
    {2, 1, 5, 6},
    {3, 2, 6, 7},
    {0, 3, 7, 4}
};
```
Create a \ref PnSpline object
```cpp
    PnSpline surface = PnSpline(cubeVertices, cubeFaces);
```

## Accessing the Piecwise Patches

@ref PnSPatch reresents a single peicewise polynomial patch in Bernstein–Bézier form.

A patch can be retireved using an index. The value for each dimention for each coefficient can then be accessed.

```cpp
for(uint32_t p = 0; p < surface.numPatches(); p++){

    PnSPatch patch = surface.getPatch(p);

    for(uint32_t u = 0; u < patch.getDegreeU() + 1; u++){
        for(uint32_t v= 0; v < patch.getDegreeV() + 1; v++){
            double x = patch(u, v, 0);
            double y = patch(u, v, 1);
            double z = patch(u, v, 2);
        }
    }
}
```

## Updating the Control Net

The control net for a PnSpline can be efficiently updated using the @ref PnSpline::updateControlMesh "updateControlMesh" function. This will only regenerate the required patches. Making it a lot faster than creating a new PnSpline from scratch, especially for large control nets. 

```cpp
std::vector<std::array<double,3>> updatedVertices = {
    {-2, -1,  1},
    { 2, -1,  1}
};
std::vector<uint32_t> updateIndices = {0, 1};

std::vector<uint32_t> upadatedPatchesIndices = surface.updateControlMesh(updatedVertices, updateIndices);

```

Each element in `upadatedPatchesIndices` represents the index to a patch that has been modified. The remaining patches remain unchanged. 

@note Modifying the topology/connectivity of the control net requires a new PnSpline to be created.