# Polyhedral Splines

## Table of Contents
1.  [Environment Setting](#environment-setting)
2.  [Building](#building)
3.  [Usage](#usage)
4.  [View .bv file](#view-bv-file)
5.  [Python Bindings](#python-bindings)
6.  [C# Bindings](#c-bindings)
7.  [Web Interface](#web-interface)
8.  [C++ Library](#c-library)
8.  [Citation](#citation)
9.  [License](#license)

# Environment Setting

### Tools & Dependencies
- [CMake](https://cmake.org/) (>=3.15)

**Note:**
- CMake will automatically download and install OpenMesh into `/External` using the source with the tested commit hash.
- The program should be compatible with equivalent or higher versions with little or no modification.

# Building

```shell
git clone https://bitbucket.org/surflab/polyhedral_splines.git
cd polyhedral_splines
cmake -B build
cmake --build build
```

# Usage

- **Input:**  quad-dominant mesh in .obj file format  
- **Output:** BB-patches written in .bv or .igs file format  

Please find [BView file introduction](https://www.cise.ufl.edu/research/SurfLab/bview/#file-format).

## Execution

**For UNIX-based systems**  
```shell
build/PolyhedralSplines [options] <input>
```

**For Windows (PowerShell)**  
```shell
build\Debug\PolyhedralSplines.exe [options] <input>
```

### Options
- `-h`, `--help`  
  Show this help message and exit.

- `-d`, `--DEGREE_RAISE`  
  Raise degree-2 patches to degree 3.

- `-f`, `--FORMAT <enum>`  
  Output format: `bv`, `igs` (default: `bv`)

### Positional arguments
- `input<string>`  
  Input file (required). Example: `mesh.obj`

### Usage Examples
- Convert `mesh.obj` and write BV format (default):  
  build/PolyhedralSplines mesh.obj

- Convert `mesh.obj`, raise all degree-2 patches to degree 3, and write BV format:  
  build/PolyhedralSplines -d mesh.obj

- Convert `mesh.obj` and write IGES format:  
  build/PolyhedralSplines -f igs mesh.obj

- Convert `mesh.obj`, raise degrees, and write IGES format:  
  build/PolyhedralSplines -d -f igs mesh.obj

> **Note:** test .obj files are in `/testfile`.

# View .bv file

Users can display `.bv` files with either the online or desktop version of [BView](https://www.cise.ufl.edu/research/SurfLab/bview/).  
BView provides tools such as zoom, group-wise patch coloring, Gaussian curvature, highlight lines, and more.

**Quick view of `.bv` file:**
1. Go to the [WebGL viewer](https://www.cise.ufl.edu/research/SurfLab/bview/webgl/).  
2. Click **Load File** → **Choose File** to import your `.bv` file.  
3. Under **Display**, set **Patch Detail** to 16×16.

# Python Bindings

See the [python README](python/README.md) for details.

# C# Bindings

See the [csharp README](csharp/README.md) for details.

# Web Interface

See the [wasm README](wasm/README.md) for building the web demo, or try the [live demo](https://cise.ufl.edu/~p.gupta/pns-web/).

# C++ Library

## Installation

After building, install the library into the default system location:
Note: Installing to system directories (e.g., /usr/local or C:\Program Files) requires administrator (root) privileges.

```shell
cmake --install build
```

### Typical Install Paths

**Unix**  
- Headers: `/usr/local/include/PolyhedralSplines`  
- Library: `/usr/local/lib/libPolyhedralSplinesLib.a` (static) or `/usr/local/lib/libPolyhedralSplinesLib.so` (shared)

**Windows**  
- Headers: `C:\Program Files\PolyhedralSplines\include`  
- Import Library: `C:\Program Files\PolyhedralSplines\bin\PolyhedralSplinesLib.lib`  
- DLL: `C:\Program Files\PolyhedralSplines\bin\PolyhedralSplinesLib.dll`

## Shared vs Static Builds

Control whether you build shared or static libraries with the `BUILD_SHARED_LIBS` option:

```shell
cmake -B build -DBUILD_SHARED_LIBS=ON    # build shared (.so / .dll)
cmake -B build -DBUILD_SHARED_LIBS=OFF   # build static (.a / .lib)
cmake --build build
cmake --install build
```

## OpenMesh Linking Details

OpenMesh is compiled statically into the PolyhedralSplines library:

- **On Unix:**  
  The OpenMesh code is bundled inside your `.a` or `.so`, so downstream projects don’t need to link OpenMesh separately.

- **On Windows:**  
  Although `PolyhedralSplinesLib.dll` contains the OpenMesh code at runtime, you **must** link against the statically built `OpenMeshCore.lib` and `OpenMeshTools.lib` when building your own application. These can be found in the `External/OpenMesh` foler. The `.lib` file holds the symbol definitions needed at link time.


## Citation

<i>Bi-cubic splines for polyhedral control nets</i>,  
Jörg Peters, Kyle Lo, Kestutis Karciauskas  
TOMS, 2023  
[paper](https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS.pdf)  
[documentation](https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS_Doc.pdf)

## License

This project is licensed under the [CC BY-NC](LICENSE).
