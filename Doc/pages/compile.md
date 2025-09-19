\page compile Building from Source

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

# Build Options

The build can be customized by setting the following CMake variables:

- **BUILD_SHARED_LIBS**  
  - `OFF` (default) → Build the \ref cpp_lib_compile as a **static library** (`.a` on Unix, `.lib` on Windows).  
  - `ON` → Build the \ref cpp_lib_compile a **shared library** (`.so` on Unix, `.dll` on Windows).  

- **OPENMESH_BUILD_SHARED**  
  - `OFF` (default) → Build OpenMesh **statically**.  
    - OpenMesh code will be embedded directly into the standalone executable and library.  
    - No extra OpenMesh binaries need to be shipped.  
  - `ON` → Build OpenMesh as a **shared library**.  
    - On **Windows**, the OpenMesh DLLs must be next to the executable/library (or in `PATH`).  
    - On **Linux/macOS**, the `.so`/`.dylib` must be in the system library path (`LD_LIBRARY_PATH`, `DYLD_LIBRARY_PATH`, or installed into `/usr/lib` or `/usr/local/lib`).  
  - **BUILD_DOCS**  
    - `OFF` (default) → Documentation is not generated.  
    - `ON` → If Doxygen is installed, the documentation will be built automatically as part of the normal build process.  
    - Output is written to the `Doc/html` directory (configured by the Doxyfile).  
    - Open the `Doc/html/index.html` file in your browser to view the generated docs. 

### Example: enabling shared builds
```bash
cmake -B build -DBUILD_SHARED_LIBS=ON -DOPENMESH_BUILD_SHARED=ON -DBUILD_DOCS=ON
cmake --build build
```

# Other Builds

- \subpage cpp_lib_compile
- \subpage c_sharp_compile
- \subpage python_compile
- \subpage web_compile