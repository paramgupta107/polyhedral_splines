# Polyhedral Net Splines Python Bindings
  

Python bindings for constructing Polyhedral Net Splines

---

## Table of Contents

1.  [Installation](#installation)

2.  [Quick Start](#quick-start)

3.  [Key Concepts](#key-concepts)

4.  [API Reference](#api-reference)

5.  [Citation](#citation)

---


## Installation

Clone git repositry and run the following command inthe project directory.
```bash
pip  install  .
```
---

## Quick Start

### 1 · Pns Control Net to BV

```python
import polyhedral_net_splines as pns

# Writes output.bv after elevating patches to degree 3
pns.create_bv_from_file("cube.obj",  "output.bv",  degreeRaise=True)
```

### 2 · Use PatchBuilder to support interactive modification

```python
import polyhedral_net_splines as pns

control_mesh = pns.Pns_control_mesh.from_file("cube.obj")
bv_writer = pns.BVWriter("output2.bv")
bv_writer.start()

for builder in pns.get_patch_builders(control_mesh):
	for patch in builder.build_patches(control_mesh):
		patch.degRaise()  # optional degree‑3 elevation
		bv_writer.consume(patch)
bv_writer.stop()
```

### 3 · Using `process_mesh` helper

```python
control_mesh = pns.Pns_control_mesh.from_file("cube.obj")
bv_writer = pns.BVWriter("output3.bv")

# Internally performs the same loop as example 2
pns.process_mesh(control_mesh, bv_writer,  is_deg_raise=True)
```
---

## Key Concepts

| Term | Description |
|  -----------------  |  -----------------------------------------------------------------------------------------------------------------------------------  |
|  **Control Mesh**  | Polyhedral contorl net defining the surface domain. Represented by `Pns_control_mesh`, a thin wrapper around *OpenMesh*. |
|  **PatchBuilder**  | Factory that gathers a local neighbourhood and produces one or more Bézier **Patch** objects. Can be used to re-generate patches with modifed vertex positions |
|  **Patch**  | Bézier surface patch with control points `bb_coefs` and degrees `deg_u`, `deg_v`. |
|  **PatchConsumer**  | Abstract writer that receives patches. Concrete implementations include `BVWriter` and `IGSWriter`. |
|  **process_mesh**  | Convenience pipeline that combines *builders*, *degree elevation*, and a *consumer* in one call. |
---

  

## API Reference

###  `Pns_control_mesh`

  

| Method | Purpose |
|  -------------------------  |  -------------------------------  |
|  `from_data(verts, faces)`  | Build a mesh from Python lists. |
|  `from_file(path)`  | Load OBJ/OFF/… via OpenMesh. |
|  `set_vertex(i, x, y, z)`  | Move a control point. |
|  `get_vertex(i)`  |  `(x, y, z)` tuple. |
|  `get_vertices()`  | List of all vertices. |
|  `get_faces()`  | Face → vertex index lists. |


###  `get_patch_builders(mesh)`

Returns a list of `PatchBuilder` instances covering the mesh.

 
###  `PatchBuilder`

| Property/Method | Description |
|  ---------------------  |  ----------------------------------------  |
|  `neighbor_verts`  | Indices of Pns patch neighborhood that are use to generate patches. |
|  `mask`  | Linear transform to generate Patches from neighborhood. |
|  `num_patches`  | Number of patches produced. |
|  `build_patches(mesh)`  | Returns a list of `Patch`. |


###  `Patch`

 
| Property | Meaning |
|  -----------------  |  -----------------------------------  |
|  `deg_u` / `deg_v`  | Parametric degrees. |
|  `bb_coefs`  | 2‑D array of Bézier control points. |
|  `degRaise()`  | Elevate both degrees to 3. |

  

###  `PatchConsumer`

Base class with: `start()`, `consume(patch)`, `stop()`.

Concrete subclasses:

*  **BVWriter**(`filename`)

*  **IGSWriter**(`filename`)

###  `process_mesh(control_mesh, consumer, is_deg_raise=False)`

Runs the full pipeline in C++.

---

## Citation

Bi-cubic splines for polyhedral control nets, Jörg Peters , Kyle Lo, Kestutis Karciauskas TOMS , 2023 paper documentation