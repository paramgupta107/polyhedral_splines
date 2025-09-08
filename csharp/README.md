# Polyhedral Net Splines C# Bindings

C# bindings for constructing Polyhedral Net Splines

---

## Table of Contents

1.  [Installation](#installation)

2.  [Quick Start](#quick-start)

3.  [Key Concepts](#key-concepts)

4.  [API Reference](#api-reference)

5.  [Citation](#citation)

---

## Installation

Clone git repositry and run the following commands in the project directory.
```bash
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build
cmake --install build
dotnet build ./csharp
```
Then, copy both DLL files in ./csharp and the OpenMesh DLL into your project.
---

## Quick Start

### 1 · PNS Control Net to BV

```cs
using PolyhedralNetSplines;

//Generates output.bv after raising patches to degree 3
PolyhedralNetSplines.CreateBVFromFile("cube.obj", "output.bv", true);
```

### 2 · Use PatchBuilder to support interactive modification

```cs
using PolyhedralNetSplines;

var controlMesh = PolyhedralNetSplines.PNSControlMesh.FromFile("cube.obj");
var bvWriter = new PolyhedralNetSplines.BVWriter("output2.bv");
bvWriter.Start();

foreach (var builder in PolyhedralNetSplines.PNS.GetPatchBuilders(controlMesh))
{
  foreach (var patch in builder.BuildPatches(controlMesh))
  {
    patch.DegRaise(); //Raises patch to degree 3
    bvWriter.Consume(patch); //Writes the patch to output.bv
  }
}

bvWriter.Stop();
```

### 3 · Using `process_mesh` helper

```cs
using PolyhedralNetSplines;

var controlMesh = PolyhedralNetSplines.PNSControlMesh.FromFile("cube.obj");
var bvWriter = new PolyhedralNetSplines.BVWriter("output3.bv");

//Performs the loop from example 2
PolyhedralNetSplines.PNS.ProcessMesh(controlMesh, bvWriter, true);
```
---

## Key Concepts

| Term | Description |
|  -----------------  |  -----------------------------------------  |
|  **PNSControlMesh**  | Polyhedral control net defining the surface domain. Is a thin wrapper class around an *OpenMesh* `MeshType`. |
|  **PatchBuilder**  | Factory that gathers a local neighborhood and produces one or more Bézier `Patch` objects. Can be used to re-generate patches with modifed vertex positions |
|  **Patch**  | Bézier surface patch with control points `BBCoefs` and degrees `DegU`, `DegV`. |
|  **PatchConsumer**  | Abstract writer that receives patches. Concrete implementations includes `BVWriter` and `IGSWriter`. |
|  **ProcessMesh**  | Convenient pipeline that combines *builders*, *degree elevation*, and a *consumer* in one call. |

  

## API Reference

###  `PNSControlMesh`

|  Method                       |  Description                                     |
|  ---------------------------  |  ----------------------------------------------  |
|  `FromData(vertices, faces)`  |  Builds a mesh from a float[] and int[][].       |
|  `FromFile(filename)`         |  Load OBJ/OFF/… via OpenMesh.                    |
|  `SetVertex(index, x, y, z)`  |  Move a single control point.                    |
|  `GetVertex(i)`               |  Get position tuple of ith vertex.               |
|  `GetVertices()`              |  Get list of all vertices.                       |
|  `GetFaces()`                 |  Gets jagged array of vertex indices for faces.  |



###  `PatchBuilder`

|  Property/Method       |  Description                                                          |
|  --------------------  |  -------------------------------------------------------------------  |
|  `NeighborVerts`       |  Indices of Pns patch neighborhood that are use to generate patches.  |
|  `Mask`                |  Linear transform to generate Patches from neighborhood.              |
|  `NumPatches`          |  Number of patches produced.                                          |
|  `DegU` / `DegV`       |  Parametric degrees.                                                  |
|  `PatchType`           |  Name of the patch type.                                              |
|  `BuildPatches(mesh)`  |  Constructs patches from the control mesh.                            |



###  `Patch`

|  Property         |  Description                                 |
|  ---------------  |  ------------------------------------------  |
|  `DegU` / `DegV`  |  Parametric degrees.                         |
|  `Group`          |  Group name of the patch.                    |
|  `IsValid`        |  Notes if patch is valid.                    |
|  `GetBBCoefs()`   |  Returns 2D array of Bézier control points.  |
|  `DegRaise()`     |  Elevate both degrees to 3.                  |

  

###  `PatchConsumer`
Abstract class for BVWriter and IGSWriter

|  Method            | Description                 |
|  ----------------  |  -------------------------  |
|  `Start()`         |  Starts the consumer        |
|  `Stop()`          |  Stops the consumer.        |
|  `Consume(patch)`  |  Writes patch to the file.  |

### `BVWriter(filename)`
### `IGSWriter(filename)`
### `STEPWriter(filename)`


### `PNS`
Container class for library methods.

|  Method                                                         |  Description                                                      |
|  -------------------------------------------------------------  |  ---------------------------------------------------------------  |
|  `ProcessMesh(controlMesh, consumer, isDegRaise)`               |  Constructs surface using a control net and writes it.            |
|  `GetPatchBuilders(mesh)`                                       |  Creates a PatchBuilder for each patch type in the control mesh.  |
|  `CreateBVFromFile(controlMeshFile, outputFile, degreeRaise)`   |  Write a BV file from a mesh file.                                |
|  `CreateBV(controlMesh, outputFile, degreeRaise)`               |  Write a BV file from a control mesh.                             |
|  `CreateIGSFromFile(controlMeshFile, outputFile, degreeRaise)`  |  Write an IGS file from a mesh file.                              |
|  `CreateIGS(controlMesh, outputFile, degreeRaise)`              |  Write an IGS file from a control mesh.                           |
|  `CreateSTEPFromFile(controlMeshFile, outputFile, degreeRaise)` |  Write an STEP file from a mesh file.                             |
|  `CreateSTEP(controlMesh, outputFile, degreeRaise)`             |  Write an STEP file from a control mesh.                          |

---

## Citation

Bi-cubic splines for polyhedral control nets, Jörg Peters , Kyle Lo, Kestutis Karciauskas TOMS , 2023 paper documentation