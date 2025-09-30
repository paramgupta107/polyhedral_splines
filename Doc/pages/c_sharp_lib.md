\page c_sharp_lib C# Library

C# bindings for 

### Public API reference 
- @ref api_group_cs


## Importing Library

Include the PolyhedralNetSplines.dll and PolyhedralSplinesLib.dll files within your project, for more detailed info, see \ref c_sharp_compile
```cs
using PolyhedralNetSplines;
```

## Creating a PnSpline Object

@ref PolyhedralNetSplines.PnSpline "PnSpline" objects represent a Polyhedral-net-Spline surface. It is represented as peicewise polynomial patches in Bernstein–Bézier form. A @ref PolyhedralNetSplines.PnSpline "PnSpline" object can be created using the control-net data.

Create the data for the control net.

```cs
List<int[]> cubeVertices = new List<int[]>
{
  new int[] {-1, -1,  1},
  new int[] { 1, -1,  1},
  new int[] { 1,  1,  1},
  new int[] {-1,  1,  1},
  new int[] {-1, -1, -1},
  new int[] { 1, -1, -1},
  new int[] { 1,  1, -1},
  new int[] {-1,  1, -1}
};

List<int[]> cubeFaces = new List<int[]>
{
  new int[] {0, 1, 2, 3},
  new int[] {7, 6, 5, 4},
  new int[] {1, 0, 4, 5},
  new int[] {2, 1, 5, 6},
  new int[] {3, 2, 6, 7},
  new int[] {0, 3, 7, 4}
};
```
Create a @ref PolyhedralNetSplines.PnSpline "PnSpline" object
```cs
    var surface = PolyhedralNetSplines.PnSpline(cubeVertices, cubeFaces);
```

## Accessing the Piecewise Patches

A @ref PolyhedralNetSplines.PnSPatch "PnSPatch" reresents a single peicewise polynomial patch in Bernstein–Bézier form.
A patch can be retireved using an index. The value for each dimention for each coefficient can then be accessed.

```cs
for(int p = 0; p < surface.numPatches(); p++) {

  var patch = surface.getPatch(p);

  for(int u = 0; u < patch.getDegreeU() + 1; u++) {
    for(uint32_t v= 0; v < patch.getDegreeV() + 1; v++) {
        double x = patch(u, v, 0);
        double y = patch(u, v, 1);
        double z = patch(u, v, 2);
    }
  }
}
```

## Updating the Control Net

The control net for a @ref PolyhedralNetSplines.PnSpline "PnSpline" can be efficiently updated using the @ref PolyhedralNetSplines.PnSpline::UpdateControlMesh "UpdateControlMesh" function, which only regenerates the required patches. This is a lot faster than creating a new @ref PolyhedralNetSplines.PnSpline "PnSpline" from scratch, especially for large control nets. 

```cs
List<int[]> updatedVertices = new List<int[]>
{
  new int[] {-2, -1,  1},
  new int[] { 2, -1,  1}
};

List<int> updateIndices = new List<int> {0, 1};
List<int> updatedPatchIndices = surface.updateControlMesh(updatedVertices, updateIndices)
```

Each element in `updatedPatchesIndices` represents the index to a patch that has been modified. The remaining patches remain unchanged. 

@note Modifying the topology/connectivity of the control net requires a new @ref PolyhedralNetSplines.PnSpline "PnSpline" to be created.