\page cpp_lib_compile Building C++ Library

## Installation

After [Building from Source](@ref compile) , install the library into the default system location:
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

If OpenMesh is built as a shared library, the binaries must be distributed alongside our library.

For Windows, projects using the PolyhedralSplines library functions outside the \ref api_group must also link against the OpenMesh `.lib` files.