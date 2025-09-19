\page exe Standalone Executable
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